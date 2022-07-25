/*
    sdlgui/keyboard.cpp -- Simple keyboard widget which is attached to another given
    window (can be nested)

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

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

  AsyncTexture(int _id) : id(_id) {};

  void load(Keyboard* ptr, int dx)
  {
    Keyboard* pp = ptr;
    AsyncTexture* self = this;
    /* 创建了一个线程去做这些事情 */
    std::thread tgr([=]() {
      std::lock_guard<std::mutex> guard(pp->theme()->loadMutex);

      NVGcontext *ctx = nullptr;
      int realw, realh;
      /* 传递的参数都是引用,这个函数比较重要 */
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
    : Window(parent, ""), mParentWindow(parentWindow),
      mAnchorPos(Vector2i::Zero()), mAnchorHeight(30)
{
  if (type == KeyboardType::Number)
  {
    setLayout(new GridLayout(Orientation::Horizontal, 3, Alignment::Middle, 5, 5));
    this->wdg<Button>("1").setCallback([]() {
                printf("num 1 pushed\n");
        });
    Button *button2 = new Button(this, "2");
    button2->setCallback([]{printf("num 2 catched\n");});
    Button *button3 = new Button(this, "3");
    Button *button4 = new Button(this, "4");
    Button *button5 = new Button(this, "5");
    Button *button6 = new Button(this, "6");
    Button *button7 = new Button(this, "7");
    Button *button8 = new Button(this, "8");
    Button *button9 = new Button(this, "9");
    Button *button_del = new Button(this, "", ENTYPO_ICON_LEFT_THIN);
    Button *button0 = new Button(this, "0");
    Button *button_ok = new Button(this, "↵");
    button_ok->setFixedSize(button0->size());
    button_del->setFixedSize(button0->size());
  }
}

/*
 * ctx 传递的是引用
 * */
void Keyboard::rendereBodyTexture(NVGcontext*& ctx, int& realw, int& realh, int dx)
{
  int ww = width();
  int hh = height();
  int ds = mTheme->mWindowDropShadowSize;
  int dy = 0;

  printf("ww=%d hh=%d\n", ww, hh);
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

  /* 这三个函数是在做什么 */
  nvgMoveTo(ctx, base.x + 15 * sign, base.y);
  nvgLineTo(ctx, base.x, base.y - 15);
  nvgLineTo(ctx, base.x, base.y + 15);

  nvgFillColor(ctx, mTheme->mWindowKeyboard.toNvgColor());
  nvgFill(ctx);
  nvgEndFrame(ctx);
}

void Keyboard::performLayout(SDL_Renderer *ctx) 
{
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
   * 固定的匹配 id == 1 ???
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
    AsyncTexturePtr newtx = std::make_shared<AsyncTexture>(id);
    /* 添加新创建的 AsyncTexturePtr 到向量尾部
     * 执行这个 load 函数,这个函数会执行一次
     * */
    newtx->load(this, _anchorDx);
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

  drawBody(renderer);
  
  Widget::draw(renderer);
}

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
            printf("get here now\n");
        }
    }

    printf("get here right\n");
    return false;
}

NAMESPACE_END(sdlgui)
