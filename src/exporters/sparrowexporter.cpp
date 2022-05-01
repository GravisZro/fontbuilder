#include "sparrowexporter.h"
#include "../fontconfig.h"
#include <QtXml>

SparrowExporter::SparrowExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("fnt");
}

bool SparrowExporter::Export(QByteArray& out) {
    QDomDocument doc;
    QDomElement root = doc.createElement("font");
    doc.appendChild(root);
    QDomElement info = doc.createElement("info");
    root.appendChild(info);
    info.setAttribute("face", fontConfig()->family());
    info.setAttribute("size", fontConfig()->size());
    QDomElement common = doc.createElement("common");
    root.appendChild(common);
    int32_t height = metrics().height;
    common.setAttribute("lineHeight", height);
    QDomElement pages = doc.createElement("pages");
    root.appendChild(pages);
    QDomElement page = doc.createElement("page");
    pages.appendChild(page);
    page.setAttribute("id", "0");
    page.setAttribute("file", texFilename());
    QDomElement chars = doc.createElement("chars");
    root.appendChild(chars);
    chars.setAttribute("count", symbols().size());
    QDomElement kernings = doc.createElement("kernings");
    int32_t kernNumber = 0;
    foreach(const Symbol& c , symbols()) {
        QDomElement ch = doc.createElement("char");
        ch.setAttribute("id", QString::number(c.id));
        ch.setAttribute("x", QString::number(c.placeX));
        ch.setAttribute("y", QString::number(c.placeY));
        ch.setAttribute("width", QString::number(c.placeW));
        ch.setAttribute("height", QString::number(c.placeH));
        ch.setAttribute("xoffset", QString::number(c.offsetX));
        ch.setAttribute("yoffset", QString::number(height - c.offsetY));
        ch.setAttribute("xadvance", QString::number(c.advance));
        ch.setAttribute("page", "0");
        ch.setAttribute("chnl", "0");
        ch.setAttribute("letter", c.id==32 ? "space" : QString().append(c.id));
        chars.appendChild(ch);

        for (const auto& k : c.kerning) {
            QDomElement ke = doc.createElement("kerning");
            ke.setAttribute("first", QString::number(c.id));
            ke.setAttribute("second",QString().append(k.first));
            ke.setAttribute("amount",k.second);
            kernings.appendChild(ke);
            kernNumber++;
        }
    }
    kernings.setAttribute("count", QString::number(kernNumber));
    root.appendChild(kernings);

    out = doc.toByteArray(1);
    return true;
}

AbstractExporter* SparrowExporterFactoryFunc (QObject* parent) {
    return new SparrowExporter(parent);
}
