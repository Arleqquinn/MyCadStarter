#include "SceneTreeDock.h"
#include "../../core/project/ProjectModel.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QIcon>

SceneTreeDock::SceneTreeDock(QWidget* parent)
    : QDockWidget("Scene", parent)
{
    m_tree = new QTreeWidget(this);
    m_tree->setColumnCount(3);
    m_tree->setHeaderLabels({"Name", "Type", "ID"});
    m_tree->header()->setStretchLastSection(false);
    m_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    setWidget(m_tree);

    connect(m_tree, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem* item, int) {
        emit nodeActivated(item->data(0, Qt::UserRole).toInt());
    });
}

void SceneTreeDock::setModel(ProjectModel* model)
{
    m_model = model;
    if (!m_model) return;

    connect(m_model, &ProjectModel::modelChanged, this, [this]() { rebuild(); });
    connect(m_model, &ProjectModel::selectionChanged, this, [this](int id) {
        for (int i = 0; i < m_tree->topLevelItemCount(); ++i) {
            QTreeWidgetItem* item = m_tree->topLevelItem(i);
            if (item->data(0, Qt::UserRole).toInt() == id) {
                m_tree->setCurrentItem(item);
                return;
            }
        }
    });

    rebuild();
}

void SceneTreeDock::rebuild()
{
    m_tree->clear();
    if (!m_model) return;

    for (const auto& node : m_model->nodes()) {
        auto* item = new QTreeWidgetItem(m_tree);
        item->setText(0, node.name);
        item->setText(1, node.kind);
        item->setText(2, QString::number(node.id));
        item->setData(0, Qt::UserRole, node.id);
        item->setIcon(0, QIcon(node.nodeType == NodeType::Imported ? ":/icons/import.svg" : ":/icons/shape.svg"));
        m_tree->addTopLevelItem(item);
    }
}
