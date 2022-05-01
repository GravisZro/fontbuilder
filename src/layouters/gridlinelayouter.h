#ifndef GRIDLINELAYOUTER_H
#define GRIDLINELAYOUTER_H

#include "gridlayouter.h"

class GridLineLayouter : public GridLayouter
{
public:
    GridLineLayouter(QObject *parent);

    // GridLayouter interface
protected:
    void calculateSize(int32_t maxW, int32_t maxH, size_t count);
};

#endif // GRIDLINELAYOUTER_H
