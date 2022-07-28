/*
    sdlgui/keyboard.cpp -- Simple keyboard widget which is attached to another given
    window (can be nested)

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/textbox.h>
#include <sdlgui/keyboard.h>
#include <sdlgui/entypo.h>
#include <sdlgui/theme.h>
#include <thread>

#include "nanovg.h"
#define NANOVG_RT_IMPLEMENTATION
#define NANORT_IMPLEMENTATION
#include "nanovg_rt.h"

NAMESPACE_BEGIN(sdlgui)

  /* keyboard 的 AsyncTexture 结构体 */
struct Keyboard::AsyncTexture
{
  int id;
  Texture tex;
  NVGcontext* ctx = nullptr;

  /* 构造函数，指定 id */
  AsyncTexture(int _id) : id(_id) {};

  /* 加载键盘的主体，也就是这个窗口本身，不包括这个窗口上的 button */
  void load(Keyboard* ptr, int dx)
  {
    Keyboard* pp = ptr;
    AsyncTexture* self = this;
    /* 创建了一个线程去做这些事情 */
    std::thread tgr([=]() {
      std::lock_guard<std::mutex> guard(pp->theme()->loadMutex);

      NVGcontext *ctx = nullptr;
      int realw, realh;
      /* 传递的参数都是引用,这个函数比较重要
       * 绘制 keyboard 的主体内容，没有渲染 mChildren 控件
       * */
      pp->rendereBodyTexture(ctx, realw, realh, dx);
      self->tex.rrect = { 0, 0, realw, realh };
      /* 关联这个矢量图到 keyboard */
      self->ctx = ctx;
    });

    tgr.detach();
  }

  void perform(SDL_Renderer* renderer)
  {
    if (!ctx)
      return;

    unsigned char *rgba = nvgReadPixelsRT(ctx);

    tex.tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, tex.w(), tex.h());

    int pitch;
    uint8_t *pixels;
    int ok = SDL_LockTexture(tex.tex, nullptr, (void **)&pixels, &pitch);
    memcpy(pixels, rgba, sizeof(uint32_t) * tex.w() * tex.h());
    SDL_SetTextureBlendMode(tex.tex, SDL_BLENDMODE_BLEND);
    SDL_UnlockTexture(tex.tex);

    nvgDeleteRT(ctx);
    ctx = nullptr;
  }

};

