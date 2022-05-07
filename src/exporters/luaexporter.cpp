/**
 * Copyright (c) 2010-2011 Andrey AndryBlack Kunitsyn
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

#include "luaexporter.h"
#include <fontconfig.h>
#include <layoutdata.h>

#include <QString>
LuaExporter::LuaExporter(bool write_function,QObject *parent) :
    AbstractExporter(parent), m_write_function(write_function)
{
    setExtension("lua");
}

static QString charCode(uint32_t code)
{
    if (code=='\"')
      return QString("'\"'");
    if (code=='\\') return QString("\"\\\\\"");
    return QString("\"%1\"").arg(QChar(code));
}

bool LuaExporter::Export(QByteArray& out) {
    QString res ="";

    res += "%3";
    res += QString("%%1file=\"%1\"%%2\n"
                   "%%1height=%2%%2\n"
                   "%%1description = {\n"
                   "%%1\tfamily=\"%3\",\n"
                   "%%1\tstyle=\"%4\",\n"
                   "%%1\tsize=%5\n"
                   "%%1}%%2\n"
                   "%%1metrics = {\n"
                   "%%1\tascender=%6,\n"
                   "%%1\tdescender=%7,\n"
                   "%%1\theight=%2\n"
                   "%%1}%%2\n"
                   "%%1texture = {\n"
                   "%%1\tfile=\"%1\",\n"
                   "%%1\twidth=%8,\n"
                   "%%1\theight=%9\n"
                   "%%1}%%2\n"
                   "%%1chars = {\n")
           .arg(texFilename())
           .arg(metrics().height)
           .arg(fontConfig()->family())
           .arg(fontConfig()->style())
           .arg(fontConfig()->size())
           .arg(metrics().ascender)
           .arg(metrics().descender)
           .arg(texWidth())
           .arg(texHeight());


    for (const Symbol& sym : symbols())
      res += QString("%%1\t{ char=%1, width=%2, x=%3, y=%4, w=%5, h=%6, ox=%7, oy=%8 },\n")
             .arg(charCode(sym.id))
             .arg(sym.advance)
             .arg(sym.place.x())
             .arg(sym.place.y())
             .arg(sym.place.width())
             .arg(sym.place.height())
             .arg(sym.offset.x())
             .arg(sym.offset.y());
    res += "}%2\n";

    res += "%1kernings = {\n";
    for (const Symbol& sym : symbols())
      for ( const auto& k : sym.kerning)
        res += QString("%%1\t{ from=%1, to=%2, offset=%3,\n")
               .arg(charCode(sym.id))
               .arg(charCode(k.first))
               .arg(k.second);

    res += "}%2\n";
    res += "%4\n";

    if (m_write_function)
      out = res
            .arg("\t")
            .arg(",")
            .arg("return {\n")
            .arg("}\n")
            .toUtf8();
    else
      out = res.arg("").arg("").arg("").arg("").toUtf8();

    return true;
}

AbstractExporter* LuaTableExporterFactoryFunc (QObject* parent) {
    return new LuaExporter(false, parent);
}
AbstractExporter* LuaFunctionExporterFactoryFunc (QObject* parent) {
    return new LuaExporter(true, parent);
}
