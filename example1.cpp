/*
    sdlgui/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

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

/* 测试窗口类 */
class TestWindow : public Screen
{
public:
    TestWindow( SDL_Window* pwindow, int rwidth, int rheight )
      : Screen( pwindow, Vector2i(rwidth, rheight), "SDL_gui Test")
      {
        /* 小部件网格 */
        {
          auto& window = wdg<Window>("Grid of small widgets");
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
          textBox.setEditable(true);
          /* 设置控件大小 */
          textBox.setFixedSize(Vector2i(100, 20));
          textBox.setValue("50");
          textBox.setUnits("GiB");
          textBox.setDefaultValue("0.0");
          /* 设置字体大小 */
          textBox.setFontSize(16);
          textBox.setFormat("[-]?[0-9]*\\.?[0-9]+");

          window.add<Label>("整形输入框 :", "sans-bold");
          auto& textBox2 = window.wdg<TextBox>();
          textBox2.setEditable(true);
          textBox2.setFixedSize(Vector2i(100, 20));
          textBox2.setValue("50");
          textBox2.setUnits("Mhz");
          textBox2.setDefaultValue("0.0");
          textBox2.setFontSize(16);
          textBox2.setFormat("[1-9][0-9]*");

          auto* key_layout = new GridLayout(Orientation::Horizontal, 2,
                                         Alignment::Middle, 15, 3);
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

    int winWidth = 800;
    int winHeight = 600;
    printf("w=%d h=%d\n", winWidth, winHeight);

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
      "An SDL2 window",         //    const char* title
      SDL_WINDOWPOS_UNDEFINED,  //    int x: initial x position
      SDL_WINDOWPOS_UNDEFINED,  //    int y: initial y position
      winWidth,                      //    int w: width, in pixels
      winHeight,                      //    int h: height, in pixels
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN  | SDL_WINDOW_ALLOW_HIGHDPI        //    Uint32 flags: window options, see docs
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

            SDL_SetRenderDrawColor(renderer, 0xd5, 0xe8, 0xd3, 0xff );
            SDL_RenderClear( renderer );

            /* 绘制内容 */
            screen->drawAll();

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
