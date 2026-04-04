#include "ProjectModel.h"

#include "../utils/AppStrings.h"

#include <AIS_InteractiveObject.hxx>
#include <Quantity_Color.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

ProjectModel::ProjectModel(const Handle(AIS_InteractiveContext)& context, QObject* parent)
    : QObject(parent), m_context(context)
{
}

const QList<SceneNode>& ProjectModel::nodes() const
{
    return m_nodes;
}

SceneNode* ProjectModel::findNode(int id)
{
    for (auto& node : m_nodes) {
        if (node.id == id) return &node;
    }
    return nullptr;
}

const SceneNode* ProjectModel::findNode(int id) const
{
    for (const auto& node : m_nodes) {
        if (node.id == id) return &node;
    }
    return nullptr;
}

int ProjectModel::addNode(const QString& name, const QString& kind, NodeType nodeType, const TopoDS_Shape& shape)
{
    if (shape.IsNull() || m_context.IsNull()) return -1;

    SceneNode node;
    node.id = m_nextId++;
    node.name = name;
    node.kind = kind;
    node.nodeType = nodeType;
    node.shape = shape;
    node.aisShape = new AIS_Shape(shape);

    const Quantity_Color occColor(node.color.redF(), node.color.greenF(), node.color.blueF(), Quantity_TOC_RGB);
    m_context->SetColor(node.aisShape, occColor, Standard_False);
    m_context->SetDisplayMode(node.aisShape, 1, Standard_False);
    m_context->Display(node.aisShape, Standard_False);

    m_nodes.push_back(node);

    m_context->UpdateCurrentViewer();
    emit modelChanged();
    return node.id;
}

void ProjectModel::removeNode(int id)
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].id == id) {
            if (!m_nodes[i].aisShape.IsNull() && !m_context.IsNull()) {
                m_context->Remove(m_nodes[i].aisShape, Standard_False);
                m_context->UpdateCurrentViewer();
            }
            m_nodes.removeAt(i);
            emit modelChanged();
            emit selectionChanged(-1);
            return;
        }
    }
}

void ProjectModel::clear()
{
    if (!m_context.IsNull()) {
        for (const auto& node : m_nodes) {
            if (!node.aisShape.IsNull()) {
                m_context->Remove(node.aisShape, Standard_False);
            }
        }
        m_context->ClearSelected(Standard_False);
        m_context->UpdateCurrentViewer();
    }

    m_nodes.clear();
    m_nextId = 1;

    emit modelChanged();
    emit selectionChanged(-1);
}

int ProjectModel::selectedNodeId() const
{
    const auto ids = selectedNodeIds();
    return ids.isEmpty() ? -1 : ids.first();
}

QList<int> ProjectModel::selectedNodeIds() const
{
    QList<int> ids;
    if (m_context.IsNull()) return ids;

    for (m_context->InitSelected(); m_context->MoreSelected(); m_context->NextSelected()) {
        Handle(AIS_InteractiveObject) obj = m_context->SelectedInteractive();
        for (const auto& node : m_nodes) {
            if (node.aisShape == obj) {
                ids.push_back(node.id);
                break;
            }
        }
    }

    return ids;
}

void ProjectModel::syncSelectionFromContext()
{
    emit selectionChanged(selectedNodeId());
}

void ProjectModel::selectNodeInContext(int id)
{
    if (m_context.IsNull()) return;

    m_context->ClearSelected(Standard_False);

    const SceneNode* node = findNode(id);
    if (node && !node->aisShape.IsNull()) {
        m_context->AddOrRemoveSelected(node->aisShape, Standard_False);
    }

    m_context->UpdateCurrentViewer();
    emit selectionChanged(selectedNodeId());
}

bool ProjectModel::setNodeColor(int id, const QColor& color)
{
    SceneNode* node = findNode(id);
    if (!node || m_context.IsNull() || node->aisShape.IsNull()) return false;

    node->color = color;
    const Quantity_Color occColor(color.redF(), color.greenF(), color.blueF(), Quantity_TOC_RGB);
    m_context->SetColor(node->aisShape, occColor, Standard_False);
    m_context->UpdateCurrentViewer();

    emit modelChanged();
    return true;
}

bool ProjectModel::setNodeVisible(int id, bool visible)
{
    SceneNode* node = findNode(id);
    if (!node || m_context.IsNull() || node->aisShape.IsNull()) return false;

    node->visible = visible;
    if (visible) {
        m_context->Display(node->aisShape, Standard_False);
    } else {
        m_context->Erase(node->aisShape, Standard_False);
    }

    m_context->UpdateCurrentViewer();
    emit modelChanged();
    return true;
}

bool ProjectModel::setNodeDisplayMode(int id, DisplayMode mode)
{
    SceneNode* node = findNode(id);
    if (!node || m_context.IsNull() || node->aisShape.IsNull()) return false;

    node->displayMode = mode;
    m_context->SetDisplayMode(node->aisShape,
                              mode == DisplayMode::Wireframe ? 0 : 1,
                              Standard_False);
    m_context->UpdateCurrentViewer();

    emit modelChanged();
    return true;
}

