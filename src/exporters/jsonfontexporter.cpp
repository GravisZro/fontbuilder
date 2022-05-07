#include "jsonfontexporter.h"
#include <fontconfig.h>
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
    for (const Symbol& sym : symbols())
    {
        QJsonObject jsonSymbol;
        jsonSymbol.insert("id", int32_t(sym.id));
        jsonSymbol.insert("x", sym.place.x());
        jsonSymbol.insert("y", sym.place.y());
        jsonSymbol.insert("width", sym.place.width());
        jsonSymbol.insert("height", sym.place.height());
        jsonSymbol.insert("xoffset", sym.offset.x());
        jsonSymbol.insert("yoffset", metrics().ascender - sym.offset.y());
        jsonSymbol.insert("xadvance", sym.advance);

        jsonSymbols.append( jsonSymbol );
    }

    QJsonArray jsonKernels;
    for (const Symbol& sym : symbols())
    {
        if (sym.kerning.empty())
        {
            continue;
        }

        QJsonObject jsonKernel;
        for (const auto& k : sym.kerning)
        {
            jsonKernel.insert("first", int32_t(sym.id));
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
