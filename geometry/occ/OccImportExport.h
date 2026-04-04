#pragma once

#include <QString>
#include <TopoDS_Shape.hxx>

class OccImportExport
{
public:
    static TopoDS_Shape importFile(const QString& path);
    static bool exportStep(const TopoDS_Shape& shape, const QString& path);

    static QString importFilter();
    static QString exportFilter();
};
