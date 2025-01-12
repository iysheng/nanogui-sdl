/*
    sdl_gui/widget.h -- Base class of all widgets

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <sdlgui/theme.h>
#include <sdlgui/layout.h>
#include <vector>

NAMESPACE_BEGIN(sdlgui)

class Window;
class Label;
class ToolButton;
class MessageDialog;
class PopupButton;
class Button;
class ComboBox;
class CheckBox;
class VScrollPanel;
class ProgressBar;
class Slider;
class ImagePanel;
class DropdownBox;
class TextBox;
/**
 * \class Widget widget.h sdl_gui/widget.h
 *
 * \brief Base class of all widgets.
 *
 * \ref Widget is the base class of all widgets in \c sdlgui. It can
 * also be used as an panel to arrange an arbitrary number of child
 * widgets using a layout generator (see \ref Layout).
 */

class  Widget : public Object 
{
public:
    /// Construct a new widget with the given parent widget
    Widget(Widget *parent);

    /// Return the parent widget
    Widget *parent() { return mParent; }
    /// Return the parent widget
    const Widget *parent() const { return mParent; }
    /// Set the parent widget
    void setParent(Widget *parent) { mParent = parent; }

    /// Return the used \ref Layout generator
    Layout *layout() { return mLayout; }
    /// Return the used \ref Layout generator
    const Layout *layout() const { return mLayout.get(); }
    /// Set the used \ref Layout generator
    void setLayout(Layout *layout) { mLayout = layout; }

    /// Return the \ref Theme used to draw this widget
    Theme *theme() { return mTheme; }
    /// Return the \ref Theme used to draw this widget
    const Theme *theme() const { return mTheme.get(); }
    /// Set the \ref Theme used to draw this widget
    virtual void setTheme(Theme *theme);

    /// Return the position relative to the parent widget
    const Vector2i &position() const { return _pos; }
    /// Set the position relative to the parent widget
    void setPosition(const Vector2i &pos) { _pos = pos; }
    void setPosition(int x, int y) { _pos = { x, y }; }

    /// Return the absolute position on screen
    Vector2i absolutePosition() const
    {
        return mParent 
                  ? (mParent->absolutePosition() + _pos) 
                  : _pos;
    }

    /// Return the size of the widget
    const Vector2i &size() const { return mSize; }
    /// set the size of the widget
    void setSize(const Vector2i &size) { mSize = size; }

    /// Return the width of the widget
    int width() const { return mSize.x; }
    /// Set the width of the widget
    void setWidth(int width) { mSize.x = width; }

    /// Return the height of the widget
    int height() const { return mSize.y; }
    /// Set the height of the widget
    void setHeight(int height) { mSize.y = height; }

    /**
     * \brief Set the fixed size of this widget
     *
     * If nonzero, components of the fixed size attribute override any values
     * computed by a layout generator associated with this widget. Note that
     * just setting the fixed size alone is not enough to actually change its
     * size; this is done with a call to \ref setSize or a call to \ref performLayout()
     * in the parent widget.
     */
    void setFixedSize(const Vector2i &fixedSize) { mFixedSize = fixedSize; }

    /// Return the fixed size (see \ref setFixedSize())
    const Vector2i &fixedSize() const { return mFixedSize; }

    // Return the fixed width (see \ref setFixedSize())
    int fixedWidth() const { return mFixedSize.x; }
    // Return the fixed height (see \ref setFixedSize())
    int fixedHeight() const { return mFixedSize.y; }
    /// Set the fixed width (see \ref setFixedSize())
    void setFixedWidth(int width) { mFixedSize.x = width; }
    Widget& withFixedWidth(int width) { setFixedWidth(width); return *this; }

    /// Set the fixed height (see \ref setFixedSize())
    void setFixedHeight(int height) { mFixedSize.y = height; }

    /// Return whether or not the widget is currently visible (assuming all parents are visible)
    bool visible() const { return mVisible; }
    /// Set whether or not the widget is currently visible (assuming all parents are visible)
    void setVisible(bool visible) { mVisible = visible; }

