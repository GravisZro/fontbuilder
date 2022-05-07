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

#include "boxlayouter.h"
#include <layoutdata.h>

#include <cmath>

BoxLayouter::BoxLayouter(QObject *parent) :
    AbstractLayouter(parent)
{
}

struct Line {
    int32_t min_y;
    int32_t max_y;
    int32_t y;
    Line() : min_y(0),max_y(0),y(0) {}
    explicit Line(const RenderedChar& character) : y(0)
    {
        min_y = character.image.offset().y();
        max_y = character.image.offset().y() + character.image.height();
        chars.push_back(&character);
    }
    int32_t h() const { return max_y - min_y;}
    void append(const RenderedChar& character)
    {
        if (character.image.offset().y() < min_y)
            min_y = character.image.offset().y();
        if (character.image.offset().y() + character.image.height() > max_y)
            max_y = character.image.offset().y() + character.image.height();
        chars.push_back(&character);
    }

    std::vector<const RenderedChar*> chars;
};

void BoxLayouter::PlaceImages(const std::vector<RenderedChar>& chars)
{
    int32_t h = 0;
    int32_t w = 0;
    if (chars.empty())
      return;

    /// speed up
    int32_t area = 0;

    for (const auto& character : chars)
        area += character.image.width() * character.image.height();

    int32_t dim = ::sqrt(area);

    resize(dim,dim);
    w = width();
    h = height();


    std::vector<Line> lines;

    bool iteration = true;
    while (iteration) {
        int32_t x = 0;
        lines.clear();
        lines.push_back(Line(chars.front()));
        iteration = false;

        for(const auto& character : chars)
        {
            if ((x + character.image.width()) > w) {

                x = 0;
                int32_t y = lines.back().y;
                int32_t h = lines.back().h();
                lines.push_back(Line(character));
                lines.back().y = y + h;
            }

           if ( (lines.back().y + character.image.height())>h ) {
                if (w>h) {
                    resize(width(),lines.back().y + character.image.height());
                    h = height();
                }
                else {
                    resize(width() + character.image.width(), height());
                    w = width();
                }
                iteration = true;
                break;
            }

            /// place
           lines.back().append(character);
           x += character.image.width();
        }
    }


    int32_t x = 0;
    for (const Line& line : lines)
    {
        x = 0;
        for (const auto* character : line.chars )
        {
            auto l = *character;
            l.image.setOffset({ x, line.y + l.image.offset().y() - line.min_y });
            place(l);
            x += character->image.width();
        }

    }
}


AbstractLayouter* BoxLayouterFactoryFunc (QObject* parent) {
    return new BoxLayouter(parent);
}
