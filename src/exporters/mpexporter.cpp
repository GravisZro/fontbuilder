#include <iostream>
#include "mpexporter.h"
#include "../fontconfig.h"
#include "../mpcommon.h"

typedef uint32_t ucode32;

#pragma pack(1)


MPExporter::MPExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("mpf");
}

bool MPExporter::Export(QByteArray &out)
{
    QString filename = texFilename();
    int32_t _pos = filename.indexOf('.');
    filename.truncate(_pos);

    const FontConfig* cfg = fontConfig();

    Font font_img;
    memset( font_img.info.face, 0, sizeof(font_img.info.face));
//    memset(&font_img, 0, sizeof(gui_font_img_t));

    //name of the font
    std::string name = cfg->family().toLower().toStdString()+
        "_"+cfg->style().toLower().toStdString()+
        "_"+std::to_string(cfg->size());
    std::replace( name.begin(), name.end(), ' ', '_');
    if( name.length() > 64 )
        name = name.substr(0,63 );
    strncat(font_img.info.face, name.c_str(), 63);
    //size of the true type font
    font_img.info.size = static_cast<uint16_t>(cfg->size());
    //flag that informs if font is bold
    font_img.info.bold = cfg->bold() ? 1 : 0;
    //flag that informs if font is italic
    font_img.info.italic = cfg->italic() ? 1 : 0;
    //flag that informs if smoothing was turned on. 1 - smoothing was turned on.
    font_img.info.smooth = cfg->antialiased() ? 1 : 0;
    //TODO additional space between characters????
    font_img.info.char_spacing = static_cast<uint16_t>(cfg->charSpacing());
    //TODO additional space between lines
    font_img.info.line_spacing = static_cast<uint16_t>(cfg->lineSpacing());
    //distance in pixels between each line of text
    font_img.info.line_height = static_cast<uint16_t>(metrics().height);
    //number of pixels from the absolute top of the line to the base of the characters
    font_img.info.base = static_cast<uint16_t>(metrics().ascender);
    //width of the texture, normally used to scale the x pos of the character image
    font_img.info.scale_w = static_cast<uint16_t>(texWidth());
    //height of the texture, normally used to scale the y pos of the character image
    font_img.info.scale_h = static_cast<uint16_t>(texHeight());

    //number of glyphs in the font
    font_img.glyph_count = static_cast<uint32_t>(symbols().size());
    //offset to the beginning of the glyph data
    font_img.glyph_data_offset = font_img.getSize();

    //number of kerning pairs
    font_img.kern_count = 0;
    foreach(const Symbol& c , symbols()) {
        font_img.kern_count += static_cast<uint16_t>(c.kerning.size());
    }
    //array of glyphs structures
    font_img.kern_data_offset = font_img.getSize() + font_img.glyph_count * FontGlyph::getSize();
    //offset to the beginning of the image data
    font_img.image_data_offset = font_img.kern_data_offset + font_img.kern_count * FontKerning::getSize();

    //writing font header
    font_img.save( out );
    //char* font_img_ptr = reinterpret_cast<char*>(&font_img);
    //out.append(QByteArray::fromRawData( font_img_ptr, sizeof(gui_font_img_t)));

    //setting up glyph structures and byte array for image data
    QByteArray imgArray;

    uint32_t offset = 0;
    //this loop goes over all characters and writes to stream information about glyph's dimension and offset to pixmap data
    foreach(const Symbol& c , symbols()) {
        FontGlyph glyph;

        //character id
        glyph.id = static_cast<ucode32>(c.id);
        //offset in glyph data field
        glyph.glyph_offset = font_img.image_data_offset+offset;
        //width of the character image in the texture
        glyph.width = static_cast<uint16_t>(c.placeW);
        //height of the character image in the texture
        glyph.height = static_cast<uint16_t>(c.placeH);
        //how much the current position should be offset when copying the image from the texture to the screen
        glyph.xoffset = static_cast<int16_t>(c.offsetX);
        //how much the current position should be offset when copying the image from the texture to the screen
        glyph.yofset = static_cast<int16_t>(c.offsetY);
        //how much the current position should be advanced after drawing the character
        glyph.xadvance = static_cast<uint16_t>(c.advance);
        offset += glyph.width * glyph.height;

        glyph.save( out );
        //char* glyph_ptr = reinterpret_cast<char*>(&glyph);
        //out.append(QByteArray::fromRawData( glyph_ptr, sizeof(gui_font_glyph_t)));
    }

    //this loop goes over all characters and writes to stream information about kerning for each glyph
    foreach(const Symbol& c , symbols())
    {
        FontKerning kern;
        for (const auto& k : c.kerning)
        {
            //utf16 id of the first character
            kern.first = static_cast<ucode32>(c.id);
            //utf16 id of the following character
            kern.second = static_cast<ucode32>(k.first);
            //distance in pixels between beginning of first character and beginning of second character
            kern.amount = static_cast<int16_t>(k.second);
            //write to file
            kern.save( out );
            //char* kern_ptr = reinterpret_cast<char*>(&kern);
            //out.append(QByteArray::fromRawData( kern_ptr, sizeof(gui_font_kerning_t)));
        }  
    }

    //writes to file temporary data about location of each glyph in the pixmap
    foreach(const Symbol& c , symbols()) {
        pixmap_coords_t coord;

        coord.x = static_cast<uint32_t>(c.placeX);
        coord.y = static_cast<uint32_t>(c.placeY);
        char* coord_ptr = reinterpret_cast<char*>(&coord);
        out.append(QByteArray::fromRawData( coord_ptr, sizeof(pixmap_coords_t)));
    }

    return true;
}

AbstractExporter* MPExporterFactoryFunc (QObject* parent) {
    return new MPExporter(parent);
}
