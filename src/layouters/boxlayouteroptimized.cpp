#include "boxlayouteroptimized.h"

BoxLayouterOptimized::BoxLayouterOptimized(QObject *parent)
    : BoxLayouter(parent)
{
}

bool BoxLayouterOptimized::SortCharsByHeight(const LayoutChar &a, const LayoutChar &b)
{
    if (a.bounding.height() + a.bounding.y() > b.bounding.height() + b.bounding.y())
        return false;
    else if (a.bounding.height() + a.bounding.y() == b.bounding.height() + b.bounding.y())
    {
        if (a.bounding.width() > b.bounding.width())
            return false;
        else if (a.bounding.width() == b.bounding.width())
        {
            if (a.bounding.y() > b.bounding.y())
                return false;
            else if (a.bounding.y() == b.bounding.y())
            {
                if (a.symbol > b.symbol)
                    return false;
            }
        }
    }
    return true;
}

void BoxLayouterOptimized::OptimizeLayout(QVector<LayoutChar> &chars)
{
  std::sort(chars.begin(), chars.end(), SortCharsByHeight);
}


AbstractLayouter* BoxLayouterOptimizedFactoryFunc (QObject* parent) {
    return new BoxLayouterOptimized(parent);
}
