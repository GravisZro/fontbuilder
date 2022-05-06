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

#ifndef LAYOUTCONFIG_H
#define LAYOUTCONFIG_H

#include <QObject>
#include <QRect>

class LayoutConfig : public QObject
{
Q_OBJECT
public:
    explicit LayoutConfig(QObject *parent = nullptr);

    bool onePixelOffset() const { return m_one_pixel_offset;}
    void setOnePixelOffset(bool b);
    Q_PROPERTY( bool onePixelOffset READ onePixelOffset WRITE setOnePixelOffset CONSTANT);

    bool potImage() const { return m_pot_image;}
    void setPotImage(bool b);
    Q_PROPERTY( bool potImage READ potImage WRITE setPotImage CONSTANT);

    int sizeIncrement() const { return m_size_increment;}
    void setSizeIncrement(int v);
    Q_PROPERTY( int sizeIncrement READ sizeIncrement WRITE setSizeIncrement CONSTANT);


    QRect offset(void) const { return m_offset; }
    void setOffset(QRect o);
    Q_PROPERTY( QRect offset READ offset WRITE setOffset CONSTANT);

    const QString& layouter() const { return m_layouter;}
    void setLayouter(const QString& layouter) { m_layouter = layouter;}
    Q_PROPERTY( QString layouter READ layouter WRITE setLayouter CONSTANT);

signals:
    void layoutConfigChanged();

private:
    QString m_layouter;
    bool    m_one_pixel_offset;
    bool    m_pot_image;
    int32_t m_size_increment;
    QRect   m_offset;
};

#endif // LAYOUTCONFIG_H