    /// Check if this widget is currently visible, taking parent widgets into account
    bool visibleRecursive() const {
        bool visible = true;
        const Widget *widget = this;
        while (widget) {
            visible &= widget->visible();
            widget = widget->parent();
        }
        return visible;
    }

    /// Return the number of child widgets
    int childCount() const { return (int) mChildren.size(); }

    /// Return the list of child widgets of the current widget
    const std::vector<Widget *> &children() const { return mChildren; }

    /**
     * \brief Add a child widget to the current widget at
     * the specified index.
     *
     * This function almost never needs to be called by hand,
     * since the constructor of \ref Widget automatically
     * adds the current widget to its parent
     */
    virtual void addChild(int index, Widget *widget);

    /// Convenience function which appends a widget at the end
    void addChild(Widget *widget);

    /// Remove a child widget by index
    void removeChild(int index);

    /// Remove a child widget by value
    void removeChild(const Widget *widget);

    /// Retrieves the child at the specific position
    const Widget* childAt(int index) const { return mChildren[index]; }

    /// Retrieves the child at the specific position
    Widget* childAt(int index) { return mChildren[index]; }

    /// Returns the index of a specific child or -1 if not found
    int childIndex(Widget* widget) const;

    /// Variadic shorthand notation to construct and add a child widget
    // 模板函数
    template<typename WidgetClass, typename... Args>
    WidgetClass* add(const Args&... args) {
        return new WidgetClass(this, args...);
    }

    /* 定义了一个模板函数 wdg
     * 和 add 模板有什么区别呢？？？
     * */
    template<typename WidgetClass, typename... Args>
    WidgetClass& wdg(const Args&... args)
    {
      WidgetClass* widget = new WidgetClass( this, args... );
      return *widget;
    }

    /// Walk up the hierarchy and return the parent window
    Window *window();

    /// Associate this widget with an ID value (optional)
    void setId(const std::string &id) { mId = id; }
    /// Return the ID value associated with this widget, if any
    const std::string &id() const { return mId; }

    /// Return whether or not this widget is currently enabled
    bool enabled() const { return mEnabled; }
    /// Set whether or not this widget is currently enabled
    void setEnabled(bool enabled) { mEnabled = enabled; }

    /// Return whether or not this widget is currently focused
    bool focused() const { return mFocused; }
    /// Set whether or not this widget is currently focused
    void setFocused(bool focused) { mFocused = focused; }
    /// Request the focus to be moved to this widget
    void requestFocus();

    const std::string &tooltip() const { return mTooltip; }
    void setTooltip(const std::string &tooltip) { mTooltip = tooltip; }

    /// Return current font size. If not set the default of the current theme will be returned
    int fontSize() const;
    /// Set the font size of this widget
    virtual void setFontSize(int fontSize) { mFontSize = fontSize; }
    /// Return whether the font size is explicitly specified for this widget
    bool hasFontSize() const { return mFontSize > 0; }

    /// Return a pointer to the cursor of the widget
    Cursor cursor() const { return mCursor; }
    /// Set the cursor of the widget
    void setCursor(Cursor cursor) { mCursor = cursor; }

    /// Check if the widget contains a certain position
    bool contains(const Vector2i &p) const
    {
      Vector2i d = p - _pos;
      return d.positive() && d.lessOrEq({ mSize.x, mSize.y });
    }

    /// Determine the widget located at the given position value (recursive)
    Widget *findWidget(const Vector2i &p);
    Widget *find(const std::string& id, bool inchildren=true);


    Widget *gfind(const std::string& id)
    {
      Widget* parent = this;
      while (parent->parent()) parent = parent->parent();

      return parent->find(id, true);
    }

    template<typename RetClass>
    RetClass *gfind(const std::string& id)
    { 
      Widget* f = gfind(id);
      return f ? f->cast<RetClass>() : nullptr;
    }

