#include "carrayexporter.h"
#include "../fontconfig.h"

CArrayExporter::CArrayExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("h");
}

bool CArrayExporter::Export(QByteArray &out)
{
    QString filename = texFilename();
    int32_t _pos = filename.indexOf('.');
    filename.truncate(_pos);
    int32_t charsnum = 0;
    int32_t kernnum = 0;

    const FontConfig* cfg = fontConfig();
/*
    QString tmp = QString("%1%2").arg(cfg->bold() ? "_bold" : "").arg(cfg->italic() ? "_italic" : "");
        if (!cfg->bold() && !cfg->italic())
            tmp = QString("_regular");
*/
    QString res;

    res += QString("#ifndef %1%2_%3_H\n"
                     "#define %1%2_%3_H\n"
                     "\n"
                     "#include \"pf_gui_font.h\"\n"
                     "\n")
             .arg(cfg->family().toUpper())
             .arg(filename.toUpper())
             .arg(cfg->size());

/*
 * struct font_info
 * {
 *      char face[];
 *      uint16_t size;
 *      uint8_t bold;
 *      uint8_t italic;
 *      uint8_t smooth;
 *      uint8_t charSpacing;
 *      uint8_t lineSpacing;
 * }
 */
    res += QString("#ifndef FI_%1\n"
                     "#define FI_%1 \\\n"
                     "\tstatic const struct font_info __attribute__((section(\".ExtFlash\"))) fi_%2 = { \\\n"
                     "\t\t.face=\"%3\", \\\n"
                     "\t\t.size=%4, \\\n"
                     "\t\t.bold=%5, \\\n"
                     "\t\t.italic=%6, \\\n"
                     "\t\t.smooth=%7, \\\n"
                     "\t\t.charSpacing=%8, \\\n"
                     "\t\t.lineSpacing=%9 \\\n"
                     "\t};\n"
                     "#endif\n"
                     "\n")
             .arg(filename).toUpper()
             .arg(filename).toLower()
             .arg(cfg->family())
             .arg(cfg->size())
             .arg(cfg->bold() ? 1 : 0)
             .arg(cfg->italic() ? 1 : 0)
             .arg(cfg->antialiased() ? 1 : 0)
             .arg(cfg->charSpacing())
             .arg(cfg->lineSpacing());


/*
 * struct font_common
 * {
 *      uint16_t lineHeight;
 *      uint16_t base;
 *      uint16_t scaleW;
 *      uint16_t scaleH;
 *      uint8_t pages;
 * }
 */
    res += QString("#ifndef FC_%1\n"
                     "#define FC_%1 \\\n"
                     "\tstatic const struct font_common __attribute__((section(\".ExtFlash\"))) fc_%2 = { \\\n"
                     "\t\t.lineHeight=%3, \\\n"
                     "\t\t.base=%4, \\\n"
                     "\t\t.scaleW=%5, \\\n"
                     "\t\t.scaleH=%6, \\\n"
                     "\t\t.pages=%7 \\\n"
                     "\t};\n"
                     "#endif\n\n")
             .arg(filename.toUpper())
             .arg(filename.toUpper())
             .arg(metrics().height)
             .arg(metrics().ascender)
             .arg(texWidth())
             .arg(texHeight())
             .arg(1);
/*
 * struct font_page
 * {
 *      uint8_t id;
 *      char file[];
 * }
 */
    res += QString("#ifndef FP_%1\n"
                     "#define FP_%1 \\\n"
                     "\tstatic const struct font_page __attribute__((section(\".ExtFlash\"))) fp_%2 = { \\\n"
                     "\t\t.id=%3, \\\n"
                     "\t\t.file=\"%4\" \\\n"
                     "\t};\n"
                     "#endif\n\n")
             .arg(filename.toUpper())
             .arg(filename.toLower())
             .arg(0)
             .arg(texFilename());

/*
 * struct font_char
 * {
 *      uint16_t id;
 *      uint16_t x;
 *      uint16_t y;
 *      uint16_t width;
 *      uint16_t height;
 *      uint16_t xoffset;
 *      uint16_t yofset;
 *      uint16_t xadvance;
 *      uint8_t page;
 * }
 */
    res += QString("#ifndef FCH_%1\n"
                     "#define FCH_%1 \\\n"
                     "\tstatic const struct font_char __attribute__((section(\".ExtFlash\"))) fch_%2[] = { \\\n")
             .arg(filename.toUpper())
             .arg(filename.toLower());

    foreach(const Symbol& c , symbols()) {
        res += QString("\t\t{ .id=%1,\t.x=%2,\t.y=%3,\t.width=%4,\t.height=%5,\t.xoffset=%6,\t.yoffset=%7,\t.xadvance=%8,\t.page=%9 }, /* %2 */ \\\n")
                 .arg(c.id)
                 .arg(c.placeX)
                 .arg(c.placeY)
                 .arg(c.placeW)
                 .arg(c.placeH)
                 .arg(c.offsetX)
                 .arg(metrics().ascender - c.offsetY)
                 .arg(c.advance)
                 .arg(0);
        charsnum++;
    }
    res += "\t};\n";

    //out.append( QString("\tconst uint16_t ").toUtf8() + QString("%1_CHARSNUM").arg(filename).toUpper().toUtf8() + QString(" = sizeof(fch_%1)/sizeof(fch_%1[0]);\n").arg(filename).toUtf8());

    res += "#endif\n\n";
/*
 * struct font_kerning
 * {
 *      uint16_t first;
 *      uint16_t second;
 *      int16_t amount;
 * }
 */

    res += QString("#ifndef FK_%1\n"
                     "#define FK_%1 \\\n"
                     "\tstatic const struct font_kerning __attribute__((section(\".ExtFlash\"))) fk_%1[] = { \\\n")
             .arg(filename.toUpper())
             .arg(filename);

    foreach(const Symbol& c , symbols()) {
            for(auto& k : c.kerning) {
              res += QString("\t\t{ .first = %1, .second = %2, .amount = %4 }, \\\n")
                       .arg(c.id)
                       .arg(k.first)
                       .arg(k.second);
              kernnum++;
            }
    }

    res += "\t};\n";
/*
    entry += QString("\tconst uint16_t %1_KERNNUM = sizeof(fk_%1)/sizeof(fk_%1[0]);\n")
             .arg(filename.toUpper())
             .arg(filename);
*/
    res += "#endif\n\n";

    /*
     * struct font
     * {
     *      struct *font_info;
     *      struct *font_commn;
     *      struct *font_page;
     *      struct *font_char;
     *      struct *font_kerning;
     *      uint8_t *font_data;
     *      uint16_t charsnum;
     *      uint16_t kernnum;
     * }
     *
     * const struct font NAME;
     */

    res += QString("#ifndef F_%1\n"
                     "#define F_%1 \\\n"
                     "const struct font __attribute__((section(\".ExtFlash\"))) f_%2 = { \\\n"
                     "\t.fi=&fi_%2, \\\n"
                     "\t.fc=&fc_%2, \\\n"
                     "\t.fch=fch_%2, \\\n"
                     "\t.fk=fk_%2, \\\n"
                     "\t.ff=font_data_%3, \\\n"
                     "\t.charsnum=%4, \\\n"
                     "\t.kernnum=%5 \\\n"
                     "};\n\n"
                     "#endif\n\n")
             .arg(filename.toUpper())
             .arg(filename.toLower())
             .arg(filename)
             .arg(charsnum)
             .arg(kernnum);

    res += QString("extern const struct font f_%1;\n"
//                     "extern const uint16_t %2_CHARSNUM;\n\n"
//                     "extern const uint16_t %2_KERNNUM;\n\n"
                     "#ifndef %2\n"
                     "#define %2 \\\n"
                     "\tFI_%2 \\\n"
                     "\tFP_%2 \\\n"
                     "\tFCH_%2 \\\n"
                     "\tFK_%2 \\\n"
                     "\tF_%2\n"
                     "#endif\n\n")
             .arg(filename.toLower())
             .arg(filename.toUpper());

    res += "#endif\n";

    out = res.toUtf8();

    return true;
}

AbstractExporter* CArrayExporterFactoryFunc (QObject* parent) {
    return new CArrayExporter(parent);
}
