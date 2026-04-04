#include "OccImportExport.h"

#include <STEPControl_Reader.hxx>
#include <STEPControl_Writer.hxx>
#include <IGESControl_Reader.hxx>
#include <BRepTools.hxx>
#include <BRep_Builder.hxx>
#include <QFileInfo>
#include <stdexcept>

TopoDS_Shape OccImportExport::importFile(const QString& path)
{
    const QString suffix = QFileInfo(path).suffix().toLower();

    if (suffix == "step" || suffix == "stp") {
        STEPControl_Reader reader;
        if (reader.ReadFile(path.toUtf8().constData()) != IFSelect_RetDone) {
            throw std::runtime_error("STEP import failed");
        }
        reader.TransferRoots();
        return reader.OneShape();
    }

    if (suffix == "iges" || suffix == "igs") {
        IGESControl_Reader reader;
        if (reader.ReadFile(path.toUtf8().constData()) != IFSelect_RetDone) {
            throw std::runtime_error("IGES import failed");
        }
        reader.TransferRoots();
        return reader.OneShape();
    }

    if (suffix == "brep" || suffix == "brp") {
        TopoDS_Shape shape;
        BRep_Builder builder;
        if (!BRepTools::Read(shape, path.toUtf8().constData(), builder)) {
            throw std::runtime_error("BREP import failed");
        }
        return shape;
    }

    throw std::runtime_error("Unsupported format");
}

bool OccImportExport::exportStep(const TopoDS_Shape& shape, const QString& path)
{
    if (shape.IsNull()) return false;
    STEPControl_Writer writer;
    if (writer.Transfer(shape, STEPControl_AsIs) != IFSelect_RetDone) {
        return false;
    }
    return writer.Write(path.toUtf8().constData()) == IFSelect_RetDone;
}

QString OccImportExport::importFilter()
{
    return "CAD Files (*.step *.stp *.iges *.igs *.brep *.brp);;All Files (*)";
}

QString OccImportExport::exportFilter()
{
    return "STEP Files (*.step *.stp)";
}
