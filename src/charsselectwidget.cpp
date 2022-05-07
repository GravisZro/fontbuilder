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

#include "charsselectwidget.h"

#include <QPaintEvent>
#include <QPainter>
#include <QToolTip>

static const int32_t cell_size = 24;
static const int32_t columns = 16;

CharsSelectWidget::CharsSelectWidget(QWidget *parent) :
    QWidget(parent),
    m_codes_begin(0x0000),
    m_codes_end(0xFFFF),
    m_track_mouse(false),
    m_track_erase(false)
{
    setMouseTracking(true);
}


QSize CharsSelectWidget::sizeHint() const
{
    return QSize(columns * cell_size,
                 ((m_codes_end - m_codes_begin) / columns + 1 ) * cell_size);
}


void CharsSelectWidget::setRange(char32_t begin, char32_t end)
{
  for(char32_t c : m_character_codes)
    if(c < begin || c > end)
      removeCharacterCode(c);

  m_codes_begin = begin;
  m_codes_end = end;
  setMinimumSize(sizeHint());
  setMaximumSize(sizeHint());
  adjustSize();
  update();
}

void CharsSelectWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(event->rect(),QBrush(Qt::white));

    QRect redrawRect = event->rect();
    int32_t beginRow = redrawRect.top()/cell_size;
    int32_t endRow = redrawRect.bottom()/cell_size;
    int32_t beginColumn = redrawRect.left()/cell_size;
    int32_t endColumn = redrawRect.right()/cell_size;

    painter.setPen(QPen(Qt::gray));
    for (int32_t row = beginRow; row <= endRow; ++row) {
        for (int32_t column = beginColumn; (column <= endColumn ) && (column < columns); ++column) {
            uint32_t key =  m_codes_begin+row*columns + column;
            if ( key <= m_codes_end) {
                if (m_character_codes.find(key) != m_character_codes.end())
                    painter.fillRect(column*cell_size, row*cell_size, cell_size, cell_size,QBrush(Qt::lightGray));
                painter.drawRect(column*cell_size, row*cell_size, cell_size, cell_size);
            }
        }
    }

    QFontMetrics fontMetrics(painter.font());
     painter.setPen(QPen(Qt::black));
     for (int32_t row = beginRow; row <= endRow; ++row) {

         for (int32_t column = beginColumn; (column <= endColumn ) && (column < columns); ++column) {

             uint32_t key = m_codes_begin+row*columns + column;
             if (key<=m_codes_end) {
                 painter.setClipRect(column*cell_size, row*cell_size, cell_size, cell_size);

                 QString qs = QString::fromUcs4(&key, 1);
                 painter.drawText(column*cell_size + (cell_size / 2) - fontMetrics.horizontalAdvance(qs) / 2,
                                  row*cell_size + 4 + fontMetrics.ascent(),
                                  qs);
             }
         }
     }
}


void CharsSelectWidget::mousePressEvent(QMouseEvent *event)
{
     if (event->button() == Qt::LeftButton) {
         if (event->x()<0) return;
         if (event->x()>=width()) return;
         if (event->y()<0) return;
         if (event->y()>=height()) return;

         m_select_last_code = m_select_begin_code = m_codes_begin + (event->y()/cell_size)*columns + event->x()/cell_size;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
         if (QChar::category(m_select_begin_code) != QChar::Other_NotAssigned) {
#else
         if (QChar::category(m_select_begin_code) != QChar::NoCategory) {
#endif
             m_track_mouse = true;
             if (auto pos = m_character_codes.find(m_select_begin_code); pos != m_character_codes.end()) {
                 m_character_codes.erase(pos);
                 emit codesChanged(m_select_begin_code,false);
                 m_track_erase = true;
             } else {
                 m_character_codes.insert(m_select_begin_code);
                 emit codesChanged(m_select_begin_code,true);
                 m_track_erase = false;
             }
         } else {
             m_track_mouse = false;
         }
         update();
     }
     else
         QWidget::mousePressEvent(event);
}


void CharsSelectWidget::mouseReleaseEvent(QMouseEvent *event) {
     if (event->button() == Qt::LeftButton)
        m_track_mouse = false;
     else
        QWidget::mouseReleaseEvent(event);
}

void CharsSelectWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->x()<0) return;
    if (event->x()>=width()) return;
    if (event->y()<0) return;
    if (event->y()>=height()) return;
    uint32_t code = m_codes_begin +(event->y()/cell_size)*columns + event->x()/cell_size;

    if (m_track_mouse) {
        if (code!=m_select_last_code) {
            if (code!=m_select_begin_code) {
                /*uint from = qMin(m_select_begin_code,code);
                uint to = qMax(m_select_begin_code,code);*/
                /*for (uint c = from;c<=to;c++)*/ {
                    if (m_track_erase) {
                        auto i = m_character_codes.find(code);
                        if ( i!=m_character_codes.end()) {
                            m_character_codes.erase(i);
                            emit codesChanged(code,false);
                        }
                    } else {
                        m_character_codes.insert(code);
                        emit codesChanged(code,true);
                    }
                }
            }
            m_select_last_code = code;
            update();
        }
    } else {
        QString scode = QString::number(code, 16);
        while (scode.length()<4) {
            scode=QString("0")+scode;
        }
        QString text = QString::fromLatin1("<p>Character: <span>")
                           + QChar(code)
                           + QString::fromLatin1("</span><p>Value: U+")
                           + scode;
        QToolTip::showText(event->globalPos(), text, this);
    }
}
