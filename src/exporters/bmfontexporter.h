#ifndef BMFONTEXPORTER_H
#define BMFONTEXPORTER_H

#include <abstractexporter.h>

class BMFontExporter : public AbstractExporter
{
    Q_OBJECT
public:
    explicit BMFontExporter(QObject *parent = nullptr);

    virtual bool Export(QByteArray& out);
signals:

public slots:

};

#endif // BMFONTEXPORTER_H
