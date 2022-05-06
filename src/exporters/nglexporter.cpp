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

#include "nglexporter.h"
#include <fontconfig.h>
#include <layoutdata.h>
#include <QDomDocument>

NGLExporter::NGLExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("xml");
}

bool NGLExporter::Export(QByteArray& out) {
    QDomDocument doc;
    QDomElement root = doc.createElement("font");

    doc.appendChild(doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"utf-8\"" ));

    doc.appendChild(root);
    root.setAttribute("type","NGL");

    QDomElement descr = doc.createElement("description");
    descr.setAttribute("family",fontConfig()->family());
    descr.setAttribute("style",fontConfig()->style());
    descr.setAttribute("size",fontConfig()->size());
    root.appendChild(descr);

    QDomElement fmetrics = doc.createElement("metrics");
    fmetrics.setAttribute("ascender",metrics().ascender);
    fmetrics.setAttribute("descender",metrics().descender);
    fmetrics.setAttribute("height",metrics().height);
    root.appendChild(fmetrics);

    QDomElement tex = doc.createElement("texture");
    tex.setAttribute("width",texWidth());
    tex.setAttribute("height",texHeight());
    tex.setAttribute("file",texFilename());
    root.appendChild(tex);

    QDomElement chars = doc.createElement("chars");
    foreach (const Symbol& c , symbols()) {
        QDomElement ce = doc.createElement("char");
        ce.setAttribute("id",QString().append(c.id));
        ce.setAttribute("rect_x",c.place.x());
        ce.setAttribute("rect_y",c.place.y());
        ce.setAttribute("rect_w",c.place.width());
        ce.setAttribute("rect_h",c.place.height());
        ce.setAttribute("offset_x",c.offset.x());
        ce.setAttribute("offset_y",c.offset.y());
        ce.setAttribute("advance",c.advance);

        for (const auto& k : c.kerning) {
            QDomElement ke = doc.createElement("kerning");
            ke.setAttribute("id",QString().append(k.first));
            ke.setAttribute("advance",k.second);
            ce.appendChild(ke);
        }

        chars.appendChild(ce);
    }
    root.appendChild(chars);
    out = doc.toByteArray(1);
    return true;
}


AbstractExporter* NGLExporterFactoryFunc (QObject* parent) {
    return new NGLExporter(parent);
}
