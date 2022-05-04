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

#include "targawriter.h"

#include "builtinimagewriter.h"
#include "layoutdata.h"
#include <QImage>
#include <QPainter>
#include "../layoutconfig.h"

#include <QDebug>

TargaImageWriter::TargaImageWriter(QString ext,QObject *parent) :
    AbstractImageWriter(parent)
{
    setExtension(ext);
    setReloadSupport(true);
}


struct [[gnu::packed]] TGA_HEADER
{
        uint8_t idlength; //0: Size of Image-ID-Area
        uint8_t colourmaptype; //1: 0=no palette ; 1=palette present
        uint8_t datatypecode; //2: 0 - No IMage Date Included
        //   1 - Uncompressed Paletted Image
        //   2 - Uncompressed True Color
        //   3 - Uncompressed Black-White
        // 8+1 - RLE          Paletted Image
        // 8+2 - RLE          True Color
        // 8+3 - RLE          Black-White
        uint16_t colourmaporigin; //3:
        uint16_t colourmaplength; //5: Number of palette entries
        uint8_t colourmapdepth; //7: Bits per entry. Typicaly: 15,16,24,32
        uint16_t x_origin; //8:
        uint16_t y_origin; //10: Position coordinates on a display device
        uint16_t width; //12:
        uint16_t height; //14: Size of image in pixels
        uint8_t bitsperpixel; //16: Bits per pixel. Typicaly: 8,16,24,32
        uint8_t imagedescriptor; //17: 76-54-3210
        // NA-Orign-Alpha channel bits
};


bool TargaImageWriter::Export(QFile& file) {
    QImage pixmap = buildImage();

    TGA_HEADER header;
    header.idlength = 0;
    header.colourmaptype = 0;
    header.datatypecode = 2;
    header.colourmaporigin = 0;
    header.colourmaplength = 0;
    header.colourmapdepth = 0;
    header.x_origin = 0;
    header.y_origin = 0;
    header.width = pixmap.width();
    header.height = pixmap.height();
    header.bitsperpixel = 32;
    header.imagedescriptor = (1 << 5) | (8);

    file.write((const char*)&header,18);
    for (int32_t y=0;y<pixmap.height();y++) {
        /// @todo need endian control
        file.write((const char*)pixmap.scanLine(y),pixmap.width()*4);
    }



    return true;
}

template <int32_t bpp>
inline uint8_t* copy_element (const uint8_t* src,uint8_t* dst);

template <>
[[gnu::always_inline]] inline uint8_t* copy_element<3> (const uint8_t* src,uint8_t* dst) {
    *dst++=*src++;
    *dst++=*src++;
    *dst++=*src++;
    return dst;
}

template <>
[[gnu::always_inline]] inline uint8_t* copy_element<4> (const uint8_t* src,uint8_t* dst) {
    *dst++=*src++;
    *dst++=*src++;
    *dst++=*src++;
    *dst++=*src++;
    return dst;
}

template <int32_t bpp>
        void encode_rle(uint8_t* data,QFile& file,int32_t size) {
    uint8_t c;
    while (size>0) {
        if (file.read(reinterpret_cast<char*>(&c),1)!=1) return;
        if (c < 128) {
            c++;
            file.read(reinterpret_cast<char*>(data), bpp * c);
            data+=bpp * c;
            size-= c;
        } else {
            c-=127;
            file.read(reinterpret_cast<char*>(data), bpp);
            uint8_t* out = data + bpp;
            for(uint32_t counter = 1; counter < c; counter++)
            {
                out = copy_element<bpp> (data,out);
            }
            data = out;
            size-= c;
        }
    }
}

QImage* TargaImageWriter::reload(QFile& file) {
    TGA_HEADER header;
    if (file.read(reinterpret_cast<char*>(&header),18)!=18)
        return 0;
    /// don`t support palette
    if (header.colourmaptype)
        return 0;
    bool rle = header.datatypecode & 8;
    /// support only True Color data
    if ( (header.datatypecode&7) != 2)
        return 0;
    int32_t bpp = header.bitsperpixel;
    /// support only 24 and 32 bpp
    if (bpp!=24 && bpp!=32)
        return 0;
    int32_t width = header.width;
    int32_t height = header.height;
    /// @todo endian !
    QImage* img = 0;
    img = new QImage(width,height,QImage::Format_ARGB32);
    img->fill(0);
    uint8_t* data = reinterpret_cast<uint8_t*>(img->bits());
    if (bpp==32) {
        if (!rle) {
            qDebug() << "Load TGA 32bpp";
            file.read(reinterpret_cast<char*>(data),width*height*4);
        } else {
            qDebug() << "Load TGA 32bpp, rle";
            encode_rle<4>(data,file,width*height);
        }
    } else if (bpp==24) {
        uint8_t* src = new uint8_t [ width * height * 3];
        if (!rle) {
            qDebug() << "Load TGA 24bpp";
            file.read(reinterpret_cast<char*>(src),width*height*3);
        } else {
            qDebug() << "Load TGA 24bpp, rle";
            encode_rle<3>(src,file,width*height);
        }
        const uint8_t* s = src;
        uint8_t* d = data;
        for (int32_t i=0;i<width*height;i++) {
            *d++ = *s++;
            *d++ = *s++;
            *d++ = *s++;
            *d++ = 255;
        }
        delete [] src;
    }
    /// swap top-bottom
    qDebug() << "header.imagedescriptor : " << int32_t(header.imagedescriptor);
    const int32_t line_len = width*4;
    if ((header.imagedescriptor & (1<<5))==0) {
        uint8_t line[width * 4];
        for (int32_t i=0;i<height/2;i++) {
            ::memcpy(line,data+line_len*i,line_len);
            ::memcpy(data+line_len*i,data+line_len*(height-1-i),line_len);
            ::memcpy(data+line_len*(height-1-i),line,line_len);
        }
    }
    return img;
}
