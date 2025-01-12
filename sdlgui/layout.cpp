/*
    sdlgui/layout.cpp -- A collection of useful layout managers

    The grid layout was contributed by Christian Schueller.

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <sdlgui/layout.h>
#include <sdlgui/widget.h>
#include <sdlgui/window.h>
#include <sdlgui/theme.h>
#include <sdlgui/label.h>
#include <numeric>

NAMESPACE_BEGIN(sdlgui)

BoxLayout::BoxLayout(Orientation orientation, Alignment alignment,
          int margin, int spacing)
    : mOrientation(orientation), mAlignment(alignment), mMargin(margin),
      mSpacing(spacing) 
{
}

Vector2i BoxLayout::preferredSize(SDL_Renderer *ctx, const Widget *widget) const
{
  Vector2i size(2*mMargin, 2*mMargin);

    int yOffset = 0;
    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty()) 
    {
        if (mOrientation == Orientation::Vertical)
            size[1] += widget->theme()->mWindowHeaderHeight - mMargin/2;
        else
            yOffset = widget->theme()->mWindowHeaderHeight;
    }

    bool first = true;
    int axis1 = (int) mOrientation, axis2 = ((int) mOrientation + 1)%2;
    for (auto w : widget->children()) 
    {
        if (!w->visible())
            continue;
        if (first)
            first = false;
        else
            size[axis1] += mSpacing;

        Vector2i ps = w->preferredSize(ctx), fs = w->fixedSize();
        Vector2i targetSize(
            fs[0] ? fs[0] : ps[0],
            fs[1] ? fs[1] : ps[1]
        );

        size[axis1] += targetSize[axis1];
        size[axis2] = std::max(size[axis2], targetSize[axis2] + 2*mMargin);
        first = false;
    }
    return size + Vector2i(0, yOffset);
}

void BoxLayout::performLayout(SDL_Renderer *ctx, Widget *widget) const 
{
  Vector2i fs_w = widget->fixedSize();
  Vector2i containerSize(
        fs_w[0] ? fs_w[0] : widget->width(),
        fs_w[1] ? fs_w[1] : widget->height()
    );

    int axis1 = (int) mOrientation, axis2 = ((int) mOrientation + 1)%2;
    int _position = mMargin;
    int yOffset = 0;

    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty()) 
    {
        if (mOrientation == Orientation::Vertical) 
        {
            _position += widget->theme()->mWindowHeaderHeight - mMargin/2;
        } 
        else 
        {
            yOffset = widget->theme()->mWindowHeaderHeight;
            containerSize[1] -= yOffset;
        }
    }

    bool first = true;
    for (auto w : widget->children()) 
    {
        if (!w->visible())
            continue;
        if (first)
            first = false;
        else
            _position += mSpacing;

        Vector2i ps = w->preferredSize(ctx), fs = w->fixedSize();
        Vector2i targetSize(
            fs.x ? fs.x : ps.x,
            fs.y ? fs.y : ps.y
        );
        Vector2i pos(0, yOffset);

        pos[axis1] = _position;

        switch (mAlignment) 
        {
            case Alignment::Minimum:
                pos[axis2] += mMargin;
                break;
            case Alignment::Middle:
                pos[axis2] += (containerSize[axis2] - targetSize[axis2]) / 2;
                break;
            case Alignment::Maximum:
                pos[axis2] += containerSize[axis2] - targetSize[axis2] - mMargin * 2;
                break;
            case Alignment::Fill:
                pos[axis2] += mMargin;
                targetSize[axis2] = fs[axis2] ? fs[axis2] : (containerSize[axis2] - mMargin * 2);
                break;
        }

        w->setPosition(pos);
        w->setSize(targetSize);
        w->performLayout(ctx);
        _position += targetSize[axis1];
    }
}

Vector2i GroupLayout::preferredSize(SDL_Renderer *ctx, const Widget *widget) const 
{
    int hh = mMargin, ww = 2*mMargin;

    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        hh += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true, indent = false;
    for (auto c : widget->children()) 
    {
        if (!c->visible())
            continue;
        const Label *label = dynamic_cast<const Label *>(c);
        if (!first)
            hh += (label == nullptr) ? mSpacing : mGroupSpacing;
        first = false;

        Vector2i ps = c->preferredSize(ctx), fs = c->fixedSize();
        Vector2i targetSize(
            fs.x ? fs.x : ps.x,
            fs.y ? fs.y : ps.y
        );

        bool indentCur = indent && label == nullptr;
        hh += targetSize.y;
        ww = std::max(ww, targetSize.x + 2*mMargin + (indentCur ? mGroupIndent : 0));

        if (label)
            indent = !label->caption().empty();
    }
    hh += mMargin;
    return Vector2i(ww, hh);
}

void GroupLayout::performLayout(SDL_Renderer *ctx, Widget *widget) const 
{
    int hh = mMargin, availableWidth =
        (widget->fixedWidth() ? widget->fixedWidth() : widget->width()) - 2*mMargin;

    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        hh += widget->theme()->mWindowHeaderHeight - mMargin/2;

    bool first = true, indent = false;
    for (auto c : widget->children()) 
    {
        if (!c->visible())
            continue;
        const Label *label = dynamic_cast<const Label *>(c);
        if (!first)
            hh += (label == nullptr) ? mSpacing : mGroupSpacing;
        first = false;

        bool indentCur = indent && label == nullptr;
        Vector2i ps = Vector2i{ availableWidth - (indentCur ? mGroupIndent : 0),
                               c->preferredSize(ctx).y };
        Vector2i fs = c->fixedSize();

        Vector2i targetSize(
            fs.x ? fs.x : ps.x,
            fs.y ? fs.y : ps.y
        );

        c->setPosition(Vector2i{ mMargin + (indentCur ? mGroupIndent : 0), hh });
        c->setSize(targetSize);
        c->performLayout(ctx);

        hh += targetSize.y;

        if (label)
            indent = !label->caption().empty();
    }
}

Vector2i GridLayout::preferredSize(SDL_Renderer *ctx,
                                   const Widget *widget) const 
{
    /* Compute minimum row / column sizes */
    std::vector<int> grid[2];
    computeLayout(ctx, widget, grid);

    Vector2i size(
        2*mMargin + std::accumulate(grid[0].begin(), grid[0].end(), 0)
         + std::max((int) grid[0].size() - 1, 0) * mSpacing[0],
        2*mMargin + std::accumulate(grid[1].begin(), grid[1].end(), 0)
         + std::max((int) grid[1].size() - 1, 0) * mSpacing[1]
    );

    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        size[1] += widget->theme()->mWindowHeaderHeight - mMargin/2;

    return size;
}

