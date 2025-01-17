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

#ifndef ABSTRACTLAYOUTER_H
#define ABSTRACTLAYOUTER_H

#include <QObject>
#include <vector>

#include "layoutchar.h"
#include "rendererdata.h"

class LayoutConfig;
class LayoutData;


class AbstractLayouter : public QObject
{
Q_OBJECT
public:
    explicit AbstractLayouter(QObject *parent );
    void setConfig(const LayoutConfig*);
    void setData(LayoutData* data);
private:
    const LayoutConfig*   m_config;
    LayoutData* m_data;
    std::vector<RenderedChar> m_chars;
    int32_t m_compact_w;
    int32_t m_compact_h;
    void DoPlace(const std::vector<RenderedChar>& chars);
    virtual void OptimizeLayout(std::vector<RenderedChar>& chars);
protected:
    void resize(int32_t w,int32_t h);
    int32_t width() const;
    int32_t height() const;

    void place(const RenderedChar&);
    virtual void PlaceImages(const std::vector<RenderedChar>& chars) = 0;

protected slots:
    void on_LayoutDataChanged();
signals:

public slots:
    void on_ReplaceImages(const std::vector<RenderedChar>& chars);
};

#endif // ABSTRACTLAYOUTER_H
