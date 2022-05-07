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

#include "fonttestwidget.h"
#include "layoutdata.h"
#include "rendererdata.h"
#include "fontconfig.h"
#include "unicodeutils.h"

#include <QPaintEvent>
#include <QPainter>


FontTestWidget::FontTestWidget(QWidget *parent) :
    QWidget(parent),
    m_layout_data(nullptr),
    m_renderer_data(nullptr),
    m_font_config(nullptr),
    m_use_kerning(false),
    m_align(ALIGN_CENTER)
{
}


const RenderedChar* FontTestWidget::layoutChar(char32_t symbol) const {
    for (const auto& c : m_layout_data->placed()) {
        if (symbol == c.symbol)
            return &c;
    }
    return 0;
}

int32_t FontTestWidget::lineWidth(std::u32string::const_iterator start) const
{
    int32_t x = 0;
    for(auto pos = start; pos < m_text.end() && *pos != '\n'; ++pos)
    {
      if (m_renderer_data->chars.contains(*pos))
      {
        const auto& rendered = m_renderer_data->chars[*pos];
        x += rendered.advance + m_font_config->charSpacing();
        if (useKerning())
          if (auto r = rendered.kerning.find(*pos); r != rendered.kerning.end())
            x += r->second;
      }
    }
    return x;
}

void FontTestWidget::paintEvent ( QPaintEvent * event )
{
  Q_UNUSED(event);
  Q_ASSERT(m_renderer_data);
  Q_ASSERT(m_layout_data);
  Q_ASSERT(m_font_config);

    calcBBox();
    QPainter painter(this);
    painter.fillRect(rect(),QBrush(m_bg_color));
    if (!m_renderer_data || !m_layout_data) return;

    int32_t x = m_left;

    int32_t y = m_top;

    if (m_align != ALIGN_LEFT) {
        int32_t width = lineWidth(m_text.begin());
        if (m_align == ALIGN_RIGHT) {
            x = m_left + m_width-width;
        } else {
            x = m_left + (m_width - width) / 2;
        }
    }

    for(auto pos = m_text.begin(); pos < m_text.end(); ++pos)
    {
        if (*pos=='\n') {
            x = m_left;
            if (m_align != ALIGN_LEFT) {
                int32_t width = lineWidth(pos + 1);
                if (m_align == ALIGN_RIGHT) {
                    x = m_left + m_width - width;
                } else {
                    x = m_left + (m_width - width) / 2;
                }
            }
            y += m_renderer_data->metrics.height+m_font_config->lineSpacing();
        }
        else if (m_renderer_data->chars.contains(*pos))
        {
            const auto& rendered = m_renderer_data->chars[*pos];
            const auto* rend = layoutChar(*pos);
            if (rend)
            {
                painter.drawImage(
                      rendered.image.offset() + QPoint {x, y},
                      m_layout_data->image(),
                      rendered.image.rect());
            }

            x += rendered.advance + m_font_config->charSpacing();
            if (useKerning())
              if (auto r = rendered.kerning.find(*pos); r != rendered.kerning.end())
                  x += r->second;
        }
    }
}

void FontTestWidget::calcBBox(void)
{

    int32_t left = 0;
    int32_t right = 1;
    int32_t top = 0;
    int32_t bottom = 1;

    int32_t x = left;
    int32_t y = top;
    int32_t max_x = x;
    bool first = true;
    bool last = false;

    for(auto pos = m_text.begin(); pos < m_text.end(); ++pos)
    while (*pos)
    {
        if (*pos == '\n')
        {
            x=0;
            y += m_renderer_data->metrics.height+m_font_config->lineSpacing();
            first = true;
        }
        else if (m_renderer_data->chars.contains(*pos))
        {
            const auto& rendered = m_renderer_data->chars[*pos];
            const auto* layout = layoutChar(*pos);
            if (!layout)
              continue;
            last = (pos + 1 == m_text.end()) || (*(pos + 1) == '\n');

            if (first && (rendered.image.offset().x()) < left)
              left = rendered.image.offset().x();

            if (last &&  (rendered.image.offset().x() + layout->image.width() - rendered.advance - m_font_config->charSpacing()) > right)
              right = rendered.image.offset().x() + layout->image.width() - rendered.advance - m_font_config->charSpacing();

            if ( (y - rendered.image.offset().y()) < top)
                top = y - rendered.image.offset().y();
            if ( (y - rendered.image.offset().y() + layout->image.height()) > bottom)
                bottom = y - rendered.image.offset().y() + layout->image.height();

            x += rendered.advance + m_font_config->charSpacing();
            first = false;

            if (useKerning() && pos + 1 != m_text.end())
              if (auto r = rendered.kerning.find(*(pos + 1)); r != rendered.kerning.end())
                  x += r->second;

            if (x > max_x)
                max_x = x;
        }
    }

    m_left = -left+1;
    m_width = max_x;
    m_top = -top+1;
    resize(max_x+right-left+2,bottom-top+2);
    setMinimumSize(max_x+right-left+2,bottom-top+2);
}

void FontTestWidget::setText(const QString& text)
{
    m_text = convert(text);
    update();
}

void FontTestWidget::setBGColor(QColor c) {
    m_bg_color = c;
    update();
}
