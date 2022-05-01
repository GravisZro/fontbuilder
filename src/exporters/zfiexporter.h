#ifndef ZFIEXPORTER_H
#define ZFIEXPORTER_H

#include "../abstractexporter.h"

typedef struct
{
  float X;
  float Y;
} zglTPoint2D;

typedef struct
{
  uint16_t Page;
  uint8_t Width;
  uint8_t Height;
  int32_t ShiftX;
  int32_t ShiftY;
  int32_t ShiftP;
  zglTPoint2D TexCoords[4];
} zglTCharDesc;

class ZFIExporter : public AbstractExporter
{
Q_OBJECT
public:
    explicit ZFIExporter(QObject *parent = nullptr);
protected:
    virtual bool Export(QByteArray& out);
signals:

public slots:

};

#endif // ZFIEXPORTER_H
