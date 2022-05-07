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

    foreach(const Symbol& c , symbols()) {
      res += QString("%1 %2 %3 %4 %5 %6 %7 %8\n")
             .arg(c.id)
             .arg(c.place.x())
             .arg(c.place.y())
             .arg(c.place.width())
             .arg(c.place.height())
             .arg(c.offset.x())
             .arg(metrics().height - c.offset.y())
             .arg(c.advance);   out.append('\n');
    }

    res += "%1\n";
    int32_t kerningsCount = 0;
    foreach (const Symbol& c , symbols()) {
      for ( const auto& k : c.kerning)
      {
        res += QString("%1 %2 %3 \n")
               .arg(c.id)
               .arg(k.first)
               .arg(k.second);
        ++kerningsCount;
      }
    }

    out = res.arg(kerningsCount).toUtf8();

    return true;
}

AbstractExporter* SimpleExporterFactoryFunc (QObject* parent) {
    return new SimpleExporter(parent);
}
