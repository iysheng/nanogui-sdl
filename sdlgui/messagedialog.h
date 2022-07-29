/*
    sdlgui/messagedialog.h -- Simple "OK" or "Yes/No"-style modal dialogs

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <sdlgui/window.h>

NAMESPACE_BEGIN(sdlgui)

class Label;

class  MessageDialog : public Window 
{
public:
    enum class Type 
    {
        Information,
        Question,
        Warning,
        Choose,
    };

#if 0
    MessageDialog(Widget *parent, Type type, const std::string &title = "Untitled",
                  const std::string &message = "Message",
                  const std::string &buttonText = "OK",
                  const std::string &altButtonText = "Cancel", bool altButton = false);
#endif

    MessageDialog(Widget *parent, Type type, const std::string &title = "FuncChoice",
                  const std::string &message = "Make your choice",
                  const std::string &confirmButtonText = "确认",
                  const std::string &setButtonText = "参数配置",
                  const std::string &cancleButtonText = "取消", bool setButton = true);

    MessageDialog(Widget *parent, Type type, const std::string &title,
                  const std::string &message,
                  const std::string &confirmButtonText,
                  const std::string &cancleButtonText,
                  const std::function<void(int)> &callback )
      : MessageDialog(parent, type, title, message, confirmButtonText, "", cancleButtonText, false)
    { setCallback(callback); }

    MessageDialog(Widget *parent, Type type, const std::string &title,
      const std::string &message,
      const std::function<void(int)> &callback)
      : MessageDialog(parent, type, title, message)
    {
      setCallback(callback);
    }

    Label *messageLabel() { return mMessageLabel; }
    const Label *messageLabel() const { return mMessageLabel; }

    std::function<void(int)> callback() const { return mCallback; }
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }

    MessageDialog& withCallback(const std::function<void(int)> &callback)
    { setCallback( callback ); return *this; }
protected:
    std::function<void(int)> mCallback;
    Label *mMessageLabel;
};

NAMESPACE_END(sdlgui)
