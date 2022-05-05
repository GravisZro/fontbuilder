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

#ifndef CHARSSELECTWIDGET_H
#define CHARSSELECTWIDGET_H

#include <QWidget>
#include <QSet>

class CharsSelectWidget : public QWidget
{
Q_OBJECT
public:
    explicit CharsSelectWidget(QWidget *parent = nullptr);

    void setRange(uint32_t begin, uint32_t end);

    QSet<uint32_t> getCharacterCodes(void) { return m_character_codes; }
    void clearCharacterCodes(void) { m_character_codes.clear(); }
    void insertCharacterCode(uint32_t c) { m_character_codes.insert(c); }
    bool removeCharacterCode(uint32_t c) { return m_character_codes.remove(c); }

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);
    virtual void mouseMoveEvent(QMouseEvent *);
    virtual QSize sizeHint() const ;
signals:
    void codesChanged(uint32_t code,bool add);
public slots:

private:
    QSet<uint32_t> m_character_codes;
    uint32_t    m_codes_begin;
    uint32_t    m_codes_end;
    uint32_t    m_select_begin_code;
    uint32_t    m_select_last_code;

    bool m_track_mouse;
    bool m_track_erase;
};

#endif // CHARSSELECTWIDGET_H
