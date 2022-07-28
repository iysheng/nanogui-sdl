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

---------------- PerformLayout
performLayout
    Widget::performLayout
        mLayout->performLayout 如果定义了 layout 使用 layout 对象的 performLayout 函数
        GridLayout 比如定义了 GridLayout 对象
            GridLayout::performLayout 函数中会遍历 child 节点的 widget 指定对应的操作，包括
                preferredSize 获取建议的大小，这个数值是自动计算出来的
                    如果有 mLayout 会用 mLayout->preferredSize 函数结算 preferred 大小,否则使用的是 mSize 成员
                    GridLayout::computeLayout
                fixedSize  如果定义了 fixedSize 那么会优先使用 fixedSize
                setPositon
                setSize
                performLayout
