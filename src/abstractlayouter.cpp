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

#include "abstractlayouter.h"
#include "layoutdata.h"
#include "layoutconfig.h"

AbstractLayouter::AbstractLayouter(QObject *parent) :
    QObject(parent)
{
    m_config = nullptr;
    m_data = nullptr;
}


void AbstractLayouter::setConfig(const LayoutConfig* config) {
    m_config = config;
    connect(m_config,SIGNAL(layoutConfigChanged()),this,SLOT(on_LayoutDataChanged()));
}

void AbstractLayouter::setData(LayoutData* data) {
    m_data = data;
}

void AbstractLayouter::DoPlace(const QVector<LayoutChar>& chars) {
    m_data->beginPlacing();
    m_compact_w = 0;
    m_compact_h = 0;
    PlaceImages(chars);
    resize(m_compact_w,m_compact_h);
    m_data->endPlacing();
}

void AbstractLayouter::OptimizeLayout(QVector<LayoutChar> &)
{
}

void AbstractLayouter::on_ReplaceImages(const QVector<LayoutChar>& chars) {
    m_chars = chars;

    if (m_data!=0 && m_config!=0 ) {
        on_LayoutDataChanged();
    }
}


void AbstractLayouter::on_LayoutDataChanged()
{
  Q_ASSERT(m_data != nullptr);
  Q_ASSERT(m_config != nullptr);
  QVector<LayoutChar> chars = m_chars;
  for(LayoutChar& c : chars)
  {
    QSize sz = c.bounding.size();
    if (m_config->onePixelOffset())
    {
      sz.rwidth()++;
      sz.rheight()++;
    }
    sz.rwidth () += m_config->offset().left() + m_config->offset().right();
    sz.rheight() += m_config->offset().top()  + m_config->offset().bottom();
    c.bounding.setSize(sz);
  }

  OptimizeLayout(chars);
  DoPlace(chars);
}

static uint32_t nextpot(uint32_t val) {
    val--;
    val = (val >> 1) | val;
    val = (val >> 2) | val;
    val = (val >> 4) | val;
    val = (val >> 8) | val;
    val = (val >> 16) | val;
    val++;
    return val;
}

void AbstractLayouter::resize(int32_t w,int32_t h) {
    if (m_config) {
        if (m_config->onePixelOffset())
        {
            w+=2;
            h+=2;
        }

        if (m_config->potImage()) {
            w = nextpot(w);
            h = nextpot(h);
        }

        int32_t sizeIncrement = m_config->sizeIncrement();
        if (sizeIncrement > 1)
        {
            w = ((w + sizeIncrement - 1) / sizeIncrement) * sizeIncrement;
            h = ((h + sizeIncrement - 1) / sizeIncrement) * sizeIncrement;
        }
    }
    if (m_data) {
        m_data->resize(w,h);
    }
}

int32_t AbstractLayouter::width() const {
    int32_t w = 0;
    if (m_data) {
        w = m_data->width();
    }
    if (m_config) {
        if (m_config->onePixelOffset())
            w-=2;
    }
    return w;
}
int32_t AbstractLayouter::height() const {
    int32_t h= 0;
    if (m_data) {
        h = m_data->height();
    }
    if (m_config) {
        if (m_config->onePixelOffset())
            h-=2;
    }
    return h;
}
void AbstractLayouter::place(const LayoutChar& c) {
    LayoutChar out = c;
    if (out.bounding.x() + out.bounding.width() > m_compact_w)
        m_compact_w = out.bounding.x() + out.bounding.width();
    if (out.bounding.y() + out.bounding.height() > m_compact_h)
        m_compact_h = out.bounding.y() + out.bounding.height();
    if (m_config) {
        if (m_config->onePixelOffset()) {
          out.bounding.adjust(1, 1, -1, -1);
        }
    }
    if (m_data)
        m_data->placeChar(out);

}
