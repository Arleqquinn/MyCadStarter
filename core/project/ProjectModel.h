#pragma once

#include "SceneNode.h"

#include <QObject>
#include <QList>
#include <AIS_InteractiveContext.hxx>

/**
 * @brief Stores scene entities and synchronises them with the OpenCascade interactive context.
 */
class ProjectModel : public QObject
{
    Q_OBJECT
public:
    explicit ProjectModel(const Handle(AIS_InteractiveContext)& context, QObject* parent = nullptr);

    const QList<SceneNode>& nodes() const;
    SceneNode* findNode(int id);
    const SceneNode* findNode(int id) const;

    int addNode(const QString& name, const QString& kind, NodeType nodeType, const TopoDS_Shape& shape);
    void removeNode(int id);
    void clear();

    int selectedNodeId() const;
    QList<int> selectedNodeIds() const;
    void syncSelectionFromContext();
    void selectNodeInContext(int id);

    bool setNodeColor(int id, const QColor& color);
    bool setNodeVisible(int id, bool visible);
    bool setNodeDisplayMode(int id, DisplayMode mode);

    bool translateNode(int id, double dx, double dy, double dz);
    int duplicateNode(int id);
    int booleanCut(int baseId, int toolId);
    int booleanFuse(int firstId, int secondId);
    int booleanCommon(int firstId, int secondId);

signals:
    void modelChanged();
    void selectionChanged(int id);

private:
    Handle(AIS_InteractiveContext) m_context;
    QList<SceneNode> m_nodes;
    int m_nextId = 1;
};
