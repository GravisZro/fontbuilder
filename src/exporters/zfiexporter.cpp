#include "zfiexporter.h"
#include <fontconfig.h>
#include <layoutdata.h>

ZFIExporter::ZFIExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("zfi");
}

bool ZFIExporter::Export(QByteArray& out) {
  uint16_t Pages = 1;
  uint16_t Chars = 0;
  int32_t MaxHeight = 0;
  int32_t MaxShiftY = 0;
  int32_t PageWidth = texWidth();
  int32_t PageHeight = texHeight();
  uint8_t Padding[4] = { 0, 0, 0, 0 };
  zglTCharDesc CharDesc;

  for (const Symbol& sym : symbols())
  {
    if ( sym.place.height() > MaxHeight )
      MaxHeight = sym.place.height();
    if ( sym.place.height() - sym.offset.y() > MaxShiftY )
      MaxShiftY = sym.place.height() - sym.offset.y();

    Chars++;
  }

  out.append( "ZGL_FONT_INFO", 13 );
  out.append( (char*)&Pages, 2 );
  out.append( (char*)&Chars, 2 );
  out.append( (char*)&MaxHeight, 4 );
  out.append( (char*)&MaxShiftY, 4 );
  out.append( (char*)&Padding, 4 );

  for (const Symbol& sym : symbols())
  {
    int32_t id = sym.id;
    float u = 1.f / PageWidth, v = 1.f / PageHeight;
    out.append( (char*)&id, 4 );

    CharDesc.Page = 0;
    CharDesc.Width = sym.place.width();
    CharDesc.Height = sym.place.height();
    CharDesc.ShiftX = sym.offset.x();
    CharDesc.ShiftY = sym.place.height() - sym.offset.y();
    CharDesc.ShiftP = sym.advance;

    CharDesc.TexCoords[ 0 ].X = (float)u * sym.place.x();
    CharDesc.TexCoords[ 0 ].Y = 1 - (float)v * sym.place.y();
    CharDesc.TexCoords[ 1 ].X = (float)u * ( sym.place.x() + sym.place.width() );
    CharDesc.TexCoords[ 1 ].Y = 1 - (float)v * sym.place.y();
    CharDesc.TexCoords[ 2 ].X = (float)u * ( sym.place.x() + sym.place.width() );
    CharDesc.TexCoords[ 2 ].Y = 1 - (float)v * ( sym.place.y() + sym.place.height() );
    CharDesc.TexCoords[ 3 ].X = (float)u * sym.place.x();
    CharDesc.TexCoords[ 3 ].Y = 1 - (float)v * ( sym.place.y() + sym.place.height() );

    out.append( (char*)&CharDesc.Page, 4 );
    out.append( (char*)&CharDesc.Width, 1 );
    out.append( (char*)&CharDesc.Height, 1 );
    out.append( (char*)&CharDesc.ShiftX, 4 );
    out.append( (char*)&CharDesc.ShiftY, 4 );
    out.append( (char*)&CharDesc.ShiftP, 4 );
    out.append( (char*)&CharDesc.TexCoords, sizeof( zglTPoint2D ) * 4 );
  }

  return true;
}


AbstractExporter* ZFIExporterFactoryFunc (QObject* parent) {
    return new ZFIExporter(parent);
}
