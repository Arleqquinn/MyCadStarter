#pragma once

#include <QString>
#include <QMap>
#include <TopoDS_Shape.hxx>

class OccPrimitiveBuilder
{
public:
    static TopoDS_Shape makeBox(double dx = 100.0, double dy = 80.0, double dz = 60.0);
    static TopoDS_Shape makeCylinder(double radius = 30.0, double height = 100.0);
    static TopoDS_Shape makeSphere(double radius = 50.0);
    static TopoDS_Shape makeCone(double bottomRadius = 40.0, double topRadius = 10.0, double height = 70.0);
    static TopoDS_Shape makeTorus(double majorRadius = 50.0, double minorRadius = 15.0);
    static TopoDS_Shape build(const QString& kind, const QMap<QString, double>& params);
    static QString nextDefaultName(const QString& kind, int id);
};
