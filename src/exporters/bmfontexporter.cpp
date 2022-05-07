#include "bmfontexporter.h"
#include <fontconfig.h>

BMFontExporter::BMFontExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("fnt");
}

bool BMFontExporter::Export(QByteArray &out)
{
    // Format description:
    // http://www.angelcode.com/products/bmfont/doc/file_format.html

    const FontConfig* cfg = fontConfig();

    QString res;

    res += QString("info face=\"%1\" size=%2 bold=%3 italic=%4 smooth=%5 spacing=%6,%7\n")
             .arg(cfg->family())
             .arg(cfg->size())
             .arg(cfg->bold() ? 1 : 0)
             .arg(cfg->italic() ? 1 : 0)
             .arg(cfg->antialiased() ? 1 : 0)
             .arg(cfg->charSpacing())
             .arg(cfg->lineSpacing());

    res += QString("common lineHeight=%1 base=%2 scaleW=%3 scaleH=%4 pages=1\n")
             .arg(metrics().height)
             .arg(metrics().ascender)
             .arg(texWidth())
             .arg(texHeight());

    res += QString("page id=%1 file=\"%1\"\n")
             .arg(0)
             .arg(texFilename());


    for (const Symbol& sym : symbols())
        res += QString("char id=%1 x=%2 y=%3 width=%4 height=%5 xoffset=%6 yoffset=%7 xadvance=%8 page=%9\n")
                 .arg(sym.id)
                 .arg(sym.place.x())
                 .arg(sym.place.y())
                 .arg(sym.place.width())
                 .arg(sym.place.height())
                 .arg(sym.offset.x())
                 .arg(metrics().ascender - sym.offset.y())
                 .arg(sym.advance)
                 .arg(0);

    for (const Symbol& sym : symbols())
      for(auto& k : sym.kerning)
        res += QString("kerning first=%1 second=%2 amount=%3")
                 .arg(sym.id)
                 .arg(k.first)
                 .arg(k.second);

    out = res.toUtf8();

    return true;
}

AbstractExporter* BMFontExporterFactoryFunc (QObject* parent) {
    return new BMFontExporter(parent);
}
