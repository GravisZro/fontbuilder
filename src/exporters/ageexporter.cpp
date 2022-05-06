#include "ageexporter.h"


#include <fontconfig.h>
#include <layoutdata.h>

struct AGEHeader
{
    char header[4];
    unsigned count;
    float height;
};
static const char* AGEsig = "AGEF";

template<typename T>
void append(QByteArray& out, T val)
{
    out.append((char*)&val, sizeof(T));
}



AGEExporter::AGEExporter(QObject* parent)
    : AbstractExporter(parent)
{
    setExtension("agefnt");
}

bool AGEExporter::sortSymbols(const Symbol& a, const Symbol& b)
{
    return a.id < b.id;
}

bool AGEExporter::Export(QByteArray& out)
{
    QVector<Symbol> list = symbols();
    std::sort(list.begin(), list.end(), sortSymbols);

    unsigned charsCount = list.size();
    unsigned maxHeight = 0;

    foreach(const Symbol& c, list)
    {
        maxHeight = std::max<float>(maxHeight, c.place.height());
    }

    AGEHeader header;
    memcpy(header.header, AGEsig, sizeof(header.header));
    header.count = charsCount;
    header.height = maxHeight;
    append(out, header);

    foreach(const Symbol& c, list)
    {
        append(out, (unsigned)c.id);
        append(out, (float)c.place.x());
        append(out, (float)c.place.y());
        append(out, (float)(c.place.x() + c.place.width()));
        append(out, (float)(c.place.y() + c.place.height()));
        append(out, (float)c.offset.x());
        append(out, (float)(maxHeight - c.offset.y()));
        append(out, (float)c.advance);
    }

    return true;
}

AbstractExporter* AGEExporterFactoryFunc(QObject* parent)
{
    return new AGEExporter(parent);
}
