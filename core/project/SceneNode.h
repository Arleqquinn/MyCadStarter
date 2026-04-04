#pragma once

#include <QString>
#include <QColor>
#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

enum class NodeType {
    Primitive,
    Imported
};

enum class DisplayMode {
    Wireframe = 0,
    Shaded = 1
};

/**
 * @brief In-memory representation of a CAD object shown in the scene tree and viewport.
 */
struct SceneNode {
    int id = -1;
    QString name;
    QString kind;
    NodeType nodeType = NodeType::Primitive;
    QColor color = QColor(180, 180, 220);
    bool visible = true;
    DisplayMode displayMode = DisplayMode::Shaded;
    TopoDS_Shape shape;
    Handle(AIS_Shape) aisShape;
    double posX = 0.0;
    double posY = 0.0;
    double posZ = 0.0;
};
