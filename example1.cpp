/*
    sdlgui/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <atomic>
#include <sdlgui/screen.h>
#include <sdlgui/window.h>
#include <sdlgui/layout.h>
#include <sdlgui/label.h>
#include <sdlgui/checkbox.h>
#include <sdlgui/button.h>
#include <sdlgui/toolbutton.h>
#include <sdlgui/popupbutton.h>
#include <sdlgui/combobox.h>
#include <sdlgui/dropdownbox.h>
#include <sdlgui/progressbar.h>
#include <sdlgui/entypo.h>
#include <sdlgui/messagedialog.h>
#include <sdlgui/textbox.h>
#include <sdlgui/slider.h>
#include <sdlgui/imagepanel.h>
#include <sdlgui/imageview.h>
#include <sdlgui/vscrollpanel.h>
#include <sdlgui/colorwheel.h>
#include <sdlgui/graph.h>
#include <sdlgui/tabwidget.h>
#include <sdlgui/switchbox.h>
#include <sdlgui/formhelper.h>
#include <memory>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL_image.h>
#endif

using std::cout;
using std::cerr;
using std::endl;

#undef main

using namespace sdlgui;


void do_with_green_light_normal(Widget *widget, int choose)
{
  std::cout << "green light normal:" << choose << std::endl;
  if (choose != 2)
  {
    /* TODO change green light status */
  }
}
void do_with_green_light_mocode(Widget *widget, int choose)
{
  std::cout << "green light normal:" << choose << std::endl;
  if (choose != 2)
  {
    return;
  }
  printf("button widget=%p parent=%p\n", widget, widget->parent());
  printf("button widget window parent=%p\n", widget->window()->parent());
#if 1
  Window * setWindow = new Window(widget->window()->parent(), "www参数配置");
  /* 标记为 modal winow, 该 window 会提前到最前面图层 */
  //setWindow->setModal(true);
  setWindow->setLayout(new BoxLayout(Orientation::Vertical,
                          Alignment::Middle, 0, 15));

  Widget * configWidget = setWindow->add<Widget>();
  GridLayout * layout = new GridLayout(Orientation::Horizontal, 2,
                                 Alignment::Middle, 15, 5);
  layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
  layout->setSpacing(0, 10);
  /* 定义了这个窗口的布局 */
  configWidget->setLayout(layout);
  configWidget->add<Label>("设备 IP :", "sans-bold");
  /* 创建 textBox */
  auto& textBox = configWidget->wdg<TextBox>();
  textBox.setEditable(true);
  /* 设置控件大小 */
  textBox.setFixedSize(Vector2i(100, 20));
  textBox.setValue("50");
  textBox.setUnits("GiB");
  textBox.setDefaultValue("0.0");
  /* 设置字体大小 */
  textBox.setFontSize(16);
  textBox.setFormat("[-]?[0-9]*\\.?[0-9]+");
  textBox.setAlignment(TextBox::Alignment::Left);

  configWidget->add<Label>("摄像头1 IP:", "sans-bold");
  auto& textBox2 = configWidget->wdg<TextBox>("", "", KeyboardType::NumberIP);
  textBox2.setEditable(true);
  textBox2.setFixedSize(Vector2i(150, 20));
  textBox2.setValue("192.168.255.1");
  textBox2.setFontSize(16);
  //textBox2.setFormat("[1-9][0-9]*");
  textBox2.setAlignment(TextBox::Alignment::Left);
  //widget->parent()->parent()->setVisible(false);

  Widget *btWidget = setWindow->add<Widget>();
  btWidget->setLayout(new BoxLayout(Orientation::Horizontal,
                                  Alignment::Middle, 0, 15));
  btWidget->add<Button>("返回")->setWidgetCallback([](Widget *widget){
      widget->window()->dispose();
      std::cout << "返回" << std::endl;
  });
  btWidget->add<Button>("确认")->setWidgetCallback([](Widget *widget){
      widget->window()->dispose();
      std::cout << "确认" << std::endl;
  });
  red_debug_lite("window parent %p ww parent %p", widget->window()->parent(), widget->window()->parent());

  Screen * screen = dynamic_cast<Screen *>(widget->window()->parent());
  red_debug_lite("screen=%p", screen);
  SDL_Renderer * render = screen->sdlRenderer();
  screen->performLayout(render);
  setWindow->center();
  red_debug_lite("setWindow size=%d,%d", setWindow->width(), setWindow->height());
  setWindow->requestFocus();
#else
  MessageDialog *msg = dynamic_cast<MessageDialog *>(widget->parent()->parent());
  msg->add<Label>("oh no");
  Widget *wdg = dynamic_cast<Widget *>(widget->parent());
  wdg->add<Label>("oh no");
#endif
//setWindow.add<Label>("oh no", "sans");
  //std::cout << "what's wrong:" << choose << "setWindow:" << &setWindow << std::endl;

  //setWindow.requestFocus();
}

