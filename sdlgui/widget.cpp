/*
    sdlgui/widget.cpp -- Base class of all widgets

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/widget.h>
#include <sdlgui/layout.h>
#include <sdlgui/theme.h>
#include <sdlgui/window.h>
#include <sdlgui/screen.h>
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL.h>
#endif

NAMESPACE_BEGIN(sdlgui)

Widget::Widget(Widget *parent)
    : mParent(nullptr), mTheme(nullptr), mLayout(nullptr),
      _pos(Vector2i::Zero()), mSize(Vector2i::Zero()),
      mFixedSize(Vector2i::Zero()), mVisible(true), mEnabled(true),
      mFocused(false), mMouseFocus(false), mTooltip(""), mFontSize(-1.0f),
      mCursor(Cursor::Arrow) 
{
    if (parent)
        parent->addChild(this);
}

Widget::~Widget() 
{
    for (auto child : mChildren) 
    {
        if (child)
            child->decRef();
    }
}

void Widget::setTheme(Theme *theme) 
{
    if (mTheme.get() == theme)
        return;
    mTheme = theme;
    for (auto child : mChildren)
        child->setTheme(theme);
}

int Widget::fontSize() const 
{
    return (mFontSize < 0 && mTheme) 
                      ? mTheme->mStandardFontSize 
                      : mFontSize;
}

Vector2i Widget::preferredSize(SDL_Renderer *ctx) const 
{
    if (mLayout)
        return mLayout->preferredSize(ctx, this);
    else
        return mSize;
}

void Widget::performLayout(SDL_Renderer *ctx) 
{
  /* 如果定义了 mLayout 布局，那么直接使用这个布局初始化这个对象 */
    if (mLayout) 
    {
        mLayout->performLayout(ctx, this);
    } 
    else 
    {
      /* 遍历 mChildren 向量 */
        for (auto c : mChildren) 
        {
          /* 获取 preferredSize 和 fixedsize */
          Vector2i pref = c->preferredSize(ctx), fix = c->fixedSize();
          /* 如果有 fixedsize 那么使用 fixedsize
           * 否则使用 preferredSize
           * 这个步骤是确定大小
           * */
            c->setSize(Vector2i(
                fix[0] ? fix[0] : pref[0],
                fix[1] ? fix[1] : pref[1]
            ));
            /* 执行 children 的 performLayout 函数 */
            c->performLayout(ctx);
        }
    }
}

Widget* Widget::find(const std::string& id, bool inchildren)
{
  if (mId == id)
    return this;

  if (inchildren)
  {
    for (auto* child : mChildren)
    {
      Widget* w = child->find(id, inchildren);
      if (w)
        return w;
    }
  }

  return nullptr;
}

Widget *Widget::findWidget(const Vector2i &p)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (child->visible() && child->contains(p - _pos))
            return child->findWidget(p - _pos);
    }
    return contains(p) ? this : nullptr;
}

bool Widget::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (child->visible() && child->contains(p - _pos) &&
            /* 回调 child 的 mouseButtonEvent 函数 */
            child->mouseButtonEvent(p - _pos, button, down, modifiers))
            return true;
    }
    
    if (button == SDL_BUTTON_LEFT && down && !mFocused)
        requestFocus();
    return false;
}

bool Widget::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (!child->visible())
            continue;
        bool contained = child->contains(p - _pos);
        bool prevContained = child->contains(p - _pos - rel);
        if (contained != prevContained)
            child->mouseEnterEvent(p, contained);
        if ((contained || prevContained) &&
            child->mouseMotionEvent(p - _pos, rel, button, modifiers))
            return true;
    }
    return false;
}

bool Widget::scrollEvent(const Vector2i &p, const Vector2f &rel)
{
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (!child->visible())
            continue;
        if (child->contains(p - _pos) && child->scrollEvent(p - _pos, rel))
            return true;
    }
    return false;
}

bool Widget::mouseDragEvent(const Vector2i &, const Vector2i &, int, int)
{
    return false;
}

bool Widget::mouseEnterEvent(const Vector2i &, bool enter)
{
    mMouseFocus = enter;
    return false;
}

bool Widget::focusEvent(bool focused) 
{
    mFocused = focused;
    return false;
}

bool Widget::keyboardEvent(int, int, int, int) 
{
    return false;
}

bool Widget::keyboardCharacterEvent(unsigned int) 
{
    return false;
}

void Widget::addChild(int index, Widget * widget) 
{
    assert(index <= childCount());
    mChildren.insert(mChildren.begin() + index, widget);
    widget->incRef();
    widget->setParent(this);
	/* 关联 theme */
    widget->setTheme(mTheme);
}

void Widget::addChild(Widget * widget) 
{
    addChild(childCount(), widget);
}

void Widget::removeChild(const Widget *widget) 
{
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), widget), mChildren.end());
    widget->decRef();
}

void Widget::removeChild(int index) 
{
    Widget *widget = mChildren[index];
    mChildren.erase(mChildren.begin() + index);
    widget->decRef();
}

int Widget::childIndex(Widget *widget) const 
{
    auto it = std::find(mChildren.begin(), mChildren.end(), widget);
    if (it == mChildren.end())
        return -1;
    return it - mChildren.begin();
}

Window *Widget::window() 
{
    Widget *widget = this;
    while (true) 
    {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent window)");
        Window *window = dynamic_cast<Window *>(widget);
        if (window)
            return window;
        widget = widget->parent();
    }
}

int Widget::getAbsoluteLeft() const
{
  return mParent
    ? mParent->getAbsoluteLeft() + _pos.x
    : _pos.x;
}

SDL_Point Widget::getAbsolutePos() const
{
  if (mParent)
  {
    SDL_Point p = mParent->getAbsolutePos();
    return SDL_Point{ p.x + _pos.x, p.y + _pos.y };
  }
  else
    return SDL_Point{ _pos.x, _pos.y };
}

PntRect Widget::getAbsoluteCliprect() const
{
  if (mParent)
  {
    PntRect pclip = mParent->getAbsoluteCliprect();
    SDL_Point pp = getAbsolutePos();
    PntRect mclip{ pp.x, pp.y, pp.x + width(), pp.y + height() };
    if (pclip.x1 < mclip.x1)
      pclip.x1 = mclip.x1;
    if (pclip.y1 < mclip.y1)
      pclip.y1 = mclip.y1;
    if (mclip.x2 < pclip.x2)
      pclip.x2 = mclip.x2;
    if (mclip.y2 < pclip.y2)
      pclip.y2 = mclip.y2;

    return pclip;
  }
  else
  {
    return PntRect{ _pos.x, _pos.y, _pos.x + width(), _pos.y + height()};
  }
}

int Widget::getAbsoluteTop() const
{
  return mParent
    ? mParent->getAbsoluteTop() + _pos.y
    : _pos.y;
}

/* 更新 focus 到当前窗口 */
void Widget::requestFocus() 
{
    Widget *widget = this;
    /* 遍历更新当前 widget 的 parent 窗口 */
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->updateFocus(this);
}

void Widget::draw(SDL_Renderer* renderer)
{
  for (auto child : mChildren)
    if (child->visible())
      child->draw(renderer);
}

NAMESPACE_END(sdlgui)
