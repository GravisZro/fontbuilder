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

#include "layoutdata.h"

#include <QPainter>
#include <QDebug>

LayoutData::LayoutData(QObject *parent) :
    QObject(parent)
{
}

LayoutData::~LayoutData() {
}

void LayoutData::resize(int32_t w,int32_t h) {
    m_width = w;
    m_height = h;
}

void LayoutData::clearLayout() {
    m_placed.clear();
}

void LayoutData::placeChar(const RenderedChar& c) {
  m_placed.push_back(c);
}


void LayoutData::render(QPoint offset)
{
  m_image = QImage (m_width, m_height, QImage::Format_ARGB32);
  m_image.fill(0);

  QPainter painter(&m_image);
  for (const auto& character : m_placed)
  {
    QPoint pos = character.image.offset() + offset;
    painter.drawImage(pos.x(), pos.y(), character.image);
  }
  emit layoutChanged();
}

