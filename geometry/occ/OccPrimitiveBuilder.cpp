#include "OccPrimitiveBuilder.h"

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepPrimAPI_MakeTorus.hxx>

TopoDS_Shape OccPrimitiveBuilder::makeBox(double dx, double dy, double dz)
{
    return BRepPrimAPI_MakeBox(dx, dy, dz).Shape();
}

TopoDS_Shape OccPrimitiveBuilder::makeCylinder(double radius, double height)
{
    return BRepPrimAPI_MakeCylinder(radius, height).Shape();
}

TopoDS_Shape OccPrimitiveBuilder::makeSphere(double radius)
{
    return BRepPrimAPI_MakeSphere(radius).Shape();
}

TopoDS_Shape OccPrimitiveBuilder::makeCone(double bottomRadius, double topRadius, double height)
{
    return BRepPrimAPI_MakeCone(bottomRadius, topRadius, height).Shape();
}

TopoDS_Shape OccPrimitiveBuilder::makeTorus(double majorRadius, double minorRadius)
{
    return BRepPrimAPI_MakeTorus(majorRadius, minorRadius).Shape();
}

TopoDS_Shape OccPrimitiveBuilder::build(const QString& kind, const QMap<QString, double>& params)
{
    if (kind == "Box") {
        return makeBox(params.value("Width (X)", 100.0), params.value("Depth (Y)", 60.0), params.value("Height (Z)", 40.0));
    }
    if (kind == "Cylinder") {
        return makeCylinder(params.value("Radius", 30.0), params.value("Height", 80.0));
    }
    if (kind == "Sphere") {
        return makeSphere(params.value("Radius", 50.0));
    }
    if (kind == "Cone") {
        return makeCone(params.value("Bottom Radius", 40.0), params.value("Top Radius", 10.0), params.value("Height", 70.0));
    }
    if (kind == "Torus") {
        return makeTorus(params.value("Major Radius", 50.0), params.value("Minor Radius", 15.0));
    }
    return TopoDS_Shape();
}

QString OccPrimitiveBuilder::nextDefaultName(const QString& kind, int id)
{
    return QString("%1_%2").arg(kind).arg(id);
}