/* 测试窗口类 */
class TestWindow : public Screen
{
public:
    TestWindow( SDL_Window* pwindow, int rwidth, int rheight )
      : Screen( pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
      {
        /* 设备状态窗口 */
        {
          auto& swindow = wdg<Window>("设备状态");
          swindow.setId("sWindow");
          printf("swindow addr=%p\n", &swindow);

          /* 确定了 swindow 的位置 */
          swindow.withPosition({0, 550});
          /* 创建一个新的布局 */
          auto* layout = new GridLayout(Orientation::Horizontal, 6,
                                         Alignment::Minimum, 15, 5);
          layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
          layout->setSpacing(0, 10);
          /* 定义了这个窗口的布局 */
          swindow.setLayout(layout);

          /* 调用 add 函数模板
           * 将新创建的 label 控件关联到 swindow 作为其 parent
           * */
          swindow.add<Label>("        ", "sans-bold");
          swindow.add<Label>("设备状态", "sans-bold");
          swindow.add<Label>("水平/垂直角度(糖)", "sans-bold");
          swindow.add<Label>("水平/垂直速度(糖/s)", "sans-bold");
          swindow.add<Label>("莫码信息", "sans-bold");
          swindow.add<Label>("绿灯状态", "sans-bold");

          swindow.add<Label>("灯光装置终端一", "sans-bold");
          swindow.add<Label>("在线", "sans");
          swindow.add<Label>("120/10", "sans");
          swindow.children()[8]->setId("hspeed");
          //printf("size=%d id=%s\n", swindow.childCount(), swindow.children()[8]->id().c_str());
          swindow.add<Label>("20/10", "sans");
          swindow.add<Label>("GO", "sans");
          swindow.add<Label>("已授权", "sans");

          swindow.add<Label>("灯光装置终端二", "sans-bold");
          swindow.add<Label>("离线", "sans");
          swindow.add<Label>("-/-", "sans");
          swindow.add<Label>("-/-", "sans");
          swindow.add<Label>("--", "sans");
          swindow.add<Label>("已授权", "sans");
        }

        /* 小部件网格 */
        {
          auto& cwindow = wdg<Window>("灯光功能");

          /* 确定了 cwindow 的位置 */
          cwindow.withPosition({0, 670});
          /* 创建一个新的布局 */
          GridLayout * layout = new GridLayout(Orientation::Horizontal, 4,
                                         Alignment::Middle, 15, 5);
          layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
          layout->setSpacing(0, 10);
          /* 定义了这个窗口的布局 */
          cwindow.setLayout(layout);
          cwindow.add<Label>("绿灯", "sans-bold");
          cwindow.add<Button>("关闭", [&] {
              msgdialog(MessageDialog::Type::Question, "绿灯控制", "确认要打开绿光么?", "确认", "取消", do_with_green_light_normal); });
          cwindow.add<Button>("绿闪");
          cwindow.add<Button>("莫码", [&] {
              msgdialog(MessageDialog::Type::Choose, "莫码发送设置", "准备发送莫码?",
              do_with_green_light_mocode); });

          cwindow.add<Label>("白灯", "sans-bold");
          cwindow.add<Button>("常亮");
          cwindow.add<Button>("白闪");
          cwindow.add<Button>("莫码");
        }

        /* 小部件网格 */
        {
          auto& window = wdg<Window>("测试 window");

          /* 确定了 window 的位置 */
          window.withPosition({100, 100});
          /* 创建一个新的布局 */
          auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                         Alignment::Middle, 15, 5);
          layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
          layout->setSpacing(0, 10);
          /* 定义了这个窗口的布局 */
          window.setLayout(layout);

          /* 调用 add 函数模板
           * 将新创建的 label 控件关联到 window 作为其 parent
           * */
          window.add<Label>("浮点数测试 :", "sans-bold");
          /* 创建 textBox */
          auto& textBox = window.wdg<TextBox>();
          printf("textbox addr=%p\n", &textBox);
          textBox.setEditable(true);
          /* 设置控件大小 */
          textBox.setFixedSize(Vector2i(100, 20));
          textBox.setValue("50");
          textBox.setUnits("GiB");
          textBox.setDefaultValue("0.0");
          /* 设置字体大小 */
          textBox.setFontSize(16);
          textBox.setFormat("[-]?[0-9]*\\.?[0-9]+");
          textBox.setAlignment(TextBox::Alignment::Left);

          window.add<Label>("设备IP :", "sans-bold");
          auto& textBox2 = window.wdg<TextBox>("", "", KeyboardType::NumberIP);
          textBox2.setEditable(true);
          textBox2.setFixedSize(Vector2i(150, 20));
          textBox2.setValue("192.168.255.1");
          textBox2.setFontSize(16);
          //textBox2.setFormat("[1-9][0-9]*");
          textBox2.setAlignment(TextBox::Alignment::Left);

          //auto* key_layout = new GridLayout(Orientation::Horizontal, 2,
          //                               Alignment::Middle, 15, 3);
          //textBox2.keyboard().setLayout(key_layout);
          //textBox2.keyboard().setFixedSize(Vector2i(50, 50));
                      //.withLayout<GroupLayout>();

          window.add<Label>( "Checkbox :", "sans-bold");

          window.wdg<CheckBox>("Check me")
                .withChecked(true)
                .withFontSize(16);

          window.add<Label>("Combo box :", "sans-bold");
          window.wdg<ComboBox>()
                .withItems(std::vector<std::string>{ "Item 1", "Item 2", "Item 3" })
                .withFontSize(16)
                .withFixedSize(Vector2i(100,20));

          window.add<Label>("颜色按键 :", "sans-bold");
          /* 创建一个 popup button 按键 */
          auto& popupBtn = window.wdg<PopupButton>("", 0);
          popupBtn.setBackgroundColor(Color(255, 120, 0, 255));
          popupBtn.setFontSize(16);
          popupBtn.setFixedSize(Vector2i(100, 20));
          /*
           * 展开这个模板类 withLayout
           *     setLayout(new GroupLayout()) 使用 GroupLayout 初始化 popup 窗口
           * */
          auto& popup = popupBtn.popup().withLayout<GroupLayout>();

          /* 创建一个 color wheel,关联 parent 为 popup */
          ColorWheel& colorwheel = popup.wdg<ColorWheel>();
          colorwheel.setColor(popupBtn.backgroundColor());

          Button& colorBtn = popup.wdg<Button>("Pick");
          colorBtn.setFixedSize(Vector2i(100, 25));
          Color c = colorwheel.color();
          colorBtn.setBackgroundColor(c);

          colorwheel.setCallback([&colorBtn](const Color &value) {
              colorBtn.setBackgroundColor(value);
          });

          colorBtn.setChangeCallback([&colorBtn, &popupBtn](bool pushed) {
              if (pushed) {
                  popupBtn.setBackgroundColor(colorBtn.backgroundColor());
                  popupBtn.setPushed(false);
              }
          });
        }
        /* 确定每一个部件的大小 */
        performLayout(mSDL_Renderer);
    }