void GridLayout::computeLayout(SDL_Renderer *ctx, const Widget *widget, std::vector<int> *grid) const 
{
    int axis1 = (int) mOrientation, axis2 = (axis1 + 1) % 2;
    size_t numChildren = widget->children().size(), visibleChildren = 0;
    for (auto w : widget->children())
        visibleChildren += w->visible() ? 1 : 0;

    Vector2i dim;
    dim[axis1] = mResolution;
    dim[axis2] = (int) ((visibleChildren + mResolution - 1) / mResolution);

    grid[axis1].clear(); grid[axis1].resize(dim[axis1], 0);
    grid[axis2].clear(); grid[axis2].resize(dim[axis2], 0);

    size_t child = 0;
    for (int i2 = 0; i2 < dim[axis2]; i2++) 
    {
        for (int i1 = 0; i1 < dim[axis1]; i1++) 
        {
            Widget *w = nullptr;
            do 
            {
                if (child >= numChildren)
                    return;
                w = widget->children()[child++];
            } while (!w->visible());

            Vector2i ps = w->preferredSize(ctx);
            Vector2i fs = w->fixedSize();
            Vector2i targetSize(
                fs.x ? fs.x : ps.x,
                fs.y ? fs.y : ps.y
            );

            grid[axis1][i1] = std::max(grid[axis1][i1], targetSize[axis1]);
            grid[axis2][i2] = std::max(grid[axis2][i2], targetSize[axis2]);
        }
    }
}

