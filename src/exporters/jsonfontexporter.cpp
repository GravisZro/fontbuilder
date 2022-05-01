#include "jsonfontexporter.h"
#include "../fontconfig.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

JSONFontExporter::JSONFontExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("json");
}

bool JSONFontExporter::Export(QByteArray &out)
{
    const FontConfig* cfg = fontConfig();

    QJsonObject jsonConfig;
    jsonConfig.insert("face",cfg->family());
    jsonConfig.insert("size", cfg->size());
    jsonConfig.insert("bold", (cfg->bold() ? 1 : 0));
    jsonConfig.insert("italic", (cfg->italic() ? 1 : 0));
    jsonConfig.insert("smooth", (cfg->antialiased() ? 1 : 0));
    jsonConfig.insert("charSpacing", cfg->charSpacing());
    jsonConfig.insert("lineSpacing", cfg->lineSpacing());

    jsonConfig.insert("charHeight", metrics().height);
    jsonConfig.insert("base", metrics().ascender);
    jsonConfig.insert("textureWidth", texWidth());
    jsonConfig.insert("textureHeight", texHeight());
    jsonConfig.insert("textureFile", texFilename());

    QJsonArray jsonSymbols;
    foreach(const Symbol& c , symbols())
    {
        QJsonObject jsonSymbol;
        jsonSymbol.insert("id", int32_t(c.id));
        jsonSymbol.insert("x", c.placeX);
        jsonSymbol.insert("y", c.placeY);
        jsonSymbol.insert("width", c.placeW);
        jsonSymbol.insert("height", c.placeH);
        jsonSymbol.insert("xoffset", c.offsetX);
        jsonSymbol.insert("yoffset", metrics().ascender - c.offsetY);
        jsonSymbol.insert("xadvance", c.advance);

        jsonSymbols.append( jsonSymbol );
    }

    QJsonArray jsonKernels;
    typedef QMap<uint32_t,int32_t>::ConstIterator Kerning;
    foreach(const Symbol& c , symbols())
    {
        if (c.kerning.empty())
        {
            continue;
        }

        QJsonObject jsonKernel;
        for (const auto& k : c.kerning)
        {
            jsonKernel.insert("first", int32_t(c.id));
            jsonKernel.insert("second", int32_t(k.first));
            jsonKernel.insert("amount", k.second);
        }
        jsonKernels.append( jsonKernel );
    }

    QJsonObject jsonFont;
    jsonFont.insert("config", jsonConfig);
    jsonFont.insert("symbols", jsonSymbols);
    jsonFont.insert("kerning", jsonKernels);

    QJsonDocument doc(jsonFont);
    out.append( doc.toJson(QJsonDocument::Indented) );

    return true;
}

AbstractExporter* JSONFontExporterFactoryFunc (QObject* parent) {
    return new JSONFontExporter(parent);
}
