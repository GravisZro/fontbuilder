#ifndef SIMPLEEXPORTER_H
#define SIMPLEEXPORTER_H

#include "../abstractexporter.h"

class SimpleExporter : public AbstractExporter
{
    Q_OBJECT
public:
    explicit SimpleExporter(QObject *parent = nullptr);

    virtual bool Export(QByteArray& out);
signals:

public slots:

};

#endif // SIMPLEEXPORTER_H