/* layout 的 performLayout 成员函数 */
void GridLayout::performLayout(SDL_Renderer *ctx, Widget *widget) const 
{
  Vector2i fs_w = widget->fixedSize();
  /* 如果设置了 fixed size,那么使用 fixedSize */
  Vector2i containerSize(
        fs_w[0] ? fs_w[0] : widget->width(),
        fs_w[1] ? fs_w[1] : widget->height()
    );

    /* Compute minimum row / column sizes */
    std::vector<int> grid[2];
    computeLayout(ctx, widget, grid);
    int dim[2] = { (int) grid[0].size(), (int) grid[1].size() };

    Vector2i extra = Vector2i::Zero();
    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        extra[1] += widget->theme()->mWindowHeaderHeight - mMargin / 2;

    /* Strech to size provided by \c widget */
    for (int i = 0; i < 2; i++) 
    {
        int gridSize = 2 * mMargin + extra[i];
        for (int s : grid[i]) 
        {
            gridSize += s;
            if (i+1 < dim[i])
                gridSize += mSpacing[i];
        }

        if (gridSize < containerSize[i]) 
        {
            /* Re-distribute remaining space evenly */
            int gap = containerSize[i] - gridSize;
            int g = gap / dim[i];
            int rest = gap - g * dim[i];
            for (int j = 0; j < dim[i]; ++j)
                grid[i][j] += g;
            for (int j = 0; rest > 0 && j < dim[i]; --rest, ++j)
                grid[i][j] += 1;
        }
    }

    int axis1 = (int) mOrientation, axis2 = (axis1 + 1) % 2;
    /* 确定一个起始向量 */
    Vector2i start = Vector2i::Constant(mMargin) + extra;

    /* 获取 child 节点的数量大小 */
    size_t numChildren = widget->children().size();
    size_t child = 0;

    /* 定义一个起始的位置向量 !!! */
    Vector2i pos = start;
    for (int i2 = 0; i2 < dim[axis2]; i2++) 
    {
        pos[axis1] = start[axis1];
        for (int i1 = 0; i1 < dim[axis1]; i1++) 
        {
            Widget *w = nullptr;
            /* 遍历这个 window 的 child window */
            do 
            {
                if (child >= numChildren)
                    return;
                w = widget->children()[child++];
            } while (!w->visible());

            /* 执行 preferredSize() 函数,确定一个 preferredSize */
            Vector2i ps = w->preferredSize(ctx);
            /* 获取 fixed size */
            Vector2i fs = w->fixedSize();
            Vector2i targetSize(
                fs[0] ? fs[0] : ps[0],
                fs[1] ? fs[1] : ps[1]
            );

            Vector2i itemPos(pos);
            for (int j = 0; j < 2; j++) 
            {
                int axis = (axis1 + j) % 2;
                int item = j == 0 ? i1 : i2;
                Alignment align = alignment(axis, item);

                switch (align) 
                {
                    case Alignment::Minimum:
                        break;
                    case Alignment::Middle:
                        itemPos[axis] += (grid[axis][item] - targetSize[axis]) / 2;
                        break;
                    case Alignment::Maximum:
                        itemPos[axis] += grid[axis][item] - targetSize[axis];
                        break;
                    case Alignment::Fill:
                        targetSize[axis] = fs[axis] ? fs[axis] : grid[axis][item];
                        break;
                }
            }
            /* 确定了窗口的位置 */
            w->setPosition(itemPos);
            /* 确定了窗口的大小 */
            w->setSize(targetSize);
            /* 确定了窗口的布局 */
            w->performLayout(ctx);
            pos[axis1] += grid[axis1][i1] + mSpacing[axis1];
        }
        pos[axis2] += grid[axis2][i2] + mSpacing[axis2];
    }
}

AdvancedGridLayout::AdvancedGridLayout(const std::vector<int> &cols, const std::vector<int> &rows, int margin)
 : mCols(cols), mRows(rows), mMargin(margin) 
{
  /* 修改 mColStretch vector 的大小，数量不足时候以 0 填充 */
    mColStretch.resize(mCols.size(), 0);
  /* 修改 mRowStretch vector 的大小，数量不足时候以 0 填充 */
    mRowStretch.resize(mRows.size(), 0);
}

/* 获取该 layout 下自动计算控件大小函数 */
Vector2i AdvancedGridLayout::preferredSize(SDL_Renderer *ctx, const Widget *widget) const
{
    /* Compute minimum row / column sizes */
    std::vector<int> grid[2];
    computeLayout(ctx, widget, grid);

    Vector2i size(
        std::accumulate(grid[0].begin(), grid[0].end(), 0),
        std::accumulate(grid[1].begin(), grid[1].end(), 0));

    Vector2i extra = Vector2i::Constant(2 * mMargin);
    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        extra[1] += widget->theme()->mWindowHeaderHeight - mMargin/2;

    return size+extra;
}

