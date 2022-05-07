#ifndef BOXLAYOUTEROPTIMIZED_H
#define BOXLAYOUTEROPTIMIZED_H

#include "boxlayouter.h"

class BoxLayouterOptimized : public BoxLayouter
{
public:
    BoxLayouterOptimized(QObject *parent);

private:
    static bool SortCharsByHeight(const RenderedChar &a, const RenderedChar &b);
    void OptimizeLayout(std::vector<RenderedChar> &chars);
};

#endif // BOXLAYOUTEROPTIMIZED_H