Keyboard::Keyboard(Widget *parent, Window *parentWindow, KeyboardType type)
    : Window(parent, ""), mParentWindow(parentWindow), mKeyboardType(type),
      mAnchorPos(Vector2i::Zero()), mAnchorHeight(30) /* 默认锚点位置，在 Y 轴向下 30 像素的位置 */
{
  if (type == KeyboardType::Number)
  {
    setLayout(new GridLayout(Orientation::Horizontal, 3, Alignment::Middle, 5, 5));
    this->wdg<Button>("1").setWidgetCallback([](Widget *widget) {
        Keyboard *keyboard = dynamic_cast<Keyboard*>(widget);
        keyboard->mKeyboardValue.push_back('1');
        keyboard->getTextBox()->setValue(keyboard->mKeyboardValue);
        keyboard->getTextBox()->focusEvent(true);
        //((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("2").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('2');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("3").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('3');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("4").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('4');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("5").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('5');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("6").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('6');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("7").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('7');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("8").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('8');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("9").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('9');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    Button *button_del = new Button(this, "", ENTYPO_ICON_LEFT_THIN);
    button_del->setWidgetCallback([](Widget *widget){
      Keyboard *keyboard = dynamic_cast<Keyboard*>(widget);
      if (keyboard->mKeyboardValue.length())
      {
        keyboard->mKeyboardValue.pop_back();
        keyboard->getTextBox()->setValue(keyboard->mKeyboardValue);
        keyboard->getTextBox()->focusEvent(true);
      }
    });
    this->wdg<Button>("0").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('0');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    Button *button_ok = new Button(this, "↵");
    button_ok->setWidgetCallback([](Widget *widget) {
                printf("num ok pushed:%s\n", ((Keyboard *)widget)->mKeyboardValue.c_str());
                ((Keyboard *)widget)->setVisible(false);
                ((Keyboard *)widget)->parent()->requestFocus();
                ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        });
    /* 测试发现大小是 29，30 这里直接固定大小,但是随着字体大小的改变
     * 这个大小应该也要变化
     * */
    button_ok->setFixedSize(Vector2i(29, 30));
    button_del->setFixedSize(Vector2i(29, 30));
    printf("keyboard parent=%p parentWindow=%p\n", mParent, mParentWindow);
  }
  else if (type == KeyboardType::NumberIP)
  {
    AdvancedGridLayout *layout = new AdvancedGridLayout({30,30,30}, {30,30,30,30,30});
    layout->setMargin(0);
    //layout->setColStretch(2, 1);
    //layout->appendRow(15, 0.5f);
    //layout->appendCol(15, 0.5f);
    //Button *btn = new Button(this, "R");
    this->setLayout(layout);
    this->wdg<Button>("1").setWidgetCallback([](Widget *widget) {
        Keyboard *keyboard = dynamic_cast<Keyboard*>(widget);
        keyboard->mKeyboardValue.push_back('1');
        keyboard->getTextBox()->setValue(keyboard->mKeyboardValue);
        keyboard->getTextBox()->focusEvent(true);
        //((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("2").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('2');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("3").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('3');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("4").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('4');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("5").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('5');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("6").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('6');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("7").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('7');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("8").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('8');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>("9").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('9');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    Button *button_del = new Button(this, "", ENTYPO_ICON_LEFT_THIN);
    button_del->setWidgetCallback([](Widget *widget){
      Keyboard *keyboard = dynamic_cast<Keyboard*>(widget);
      if (keyboard->mKeyboardValue.length())
      {
        keyboard->mKeyboardValue.pop_back();
        keyboard->getTextBox()->setValue(keyboard->mKeyboardValue);
        keyboard->getTextBox()->focusEvent(true);
      }
    });
    this->wdg<Button>("0").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('0');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    this->wdg<Button>(".").setWidgetCallback([](Widget *widget) {
        ((Keyboard *)widget)->mKeyboardValue.push_back('.');
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        ((TextBox *)(((Keyboard *)widget)->getTextBox()))->focusEvent(true);
        });
    Button *button_ok = new Button(this, "↵");
    button_ok->setWidgetCallback([](Widget *widget) {
                printf("num ok pushed:%s\n", ((Keyboard *)widget)->mKeyboardValue.c_str());
                ((Keyboard *)widget)->setVisible(false);
                ((Keyboard *)widget)->parent()->requestFocus();
                ((TextBox *)(((Keyboard *)widget)->getTextBox()))->setValue(((Keyboard *)widget)->mKeyboardValue);
        });
    /* 测试发现大小是 29，30 这里直接固定大小,但是随着字体大小的改变
     * 这个大小应该也要变化
     * */
    //button_ok->setFixedSize(Vector2i(29 * 3, 30));
    button_del->setFixedSize(Vector2i(29, 30));

    int i = 0 , j = 0;
    for (; i < 4; i++)
      for (j = 0; j < 3; j++)
        layout->setAnchor(mChildren[i*3+j], AdvancedGridLayout::Anchor(j, i, 1, 1));
    layout->setAnchor(mChildren[i*3], AdvancedGridLayout::Anchor(0, i, 3, 1));
    printf("keyboard parent=%p parentWindow=%p\n", mParent, mParentWindow);
  }
}

/*
 * ctx 传递的是引用, 绘制键盘的主体内容
 * */
void Keyboard::rendereBodyTexture(NVGcontext*& ctx, int& realw, int& realh, int dx)
{
  int ww = width();
  int hh = height();
  int ds = mTheme->mWindowDropShadowSize;
  int dy = 0;

  Vector2i offset(dx + ds, dy + ds);

  realw = ww + 2 * ds + dx; //with + 2*shadow + offset
  realh = hh + 2 * ds + dy;

  /* 创建一个 ctx 用来绘图的画布, nanovg */
  ctx = nvgCreateRT(NVG_DEBUG, realw, realh, 0);

  float pxRatio = 1.0f;
  nvgBeginFrame(ctx, realw, realh, pxRatio);

  int cr = mTheme->mWindowCornerRadius;

  /* Draw a drop shadow */
  /* gradient : 梯度、斜坡 */
  NVGpaint shadowPaint = nvgBoxGradient(ctx, offset.x, offset.y, ww, hh, cr * 2, ds * 2,
    mTheme->mDropShadow.toNvgColor(),
    mTheme->mTransparent.toNvgColor());

  /* 绘制窗口的阴影部分？？ */
  nvgBeginPath(ctx);
  //nvgRect(ctx, offset.x - ds, offset.y - ds, ww + 2 * ds, hh + 2 * ds);
  // 定义了圆角矩形区
  nvgRoundedRect(ctx, offset.x - ds, offset.y - ds, ww + 2 * ds, hh + 2 * ds, cr);
  //nvgPathWinding(ctx, NVG_HOLE);
  nvgFillPaint(ctx, shadowPaint);
  nvgFill(ctx);

  /* Draw window */
  nvgBeginPath(ctx);
  nvgRoundedRect(ctx, offset.x, offset.y, ww, hh, cr);

  Vector2i base = Vector2i(offset.x + 0, offset.y + anchorHeight());
  int sign = -1;

  /* 定义线条开始坐标 */
  nvgMoveTo(ctx, base.x + 15 * sign, base.y);
  /* 定义线条结束坐标 */
  nvgLineTo(ctx, base.x, base.y - 15);
  /* 定义线条结束坐标 */
  nvgLineTo(ctx, base.x, base.y + 15);

  nvgFillColor(ctx, mTheme->mWindowKeyboard.toNvgColor());
  nvgFill(ctx);
  nvgEndFrame(ctx);
}

void Keyboard::performLayout(SDL_Renderer *ctx) 
{
    printf("%d keychildren size\n", mChildren.size());
    if (mLayout || mChildren.size() != 1) 
    {
        Widget::performLayout(ctx);
    } 
    else 
    {
        mChildren[0]->setPosition(Vector2i::Zero());
        mChildren[0]->setSize(mSize);
        mChildren[0]->performLayout(ctx);
    }
}

/* 获取控件的相对位置 */
void Keyboard::refreshRelativePlacement() 
{
    mParentWindow->refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();

    Widget *widget = this;
    while (widget->parent() != nullptr)
        widget = widget->parent();
    Screen *screen = (Screen *)widget;
    Vector2i screenSize = screen->size();

    _pos = mParentWindow->position() + mAnchorPos - Vector2i(0, mAnchorHeight);
    _pos = Vector2i(_pos.x, std::min(_pos.y, screen->size().y - mSize.y));
}

void Keyboard::drawBodyTemp(SDL_Renderer* renderer)
{
  int ds = mTheme->mWindowDropShadowSize;
  int cr = mTheme->mWindowCornerRadius;

  /* Draw a drop shadow */
  SDL_Color sh = mTheme->mDropShadow.toSdlColor();
  SDL_Rect shRect{ _pos.x - ds, _pos.y - ds, mSize.x + 2 * ds, mSize.y + 2 * ds };
  SDL_SetRenderDrawColor(renderer, sh.r, sh.g, sh.b, 64);
  SDL_RenderFillRect(renderer, &shRect);

  SDL_Color bg = mTheme->mWindowKeyboard.toSdlColor();
  SDL_Rect bgRect{ _pos.x, _pos.y, mSize.x, mSize.y };

  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
  SDL_RenderFillRect(renderer, &bgRect);

  SDL_Color br = mTheme->mBorderDark.toSdlColor();
  SDL_SetRenderDrawColor(renderer, br.r, br.g, br.b, br.a);

  SDL_Rect brr{ _pos.x - 1, _pos.y - 1, width() + 2, height() + 2 };
  SDL_RenderDrawLine(renderer, brr.x, brr.y, brr.x + brr.w, brr.y);
  SDL_RenderDrawLine(renderer, brr.x + brr.w, brr.y, brr.x + brr.w, brr.y + brr.h);
  SDL_RenderDrawLine(renderer, brr.x, brr.y + brr.h, brr.x + brr.w, brr.y + brr.h);
  SDL_RenderDrawLine(renderer, brr.x, brr.y, brr.x, brr.y + brr.h);

  // Draw window anchor
  SDL_SetRenderDrawColor(renderer, bg.r, bg.g, bg.b, bg.a);
  for (int i = 0; i < 15; i++)
  {
    SDL_RenderDrawLine(renderer, _pos.x - 15 + i, _pos.y + mAnchorHeight - i,
      _pos.x - 15 + i, _pos.y + mAnchorHeight + i);
  }
}

void Keyboard::drawBody(SDL_Renderer* renderer)
{
  int id = 1;

  /* 找到匹配的对象返回 [begin, end)
   * 固定的匹配 id == 1 ???, 这里为什么查找 id 是 1 呢？？？
   * 是因为创建这个对象的时候，直接指定了 id 是 1 ？？ 但是为什么不从 0
   * */
  auto atx = std::find_if(_txs.begin(), _txs.end(), [id](AsyncTexturePtr p) { return p->id == id; });

  /* 如果找到了匹配的对象 */
  if (atx != _txs.end())
  {
    (*atx)->perform(renderer);
    
    if ((*atx)->tex.tex)
      SDL_RenderCopy(renderer, (*atx)->tex, getOverrideBodyPos());
    else
      drawBodyTemp(renderer);
  }
  else
	  /* 第一次会走到这里 */
  {
    printf("This is first time, just create keyboard\n");
    /* 创建一个指定 id 的 vector 对象 */
    AsyncTexturePtr newtx = std::make_shared<AsyncTexture>(id);
    /* 添加新创建的 AsyncTexturePtr 到向量尾部
     * 执行这个 load 函数,这个函数会执行一次
     * */
    newtx->load(this, _anchorDx);
    /* 压入控件到 vector */
    _txs.push_back(newtx);
  }

}

Vector2i Keyboard::getOverrideBodyPos()
{
  Vector2i ap = absolutePosition();
  int ds = mTheme->mWindowDropShadowSize;
  return ap - Vector2i(_anchorDx + ds, ds);
}

void Keyboard::draw(SDL_Renderer* renderer)
{
  refreshRelativePlacement();

  if (!mVisible)
    return;

  /* 绘制 keyboard 的主体 */
  drawBody(renderer);
  
  /* 绘制 mChildren 控件 */
  Widget::draw(renderer);
}

/* 键盘控件的鼠标事件处理函数核心还是遍历 children 节点的 mouseButtonEvent 处理函数 */
bool Keyboard::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
    printf("catch keyboard (%d,%d)\n", (p - _pos).x, (p - _pos).y);
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) 
    {
        Widget *child = *it;
        if (child->visible() && child->contains(p - _pos) &&
            /* 回调 child 的 mouseButtonEvent 函数 */
            child->mouseButtonEvent(p - _pos, button, down, modifiers))
        {
            printf("get here now with keyboard\n");
            return true;
        }
    }

    printf("get here right\n");
    return false;
}

NAMESPACE_END(sdlgui)
