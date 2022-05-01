#include "gridlinelayouter.h"

GridLineLayouter::GridLineLayouter(QObject *parent)
    : GridLayouter(parent)
{
}

void GridLineLayouter::calculateSize(int32_t maxW, int32_t maxH, size_t count)
{
    resize(maxW * count, maxH);
}

AbstractLayouter* GridLineLayouterFactoryFunc (QObject* parent) {
    return new GridLineLayouter(parent);
}
