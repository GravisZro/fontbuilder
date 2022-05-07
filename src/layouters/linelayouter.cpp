/**
 * Copyright (c) 2010-2010 Andrey AndryBlack Kunitsyn
 * email:support.andryblack@gmail.com
 *
 * Report bugs and download new versions at http://code.google.com/p/fontbuilder
 *
 * This software is distributed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "linelayouter.h"
#include <rendererdata.h>
#include <layoutdata.h>
#include <vector>

LineLayouter::LineLayouter(QObject *parent) :
    AbstractLayouter(parent)
{
}


void LineLayouter::PlaceImages(const std::vector<RenderedChar>& chars) {
    int32_t w = 0;
    if (chars.empty())
      return;
    int32_t min_y = chars.front().image.offset().y();
    int32_t max_y = chars.front().image.offset().y() + chars.front().image.height();
    for(const auto& character : chars)
    {
        w += character.image.width();
        if (character.image.offset().y() < min_y)
            min_y = character.image.offset().y();
        if (character.image.offset().y() + character.image.height() > max_y)
            max_y = character.image.offset().y() + character.image.height();
    }
    resize(w,max_y-min_y);
    int32_t x = 0;
    //int32_t y = 0;

    for (const auto& character : chars)
    {
        auto l = character;
        l.image.setOffset( { x, l.image.offset().y() - min_y } );
        place(l);
        x += character.image.width();
    }
}


AbstractLayouter* LineLayouterFactoryFunc (QObject* parent) {
    return new LineLayouter(parent);
}