    ~TestWindow() {
    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers)
    {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;

        //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        // {
        //    setVisible(false);
        //    return true;
        //}
        return false;
    }

    virtual void draw(SDL_Renderer* renderer)
    {
      if (auto pbar = gfind<ProgressBar>("progressbar"))
      {
        /* 更新 progressbar 进度条更新 */
        pbar->setValue(pbar->value() + 0.001f);
        if (pbar->value() >= 1.f)
          pbar->setValue(0.f);
      }

      Screen::draw(renderer);
    }

    virtual void drawContents()
    {
    }
private:
    std::vector<SDL_Texture*> mImagesData;
    int mCurrentImage;
};


class Fps
{
public:
  explicit Fps(int tickInterval = 30)
      : m_tickInterval(tickInterval)
      , m_nextTime(SDL_GetTicks() + tickInterval)
  {
  }

  void next()
  {
    SDL_Delay(getTicksToNextFrame());

    m_nextTime += m_tickInterval;
  }

private:
  const int m_tickInterval;
  Uint32 m_nextTime;

  Uint32 getTicksToNextFrame() const
  {
    Uint32 now = SDL_GetTicks();

    return (m_nextTime <= now) ? 0 : m_nextTime - now;
  }
};


int main(int /* argc */, char ** /* argv */)
{
    char rendername[256] = {0};
    SDL_RendererInfo info;

    SDL_Init(SDL_INIT_VIDEO);   // Initialize SDL2

    SDL_Window *window;        // Declare a pointer to an SDL_Window

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int winWidth = 1080;
    int winHeight = 800;
    printf("w=%d h=%d\n", winWidth, winHeight);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
      "An SDL2 window",         //    const char* title
      SDL_WINDOWPOS_UNDEFINED,  //    int x: initial x position
      SDL_WINDOWPOS_UNDEFINED,  //    int y: initial y position
      winWidth,                      //    int w: width, in pixels
      winHeight,                      //    int h: height, in pixels
      SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN  | SDL_WINDOW_ALLOW_HIGHDPI        //    Uint32 flags: window options, see docs
    );

    // Check that the window was successfully made
    if(window==NULL){
      // In the event that the window could not be made...
      std::cout << "Could not create window: " << SDL_GetError() << '\n';
      SDL_Quit();
      return 1;
    }

    auto context = SDL_GL_CreateContext(window);

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++) {
        SDL_GetRenderDriverInfo(it, &info);
        strcat(rendername, info.name);
        strcat(rendername, " ");
    }
    printf("rendername:%s\n", rendername);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    /* 创建了测试窗口类 */
    TestWindow *screen = new TestWindow(window, winWidth, winHeight);

    Fps fps;

    bool quit = false;
    try
    {
        SDL_Event e;
        while( !quit )
        {
            //Handle events on queue
            while( SDL_PollEvent( &e ) != 0 )
            {
                //User requests quit
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                /* 处理事件 */
                screen->onEvent( e );
            }

            SDL_SetRenderDrawColor(renderer, 0xd5, 0xe8, 0xd3, 0xff);
            SDL_RenderClear( renderer );

            /* 绘制内容 */
            screen->drawAll();
            Window * swindow = dynamic_cast<Window *>(screen->gfind("sWindow"));
            static int test;
            if (swindow)
            {
              if (test++ % 30 == 1)
              {
                Label *hspeed_value = dynamic_cast<Label *>(swindow->gfind("hspeed"));
                hspeed_value->setCaption(std::to_string(test) + '/' + std::to_string(test+1));
              }
            }

            // Render the rect to the screen
            SDL_RenderPresent(renderer);

            fps.next();
        }
    }
    catch (const std::runtime_error &e)
    {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }
    return 0;
}
