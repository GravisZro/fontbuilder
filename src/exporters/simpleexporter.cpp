#include "simpleexporter.h"
#include <fontconfig.h>

SimpleExporter::SimpleExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("sfl");
}

bool SimpleExporter::Export(QByteArray &out)
{
    const FontConfig* cfg = fontConfig();
    QString res;

    res += QString("%1\n%2 %3\n%4\n%5\n")
           .arg(cfg->family())
           .arg(cfg->size())
           .arg(metrics().height)
           .arg(texFilename())
           .arg(symbols().size());

    for (const Symbol& sym : symbols())
      res += QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
             .arg(sym.id)
             .arg(sym.place.x())
             .arg(sym.place.y())
             .arg(sym.place.width())
             .arg(sym.place.height())
             .arg(sym.offset.x())
             .arg(metrics().height - sym.offset.y())
             .arg(sym.advance);

    res += "%1\n";
    int32_t kerningsCount = 0;
    for (const Symbol& sym : symbols())
      for ( const auto& k : sym.kerning)
      {
        res += QString("%1 %2 %3 \n")
               .arg(sym.id)
               .arg(k.first)
               .arg(k.second);
        ++kerningsCount;
      }

    out = res.arg(kerningsCount).toUtf8();

    return true;
}

AbstractExporter* SimpleExporterFactoryFunc (QObject* parent) {
    return new SimpleExporter(parent);
}
