#pragma once

#include <QDockWidget>

class ProjectModel;
class QTreeWidget;
class QTreeWidgetItem;

class SceneTreeDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit SceneTreeDock(QWidget* parent = nullptr);
    void setModel(ProjectModel* model);

signals:
    void nodeActivated(int id);

private:
    void rebuild();

    ProjectModel* m_model = nullptr;
    QTreeWidget* m_tree = nullptr;
};
