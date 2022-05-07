#include "boxlayouteroptimized.h"

BoxLayouterOptimized::BoxLayouterOptimized(QObject *parent)
    : BoxLayouter(parent)
{
}

bool BoxLayouterOptimized::SortCharsByHeight(const RenderedChar &a, const RenderedChar &b)
{
    if (a.image.height() + a.image.offset().y() > b.image.height() + b.image.offset().y())
        return false;
    else if (a.image.height() + a.image.offset().y() == b.image.height() + b.image.offset().y())
    {
        if (a.image.width() > b.image.width())
            return false;
        else if (a.image.width() == b.image.width())
        {
            if (a.image.offset().y() > b.image.offset().y())
                return false;
            else if (a.image.offset().y() == b.image.offset().y())
            {
                if (a.symbol > b.symbol)
                    return false;
            }
        }
    }
    return true;
}

void BoxLayouterOptimized::OptimizeLayout(std::vector<RenderedChar> &chars)
{
  std::sort(chars.begin(), chars.end(), SortCharsByHeight);
}


AbstractLayouter* BoxLayouterOptimizedFactoryFunc (QObject* parent) {
    return new BoxLayouterOptimized(parent);
}
