#include "mpwriter.h"

#include "builtinimagewriter.h"
#include "layoutdata.h"
#include <QImage>
#include <QPainter>
#include <QFileInfo>
#include "../layoutconfig.h"
#include "../mpcommon.h"

#include <QDebug>
#include <iostream>

MPImageWriter::MPImageWriter(QString ext,QObject *parent) :
    AbstractImageWriter(parent)
{
    setExtension(ext);
    setReloadSupport(true);
}

bool MPImageWriter::Export(QFile& file)
{
    //move to start of the file
    file.seek(0);

    //read the header of the file
    Font font;
    font.load( file );
//    file.read( reinterpret_cast<char*>(&font), sizeof(gui_font_img_t));

    std::vector<FontGlyph> font_glyphs;
    //loads from file information about glyphs
    for( uint32_t i=0; i<font.glyph_count; ++i )
    {
        FontGlyph glyph;
        glyph.load( file );
//        file.read( reinterpret_cast<char*>(&glyph), sizeof(gui_font_glyph_t));
        font_glyphs.push_back(glyph);
    }

    //move file pointer back to the end
    file.seek(font.image_data_offset);

    //loads from file location of glyphs in the pixmap
    std::vector<pixmap_coords_t> pixmap_coords;
    for( uint32_t i = 0; i< font.glyph_count; i++ ){
        pixmap_coords_t coords;
        file.read( reinterpret_cast<char*>(&coords), sizeof(pixmap_coords_t));
        pixmap_coords.push_back(coords);
    }

    //move file pointer back to the end
    file.seek(font.image_data_offset);

    QImage pixmap = buildImage();

    QDataStream out(&file);
    //for all glyphs go over each line of the glyph, get pixel from the pixmap and save truncuated value to the file.

    std::cout<<std::endl;
    for( uint32_t i=0; i<font_glyphs.size(); ++i)
    {
        FontGlyph glyph = font_glyphs[i];
        for (int32_t y=0; y<glyph.height; y++)
        {
            for (int32_t x = 0; x < glyph.width; x++)
            {
                uint8_t alfaChannel = static_cast<uint8_t>(qAlpha(pixmap.pixel(x+pixmap_coords[i].x, y + pixmap_coords[i].y)));
                // Scale the alfa channel to to for 1 bit color
                uint8_t scaledValue = alfaChannel > 127 ? 0x00 : 0x0F;
                out.writeRawData( reinterpret_cast<char*>(&scaledValue), 1 );
//                if( scaledValue == 15 )
//                    std::cout<<"_";
//                else
//                    std::cout<<"X";
            }
            std::cout<<std::endl;
        }
        std::cout<<std::endl;
    }

    return true;
}

QImage* MPImageWriter::reload(QFile& file) {
    QImage* img = nullptr;

    return img;
}
