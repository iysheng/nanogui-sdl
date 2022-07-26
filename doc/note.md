#### nanogui

example1
    TestWindow *screen = new TestWindow()
        screen->drawAll() // 绘图
            performLayout(mSDL_Renderer); // 确定了每一个部件的位置和大小???

----------------
Screen
    Widget
        TestWindow   ->  window (child)  -> label(child)
                                         -> textbox(child)
                                         -> keyboard(child)
