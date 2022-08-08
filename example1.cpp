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
#include <sdlgui/videoview.h>
#include <sdlgui/vscrollpanel.h>
#include <sdlgui/colorwheel.h>
#include <sdlgui/graph.h>
#include <sdlgui/tabwidget.h>
#include <sdlgui/switchbox.h>
#include <sdlgui/formhelper.h>
#include <memory>

#include <rapidjson/pointer.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/document.h>     // rapidjson's DOM-style API
#include <rapidjson/prettywriter.h> // for stringify JSON
#include <rapidjson/writer.h>
#include <cstdio>
#include <iostream>

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

#define LED3000_VERSION    9u
#define LED3000_ID         11000001u

using namespace sdlgui;
using namespace rapidjson;

void do_with_power_off(Widget *widget, int choose)
{
  std::cout << "do with power off :" << choose << std::endl;
  if (choose != 2)
  {
    /* TODO change green light status */
  }
}

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
  Window * setWindow = new Window(widget->window()->parent(), "系统参数配置");
  Window * mocodeWindow = widget->window();
  /* 标记为 modal winow, 该 window 会提前到最前面图层 */
  mocodeWindow->setModal(false);
  setWindow->setModal(true);
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
      Widget *wdg = widget->window()->parent()->gfind("莫码发送设置");
      Window * wnd = dynamic_cast<Window *>(wdg);
      wnd->setModal(true);
      widget->window()->dispose();
      std::cout << "确认" << std::endl;
  });

  Screen * screen = dynamic_cast<Screen *>(widget->window()->parent());
  SDL_Renderer * render = screen->sdlRenderer();
  screen->performLayout(render);
  setWindow->center();
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
      : Screen( pwindow, Vector2i(rwidth, rheight), "SDL_gui Test"),
        mFileName("/opt/led3000.json"), mFp(nullptr)
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
          /* 这里会弹出来新的 MessageDialog, 新的 MessageDialog 支持弹出新的 Window
           * 测试发现这个 MessageDialog Widget 的 parent 竟然是 TestWindow * ？？？
           * */

          Button * mocodeBtb = cwindow.add<Button>("莫码", [&] {
              msgdialog(MessageDialog::Type::Choose, "莫码发送设置", "准备发送莫码?",
              do_with_green_light_mocode); });

          cwindow.add<Label>("白灯", "sans-bold");
          cwindow.add<Button>("常亮");
          cwindow.add<Button>("白闪");
          cwindow.add<Button>("莫码");
        }
