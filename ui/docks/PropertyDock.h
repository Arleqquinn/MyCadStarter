#pragma once

#include <QDockWidget>

class ProjectModel;
class QLabel;
class QPushButton;
class QCheckBox;
class QComboBox;

class PropertyDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit PropertyDock(QWidget* parent = nullptr);
    void setModel(ProjectModel* model);

public slots:
    void showNode(int id);

private:
    ProjectModel* m_model = nullptr;
    int m_currentId = -1;

    QLabel* m_nameLabel = nullptr;
    QLabel* m_typeLabel = nullptr;
    QLabel* m_posLabel = nullptr;
    QCheckBox* m_visibleCheck = nullptr;
    QPushButton* m_colorButton = nullptr;
    QComboBox* m_displayModeCombo = nullptr;
};
