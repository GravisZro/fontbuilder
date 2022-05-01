#ifndef JSONFONTEXPORTER_H
#define JSONFONTEXPORTER_H

#include "../abstractexporter.h"

class JSONFontExporter : public AbstractExporter
{
    Q_OBJECT
public:
    explicit JSONFontExporter(QObject *parent = nullptr);

    virtual bool Export(QByteArray& out);
signals:

public slots:

};

#endif // JSONFONTEXPORTER_H