bool ProjectModel::translateNode(int id, double dx, double dy, double dz)
{
    SceneNode* node = findNode(id);
    if (!node || node->shape.IsNull() || m_context.IsNull()) return false;

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(dx, dy, dz));

    BRepBuilderAPI_Transform builder(node->shape, trsf, Standard_True);
    if (!builder.IsDone()) return false;

    if (!node->aisShape.IsNull()) {
        m_context->Remove(node->aisShape, Standard_False);
    }

    node->shape = builder.Shape();
    node->aisShape = new AIS_Shape(node->shape);
    node->posX += dx;
    node->posY += dy;
    node->posZ += dz;

    const Quantity_Color occColor(node->color.redF(), node->color.greenF(), node->color.blueF(), Quantity_TOC_RGB);
    m_context->SetColor(node->aisShape, occColor, Standard_False);
    m_context->SetDisplayMode(node->aisShape,
                              node->displayMode == DisplayMode::Wireframe ? 0 : 1,
                              Standard_False);

    if (node->visible) {
        m_context->Display(node->aisShape, Standard_False);
    }

    m_context->ClearSelected(Standard_False);
    m_context->AddOrRemoveSelected(node->aisShape, Standard_False);
    m_context->UpdateCurrentViewer();
    emit modelChanged();
    emit selectionChanged(id);
    return true;
}

int ProjectModel::duplicateNode(int id)
{
    SceneNode* node = findNode(id);
    if (!node || node->shape.IsNull()) return -1;

    gp_Trsf trsf;
    trsf.SetTranslation(gp_Vec(20.0, 0.0, 0.0));

    BRepBuilderAPI_Transform builder(node->shape, trsf, Standard_True);
    if (!builder.IsDone()) return -1;

    const int newId = addNode(AppStrings::makeDuplicateName(node->name), node->kind, node->nodeType, builder.Shape());

    SceneNode* newNode = findNode(newId);
    if (newNode) {
        newNode->color = node->color;
        newNode->displayMode = node->displayMode;
        newNode->posX = node->posX + 20.0;
        newNode->posY = node->posY;
        newNode->posZ = node->posZ;

        const Quantity_Color occColor(newNode->color.redF(), newNode->color.greenF(), newNode->color.blueF(), Quantity_TOC_RGB);
        m_context->SetColor(newNode->aisShape, occColor, Standard_False);
        m_context->SetDisplayMode(newNode->aisShape,
                                  newNode->displayMode == DisplayMode::Wireframe ? 0 : 1,
                                  Standard_False);
        m_context->UpdateCurrentViewer();
    }

    return newId;
}

int ProjectModel::booleanCut(int baseId, int toolId)
{
    if (baseId == toolId) return -1;

    SceneNode* baseNode = findNode(baseId);
    SceneNode* toolNode = findNode(toolId);

    if (!baseNode || !toolNode) return -1;
    if (baseNode->shape.IsNull() || toolNode->shape.IsNull()) return -1;

    BRepAlgoAPI_Cut cutter(baseNode->shape, toolNode->shape);
    cutter.Build();

    if (!cutter.IsDone()) return -1;

    const TopoDS_Shape result = cutter.Shape();
    if (result.IsNull()) return -1;

    removeNode(baseId);
    removeNode(toolId);

    return addNode(AppStrings::makeDefaultObjectName("CutResult", m_nextId), "Cut", NodeType::Primitive, result);
}

int ProjectModel::booleanFuse(int firstId, int secondId)
{
    if (firstId == secondId) return -1;

    SceneNode* firstNode = findNode(firstId);
    SceneNode* secondNode = findNode(secondId);
    if (!firstNode || !secondNode) return -1;
    if (firstNode->shape.IsNull() || secondNode->shape.IsNull()) return -1;

    BRepAlgoAPI_Fuse fuse(firstNode->shape, secondNode->shape);
    fuse.Build();
    if (!fuse.IsDone()) return -1;

    const TopoDS_Shape result = fuse.Shape();
    if (result.IsNull()) return -1;

    removeNode(firstId);
    removeNode(secondId);
    return addNode(AppStrings::makeDefaultObjectName("FuseResult", m_nextId), "Fuse", NodeType::Primitive, result);
}

int ProjectModel::booleanCommon(int firstId, int secondId)
{
    if (firstId == secondId) return -1;

    SceneNode* firstNode = findNode(firstId);
    SceneNode* secondNode = findNode(secondId);
    if (!firstNode || !secondNode) return -1;
    if (firstNode->shape.IsNull() || secondNode->shape.IsNull()) return -1;

    BRepAlgoAPI_Common common(firstNode->shape, secondNode->shape);
    common.Build();
    if (!common.IsDone()) return -1;

    const TopoDS_Shape result = common.Shape();
    if (result.IsNull()) return -1;

    removeNode(firstId);
    removeNode(secondId);
    return addNode(AppStrings::makeDefaultObjectName("CommonResult", m_nextId), "Common", NodeType::Primitive, result);
}
