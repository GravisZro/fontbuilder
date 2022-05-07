#include "gridlayouter.h"
#include <cmath>

GridLayouter::GridLayouter(QObject *parent)
    : AbstractLayouter(parent)
{
}

void GridLayouter::PlaceImages(const std::vector<RenderedChar> &chars)
{
    int32_t minY = INT_MAX;
    int32_t maxW = 0;
    int32_t maxH = 0;

    for(const auto& character : chars)
    {
        minY = std::min(minY, character.image.offset().y());
        maxW = std::max(maxW, character.image.offset().x() + character.image.width());
        maxH = std::max(maxH, character.image.height());
    }
    maxH -= minY;

    calculateSize(maxW, maxH, chars.size());

    int32_t charsPerRow = chars.size()
            ? width() / maxW
            : 0;
    int32_t rows = charsPerRow
            ? (chars.size() + charsPerRow - 1) / charsPerRow
            : 0;

    resize(maxW * charsPerRow, maxH * rows);

    int32_t row = 0;
    int32_t col = 0;
    for (const auto& character : chars)
    {
      auto l = character;
      l.image.setOffset( { l.image.offset().x() + (col * maxW),
                           l.image.offset().y() + (row * maxH - minY) } );
      place(l);

        if (++col >= charsPerRow)
        {
            col = 0;
            ++row;
        }
    }
}

void GridLayouter::calculateSize(int32_t maxW, int32_t maxH, size_t count)
{
    int32_t area = maxW * maxH * count;

    int32_t dim = ::sqrt(area);
    resize(dim,dim);
}

AbstractLayouter* GridLayouterFactoryFunc (QObject* parent) {
    return new GridLayouter(parent);
}
