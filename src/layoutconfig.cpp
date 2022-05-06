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

#include "layoutconfig.h"

LayoutConfig::LayoutConfig(QObject *parent) :
    QObject(parent),
    m_one_pixel_offset(true),
    m_pot_image(true),
    m_size_increment(1),
    m_offset(0, 0, 0, 0)
{
}


void LayoutConfig::setOnePixelOffset(bool b)
{
  if (m_one_pixel_offset != b)
  {
    m_one_pixel_offset = b;
    emit layoutConfigChanged();
  }
}

void LayoutConfig::setPotImage(bool b)
{
  if (m_pot_image != b)
  {
    m_pot_image = b;
    emit layoutConfigChanged();
  }
}

void LayoutConfig::setSizeIncrement(int32_t v) {
  if (m_size_increment != v)
  {
    m_size_increment = v;
    emit layoutConfigChanged();
  }
}

void LayoutConfig::setOffset(QRect o)
{
  if(m_offset != o)
  {
    m_offset = o;
    emit layoutConfigChanged();
  }
}
