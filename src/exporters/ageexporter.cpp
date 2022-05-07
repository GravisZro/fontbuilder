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
    std::vector<Symbol> list = symbols();
    std::sort(list.begin(), list.end(), sortSymbols);

    unsigned charsCount = list.size();
    unsigned maxHeight = 0;

    for (const Symbol& sym : list)
      maxHeight = std::max<float>(maxHeight, sym.place.height());

    AGEHeader header;
    memcpy(header.header, AGEsig, sizeof(header.header));
    header.count = charsCount;
    header.height = maxHeight;
    append(out, header);

    for (const Symbol& sym : list)
    {
        append(out, (unsigned)sym.id);
        append(out, (float)sym.place.x());
        append(out, (float)sym.place.y());
        append(out, (float)(sym.place.x() + sym.place.width()));
        append(out, (float)(sym.place.y() + sym.place.height()));
        append(out, (float)sym.offset.x());
        append(out, (float)(maxHeight - sym.offset.y()));
        append(out, (float)sym.advance);
    }

    return true;
}

AbstractExporter* AGEExporterFactoryFunc(QObject* parent)
{
    return new AGEExporter(parent);
}
