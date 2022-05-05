#pragma once

#include <abstractexporter.h>

class AGEExporter : public AbstractExporter
{
Q_OBJECT

public:
    explicit AGEExporter(QObject* parent = nullptr);

protected:
    virtual bool Export(QByteArray& out);

private:
    static bool sortSymbols(const Symbol& a, const Symbol& b);

signals:
public slots:

};