Button * sysconfigBtb;
        /* 系统窗口 */
        {
          auto& swindow = wdg<Window>("系统功能");

          /* 确定了 swindow 的位置 */
          swindow.withPosition({800, 0});
          /* 创建一个新的布局 */
          GridLayout * layout = new GridLayout(Orientation::Horizontal, 1,
                                         Alignment::Middle, 5, 5);
          layout->setColAlignment({ Alignment::Fill, Alignment::Fill });
          //layout->setSpacing(0, 5);
          /* 定义了这个窗口的布局 */
          swindow.setLayout(layout);
          swindow.add<Button>("关机", [&] {
              msgdialog(MessageDialog::Type::Question, "关机", "确认要关机么?", "确认", "取消", do_with_power_off); });
          sysconfigBtb = swindow.add<Button>("系统设置", [&] {
              msgdialog(MessageDialog::Type::Choose, "系统参数配置", "准备配置参数",
              do_with_power_off); });
        }

        /* 设备选择 */
        {
          auto& chooseWindow = wdg<Window>("设备选择");

          /* 确定了 chooseWindow 的位置 */
          chooseWindow.withPosition({1000, 300});
          /* 创建一个新的布局 */
          GridLayout * layout = new GridLayout(Orientation::Horizontal, 1,
                                         Alignment::Middle, 5, 5);
          layout->setColAlignment({ Alignment::Fill, Alignment::Fill });
          /* 定义了这个窗口的布局 */
          chooseWindow.setLayout(layout);
          Button *devBtn = chooseWindow.add<Button>("灯光装置终端一", [&] { cout << "choose device 1" << endl; });
          /* 根据实际系统功能中按键大小，为了保持大小一致，修改设备选择按键大小保持一致 */
          devBtn->setFixedSize(Vector2i(165, 30));
          devBtn = chooseWindow.add<Button>("灯光装置终端二", [&] { cout << "choose device 2" << endl; });
          devBtn->setFixedSize(Vector2i(165, 30));
        }

        /* 转台功能 */
        {
          auto& turntableWindow = wdg<Window>("转台功能");

          /* 确定了 turntableWindow 的位置 */
          turntableWindow.withPosition({800, 600});
          /* 创建一个新的布局 */
          GridLayout * layout = new GridLayout(Orientation::Horizontal, 1,
                                         Alignment::Middle, 5, 5);
          layout->setColAlignment({ Alignment::Fill, Alignment::Fill });
          /* 定义了这个窗口的布局 */
          turntableWindow.setLayout(layout);
#if 0
          Button *devBtn = turntableWindow.add<Button>("目标检测", [&] { cout << "目标检测模式" << endl; });
          /* 根据实际系统功能中按键大小，为了保持大小一致，修改设备选择按键大小保持一致 */
          devBtn->setFixedSize(Vector2i(165, 30));
          devBtn = turntableWindow.add<Button>("手动", [&] { cout << "转台手动模式" << endl; });
          devBtn->setFixedSize(Vector2i(165, 30));
          devBtn = turntableWindow.add<Button>("复位", [&] { cout << "复位转台" << endl; });
          devBtn->setFixedSize(Vector2i(165, 30));
#else
        turntableWindow.label("")._and().button("目标检测").withFlags(Button::RadioButton)._and().button("手动").withFlags(Button::RadioButton);
        turntableWindow.button("复位").withFlags(Button::RadioButton);
#endif
        }

        /* 小部件网格 */
        {
          auto& miscwindow = wdg<Window>("测试 window");

          /* 确定了 window 的位置 */
          miscwindow.withPosition({100, 100});
          /* 创建一个新的布局 */
          auto* layout = new GridLayout(Orientation::Horizontal, 2,
                                         Alignment::Middle, 15, 5);
          layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
          layout->setSpacing(0, 10);
          /* 定义了这个窗口的布局 */
          miscwindow.setLayout(layout);

          /* 调用 add 函数模板
           * 将新创建的 label 控件关联到 miscwindow 作为其 parent
           * */
          miscwindow.add<Label>("浮点数测试 :", "sans-bold");
          /* 创建 textBox */
          auto& textBox = miscwindow.wdg<TextBox>();
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

          miscwindow.add<Label>("设备IP :", "sans-bold");
          auto& textBox2 = miscwindow.wdg<TextBox>("", "", KeyboardType::NumberIP);
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

          miscwindow.add<Label>( "Checkbox :", "sans-bold");

          miscwindow.wdg<CheckBox>("Check me")
                .withChecked(true)
                .withFontSize(16);

          miscwindow.add<Label>("Combo box :", "sans-bold");
          miscwindow.wdg<ComboBox>()
                .withItems(std::vector<std::string>{ "Item 1", "Item 2", "Item 3" })
                .withFontSize(16)
                .withFixedSize(Vector2i(100,20));

          miscwindow.add<Label>("颜色按键 :", "sans-bold");
          /* 创建一个 popup button 按键 */
          auto& popupBtn = miscwindow.wdg<PopupButton>("", 0);
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

          /* 加载图形到 images 这个 list */
          ListImages images = loadImageDirectory(SDL_GetRenderer(pwindow), "icons");
          /* 创建一个 popupbutton 控件,这个 widget 的 parent 是 miscwindow */
          auto& imagePanelBtn = miscwindow.popupbutton("Image Panel", ENTYPO_ICON_FOLDER);

          // Load all of the images by creating a GLTexture object and saving the pixel data.
          mCurrentImage = 0;
          /* 根据 icon.tex 构造一个新的 SDL_Texture* 对象并放置在 mImagesData 的尾部 */
          for (auto& icon : images) mImagesData.emplace_back(icon.tex);

          /* 创建一个新的 window 对象用来显示图片 */
          auto& img_window = window("摄像头一视频", Vector2i(675, 15));
          /* 设置各个方向的 margin 为 0 */
          img_window.setLayout(new GroupLayout(0,0,0,0));
          img_window.setSize(Vector2i(400, 300));

          /* 在这个 window 上创建一个 img_window 控件 */
          auto videoview = img_window.add<VideoView>(nullptr);

          auto& img2_window = window("摄像头二视频", Vector2i(675, 315));
          /* 设置各个方向的 margin 为 0 */
          img2_window.setLayout(new GroupLayout(0,0,0,0));
          img2_window.setSize(Vector2i(400, 300));

          /* 在这个 window 上创建一个 img2_window 控件 */
          img2_window.add<VideoView>(nullptr);
        }
        /* 确定每一个部件的大小 */
        performLayout(mSDL_Renderer);

          red_debug_lite("size=%d,%d ", sysconfigBtb->size().x, sysconfigBtb->size().y);
    }

    ~TestWindow() {
    }


    void init_json_file(void)
        {
            char jsonBuffer[4096] = "{}";
            mFp = fopen(mFileName.c_str(), "r");
            if (mFp)
            {
              fread(jsonBuffer, sizeof(char), sizeof(jsonBuffer), mFp);
              cout << "jsonBuffer:" << jsonBuffer << "@" << mFileName << endl;
              if (mDocument.ParseInsitu(jsonBuffer).HasParseError())
              {
                std::cout << "Invalid content of " << mFileName << std::endl;
                return;
              }
            }
            else if (!mFp)
            {
                mFp = fopen(mFileName.c_str(), "w+");
                assert(mFp);
                //fwrite(jsonBuffer, sizeof(char), strlen(jsonBuffer) + 1, mFp);
                assert(false == mDocument.ParseInsitu(jsonBuffer).HasParseError());
            }

            Document::AllocatorType& allocator = mDocument.GetAllocator();

            if (mDocument.FindMember("sys_config") == mDocument.MemberEnd())
            {
                Value sys_config(kObjectType);
                sys_config.AddMember("sys_version", LED3000_VERSION, allocator);
                sys_config.AddMember("sys_id", LED3000_ID, allocator);
                mDocument.AddMember("sys_config", sys_config, allocator);
                cout << "Add sys_config object" << endl;
            }
            else
            {
                unsigned sys_version = Pointer("/sys_config/sys_version").Get(mDocument)->GetInt();
                cout << "sys_version:" << sys_version / 100 << "." << sys_version % 100 << endl;
                cout << "sys_id:" << Pointer("/sys_config/sys_id").Get(mDocument)->GetInt() << endl;
            }

            if (mDocument.MemberEnd() == mDocument.FindMember("eths"))
            {
                Value eth_array(kArrayType);
                Value eth0_config(kObjectType);
                Value eth1_config(kObjectType);

                eth0_config.AddMember("name", "eth0", allocator);
                eth0_config.AddMember("ip", "10.20.52.110", allocator);
                eth0_config.AddMember("netmask", "255.255.255.0", allocator);
                eth0_config.AddMember("gateway", "10.20.52.1", allocator);

                eth1_config.AddMember("name", "eth1", allocator);
                eth1_config.AddMember("ip", "192.168.100.110", allocator);
                eth1_config.AddMember("netmask", "255.255.255.0", allocator);
                eth1_config.AddMember("gateway", "192.168.100.1", allocator);

                eth_array.PushBack(eth0_config, allocator);
                eth_array.PushBack(eth1_config, allocator);

                mDocument.AddMember("eths", eth_array, allocator);
                cout << "Add eths array" << endl;
            }
            else
            {
             // for (auto& m : mDocument.FindMember("eths").GetObject())
              const Value& test_eths = mDocument["eths"];
              //for (Value::ConstValueIterator itr = test_eths.Begin(); itr != test_eths.End(); ++itr)
              for (SizeType i = 0; i < test_eths.Size(); i++)
              {
                cout << test_eths[i]["name"].GetString() << endl;
              }
#if 0
                printf("Type of member %s is %s\n",
                m.name.GetString(), kTypeNames[m.value.GetType()]);
                cout << "sys_id:" << Pointer("/eths/sys_id").Get(document)->GetString() << endl;
#endif
            }
        
            if (mDocument.MemberEnd() == mDocument.FindMember("devices"))
            {
                Value device_array(kArrayType);
                Value device_config_template(kObjectType);
                Value white_led_config(kObjectType);
                Value green_led_config(kObjectType);
                Value turntable_config(kObjectType);
            
                device_config_template.AddMember("camera_ip", "192.168.100.64", allocator);
                white_led_config.AddMember("mode", 1, allocator);
                white_led_config.AddMember("normal_status", 0, allocator);
                white_led_config.AddMember("blink_freq", 1, allocator);
                white_led_config.AddMember("mocode", "", allocator);
            
                green_led_config.AddMember("mode", 1, allocator);
                green_led_config.AddMember("normal_status", 0, allocator);
                green_led_config.AddMember("blink_freq", 1, allocator);
                green_led_config.AddMember("mocode", "", allocator);
            
                turntable_config.AddMember("mode", 1, allocator);
                turntable_config.AddMember("track_target", 0, allocator);
            
                device_config_template.AddMember("white_led", white_led_config, allocator);
                device_config_template.AddMember("green_led", green_led_config, allocator);
                device_config_template.AddMember("turntable", turntable_config, allocator);
                Value device_config;
            
                for (int i = 0; i < 2; i++)
                {
                    /* deep copy */
                    device_config.CopyFrom(device_config_template, allocator);
                    device_array.PushBack(device_config, allocator);
                    cout << "device array init:" << i << endl;
                }

                mDocument.AddMember("devices", device_array, allocator);
                cout << "Add devices array" << endl;
            }
        
            //SetValueByPointer(document, "/eths/0/ip", "1.2.3.4");
            fclose(mFp);
        
            mFp = fopen(mFileName.c_str(), "w");
            FileWriteStream os(mFp, jsonBuffer, sizeof(jsonBuffer));
            cout << "jsonBuffer:" << jsonBuffer << "@" << mFileName.c_str() <<  endl;
            Writer<FileWriteStream> writer(os);
            mDocument.Accept(writer);
            fclose(mFp);
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
    Document mDocument;
    FILE *mFp;
    std::string mFileName;
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
    //screen->init_json_file();

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