    /// Handle a mouse button event (default implementation: propagate to children)
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);

    /// Handle a mouse motion event (default implementation: propagate to children)
    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);

    /// Handle a mouse drag event (default implementation: do nothing)
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);

    /// Handle a mouse enter/leave event (default implementation: record this fact, but do nothing)
    virtual bool mouseEnterEvent(const Vector2i &p, bool enter);

    /// Handle a mouse scroll event (default implementation: propagate to children)
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel);

    /// Handle a focus change event (default implementation: record the focus status, but do nothing)
    virtual bool focusEvent(bool focused);

    /// Handle a keyboard event (default implementation: do nothing)
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    /// Handle text input (UTF-32 format) (default implementation: do nothing)
    virtual bool keyboardCharacterEvent(unsigned int codepoint);

    /// Compute the preferred size of the widget
    virtual Vector2i preferredSize(SDL_Renderer *ctx) const;

    /// Invoke the associated layout generator to properly place child widgets, if any
    virtual void performLayout(SDL_Renderer *ctx);

    /// Draw the widget (and all child widgets)
    virtual void draw(SDL_Renderer* renderer);

    virtual int getAbsoluteLeft() const;
    virtual SDL_Point getAbsolutePos() const;
    virtual PntRect getAbsoluteCliprect() const;
    virtual int getAbsoluteTop() const;

    /* 返回 parent 函数,返回的是一个 Widget 对象指针 */
    Widget& _and() { return *parent(); }
    Widget& withId(const std::string& id) { setId(id); return *this; }
    
    Widget& withPosition( const Vector2i& pos ) { setPosition( pos); return *this; }
    Widget& withFontSize(int size) { setFontSize(size); return *this; }
    Widget& withFixedSize(const Vector2i& size) { setFixedSize(size); return *this; }
    Widget& withTooltip(const std::string& text) { setTooltip(text); return *this; }

    /* withLayout 模板 */
    template<typename LayoutClass,typename... Args>
    Widget& withLayout(const Args&... args) { setLayout(new LayoutClass(args...)); return *this; }

    template<typename RetClass> RetClass* cast() { return dynamic_cast<RetClass*>(this); }

    template<typename... Args>Widget& boxlayout(const Args&... args) { return withLayout<BoxLayout>(args...); }
    template<typename... Args>ToolButton& toolbutton(const Args&... args) { return wdg<ToolButton>(args...); }
    template<typename... Args>PopupButton& popupbutton(const Args&... args) { return wdg<PopupButton>(args...); }
    /* label 模板函数 */
    template<typename... Args>Label& label(const Args&... args) { return wdg<Label>(args...); }
    template<typename... Args>ProgressBar& progressbar(const Args&... args) { return wdg<ProgressBar>(args...); }
    template<typename... Args>DropdownBox& dropdownbox(const Args&... args) { return wdg<DropdownBox>(args...); }
    template<typename... Args>ComboBox& combobox(const Args&... args) { return wdg<ComboBox>(args...); }
    template<typename... Args>Button& button(const Args&... args) { return wdg<Button>(args...); }
    template<typename... Args>Widget& widget(const Args&... args) { return wdg<Widget>(args...); }
    template<typename... Args>CheckBox& checkbox(const Args&... args) { return wdg<CheckBox>(args...); }
    template<typename... Args>MessageDialog& msgdialog(const Args&... args) { return wdg<MessageDialog>(args...); }
    template<typename... Args>VScrollPanel& vscrollpanel(const Args&... args) { return wdg<VScrollPanel>(args...); }
    template<typename... Args>ImagePanel& imgpanel(const Args&... args) { return wdg<ImagePanel>(args...); }  
    template<typename... Args>Slider& slider(const Args&... args) { return wdg<Slider>(args...); }
    template<typename... Args>TextBox& textbox(const Args&... args) { return wdg<TextBox>(args...); }


protected:
    /// Free all resources used by the widget and any children
    virtual ~Widget();

protected:
    Widget *mParent;
    ref<Theme> mTheme;
    ref<Layout> mLayout;
    std::string mId;
    Vector2i _pos; /* 这个 _pos 表示的是自己的位置？？？ */
    Vector2i mSize, mFixedSize;
    std::vector<Widget *> mChildren;
    bool mVisible, mEnabled;
    bool mFocused, mMouseFocus;
    std::string mTooltip;
    int mFontSize;
    Cursor mCursor;
};

NAMESPACE_END(sdlgui)
