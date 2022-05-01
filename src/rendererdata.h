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

#ifndef RENDERERDATA_H
#define RENDERERDATA_H

#include <QMap>
#include <QVector>
#include <QImage>



struct RenderedChar {
    uint32_t symbol;
    int32_t offsetX;
    int32_t offsetY;
    int32_t advance;
    QImage img;
    std::map<uint32_t,int32_t> kerning;
    bool    locked;
    RenderedChar() : symbol(0),locked(false) {}
    RenderedChar(uint32_t symbol,int32_t x,int32_t y,int32_t a,const QImage& img) :
            symbol(symbol),offsetX(x),offsetY(y),advance(a),img(img) ,locked(false){}
};

struct RenderedMetrics {
    int32_t ascender;
    int32_t descender;
    int32_t height;
};

struct RendererData {
    QMap<uint32_t,RenderedChar> chars;
    RenderedMetrics metrics;
};



#endif // RENDERERDATA_H