void AdvancedGridLayout::performLayout(SDL_Renderer *ctx, Widget *widget) const 
{
  /* 定义在栈上的向量 */
    std::vector<int> grid[2];
    /* 会将计算的结果保存到 grid ？？？ */
    computeLayout(ctx, widget, grid);

    grid[0].insert(grid[0].begin(), mMargin);
    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        grid[1].insert(grid[1].begin(), widget->theme()->mWindowHeaderHeight + mMargin/2);
    else
        grid[1].insert(grid[1].begin(), mMargin);

    for (int axis=0; axis<2; ++axis) 
    {
        for (size_t i=1; i<grid[axis].size(); ++i)
            grid[axis][i] += grid[axis][i-1];

        for (Widget *w : widget->children()) 
        {
            if (!w->visible())
                continue;
            /* 获取指定 child 的 anchor 信息 */
            Anchor anchor = this->anchor(w);

            int itemPos = grid[axis][anchor.pos[axis]];
            int cellSize  = grid[axis][anchor.pos[axis] + anchor.size[axis]] - itemPos;
            int ps = w->preferredSize(ctx)[axis], fs = w->fixedSize()[axis];
            int targetSize = fs ? fs : ps;

            switch (anchor.align[axis]) 
            {
                case Alignment::Minimum:
                    break;
                case Alignment::Middle:
                    itemPos += (cellSize - targetSize) / 2;
                    break;
                case Alignment::Maximum:
                    itemPos += cellSize - targetSize;
                    break;
                case Alignment::Fill:
                    targetSize = fs ? fs : cellSize;
                    break;
            }

            Vector2i pos = w->position(), size = w->size();
            pos[axis] = itemPos;
            size[axis] = targetSize;
            w->setPosition(pos);
            w->setSize(size);
            w->performLayout(ctx);
        }
    }
}

/* 计算 */
void AdvancedGridLayout::computeLayout(SDL_Renderer *ctx, const Widget *widget,
                                       std::vector<int> *_grid) const 
{
  Vector2i fs_w = widget->fixedSize();
  Vector2i containerSize(
        fs_w[0] ? fs_w[0] : widget->width(),
        fs_w[1] ? fs_w[1] : widget->height()
    );

    Vector2i extra(2 * mMargin, 2 * mMargin);
    const Window *window = dynamic_cast<const Window *>(widget);
    if (window && !window->title().empty())
        extra[1] += widget->theme()->mWindowHeaderHeight - mMargin/2;

    containerSize -= extra;

    for (int axis=0; axis<2; ++axis) 
    {
        std::vector<int> &grid = _grid[axis];
        const std::vector<int> &sizes = axis == 0 ? mCols : mRows;
        const std::vector<float> &stretch = axis == 0 ? mColStretch : mRowStretch;
        grid = sizes;

        for (int phase = 0; phase < 2; ++phase) 
        {
          /* mAnchor 是一个无序的 map */
            for (auto pair : mAnchor) {
                const Widget *w = pair.first;
                if (!w->visible())
                    continue;
                const Anchor &anchor = pair.second;
                if ((anchor.size[axis] == 1) != (phase == 0))
                    continue;
                /* 计算这个窗口的宽和高 */
                int ps = w->preferredSize(ctx)[axis], fs = w->fixedSize()[axis];
                int targetSize = fs ? fs : ps;

                if (anchor.pos[axis] + anchor.size[axis] > (int) grid.size())
                    throw std::runtime_error(
                        "Advanced grid layout: widget is out of bounds: " +
                        (std::string) anchor);

                int currentSize = 0;
                float totalStretch = 0;
                for (int i = anchor.pos[axis];
                     i < anchor.pos[axis] + anchor.size[axis]; ++i) {
                    if (sizes[i] == 0 && anchor.size[axis] == 1)
                        grid[i] = std::max(grid[i], targetSize);
                    currentSize += grid[i];
                    totalStretch += stretch[i];
                }
                if (targetSize <= currentSize)
                    continue;
                if (totalStretch == 0)
                    throw std::runtime_error(
                        "Advanced grid layout: no space to place widget: " +
                        (std::string) anchor);
                float amt = (targetSize - currentSize) / totalStretch;
                for (int i = anchor.pos[axis];
                     i < anchor.pos[axis] + anchor.size[axis]; ++i) {
                    grid[i] += (int) std::round(amt * stretch[i]);
                }
            }
        }

        int currentSize = std::accumulate(grid.begin(), grid.end(), 0);
        float totalStretch = std::accumulate(stretch.begin(), stretch.end(), 0.0f);
        if (currentSize >= containerSize[axis] || totalStretch == 0)
            continue;
        float amt = (containerSize[axis] - currentSize) / totalStretch;
        for (size_t i = 0; i<grid.size(); ++i)
            grid[i] += (int) std::round(amt * stretch[i]);
    }
}

NAMESPACE_END(sdlgui)
