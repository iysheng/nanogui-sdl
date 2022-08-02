/*
    sdlgui/messagedialog.cpp -- Simple "OK" or "Yes/No"-style modal dialogs

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/messagedialog.h>
#include <sdlgui/layout.h>
#include <sdlgui/button.h>
#include <sdlgui/entypo.h>
#include <sdlgui/label.h>
#include <array>

NAMESPACE_BEGIN(sdlgui)

#if 0
MessageDialog::MessageDialog(Widget *parent, Type type, const std::string &title,
              const std::string &message,
              const std::string &buttonText,
              const std::string &altButtonText, bool altButton) 
  : Window(parent, title)
{
    setLayout(new BoxLayout(Orientation::Vertical,
                            Alignment::Middle, 10, 10));
    setModal(true);

    Widget *panel1 = new Widget(this);
    panel1->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 10, 15));
    int icon = 0;
    switch (type)
    {
        case Type::Information: icon = ENTYPO_ICON_CIRCLED_INFO; break;
        case Type::Question: icon = ENTYPO_ICON_CIRCLED_HELP; break;
        case Type::Warning: icon = ENTYPO_ICON_WARNING; break;
    }
    Label *iconLabel = new Label(panel1, std::string(utf8(icon).data()), "icons");
    iconLabel->setFontSize(50);
    mMessageLabel = new Label(panel1, message);
    Widget *panel2 = new Widget(this);
    panel2->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 0, 15));

    if (altButton)
    {
        Button *button = new Button(panel2, altButtonText, ENTYPO_ICON_CIRCLED_CROSS);
        button->setCallback([&] { if (mCallback) mCallback(1); dispose(); });
    }
    Button *button = new Button(panel2, buttonText, ENTYPO_ICON_CHECK);
    button->setCallback([&] { if (mCallback) mCallback(0); dispose(); });
    /* 居中 */
    center();
    requestFocus();
}
#endif

MessageDialog::MessageDialog(Widget *parent, Type type, const std::string &title,
              const std::string &message,
              const std::string &confirmButtonText,
              const std::string &setButtonText,
              const std::string &cancleButtonText, bool setButton)

  : Window(parent, title)
{
    this->setId(title);
    setLayout(new BoxLayout(Orientation::Vertical,
                            Alignment::Middle, 10, 10));
    setModal(true);

    Widget *panel1 = new Widget(this);
    panel1->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 10, 15));
    int icon = 0;
    switch (type)
    {
        case Type::Information: icon = ENTYPO_ICON_CIRCLED_INFO; break;
        case Type::Question: icon = ENTYPO_ICON_CIRCLED_HELP; break;
        case Type::Warning: icon = ENTYPO_ICON_WARNING; break;
        case Type::Choose: icon = ENTYPO_ICON_CIRCLED_HELP; break;
    }
    Label *iconLabel = new Label(panel1, std::string(utf8(icon).data()), "icons");
    iconLabel->setFontSize(50);
    mMessageLabel = new Label(panel1, message);
    Widget *panel2 = new Widget(this);
    panel2->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 0, 15));

    Button *button = new Button(panel2, cancleButtonText, ENTYPO_ICON_CROSS);
    button->setCallback([&] { if (mCallback) mCallback(NULL, 0); dispose(); });
    if (setButton)
    {
        mSetButton = new Button(panel2, setButtonText);
        mSetButton->setCallback([&] { if (mCallback) mCallback(mSetButton, 2); //dispose();
        });
    }
    button = new Button(panel2, confirmButtonText, ENTYPO_ICON_CHECK);
    button->setCallback([&] { if (mCallback) mCallback(NULL, 1); dispose(); });
#if 0
        mSetButton, mSetButton->parent(), this);
#endif
    /* 居中 */
    center();
    requestFocus();
}

NAMESPACE_END(sdlgui)
