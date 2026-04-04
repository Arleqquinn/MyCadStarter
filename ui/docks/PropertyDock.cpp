#include "PropertyDock.h"
#include "../../core/project/ProjectModel.h"
#include "../../core/utils/AppStrings.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QWidget>
#include <QColorDialog>
#include <QComboBox>
#include <QSignalBlocker>

PropertyDock::PropertyDock(QWidget* parent)
    : QDockWidget("Properties", parent)
{
    auto* body = new QWidget(this);
    auto* layout = new QVBoxLayout(body);

    m_nameLabel = new QLabel("-", body);
    m_typeLabel = new QLabel("-", body);
    m_posLabel = new QLabel("X=0.00, Y=0.00, Z=0.00", body);
    m_visibleCheck = new QCheckBox("Visible", body);
    m_colorButton = new QPushButton("Change color", body);

    m_displayModeCombo = new QComboBox(body);
    m_displayModeCombo->addItem("Wireframe");
    m_displayModeCombo->addItem("Shaded");

    layout->addWidget(new QLabel("Name:", body));
    layout->addWidget(m_nameLabel);
    layout->addWidget(new QLabel("Type:", body));
    layout->addWidget(m_typeLabel);
    layout->addWidget(new QLabel("Position:", body));
    layout->addWidget(m_posLabel);
    layout->addWidget(new QLabel("Display mode:", body));
    layout->addWidget(m_displayModeCombo);
    layout->addWidget(m_visibleCheck);
    layout->addWidget(m_colorButton);
    layout->addStretch(1);

    setWidget(body);

    connect(m_visibleCheck, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_model && m_currentId >= 0) {
            m_model->setNodeVisible(m_currentId, checked);
        }
    });

    connect(m_colorButton, &QPushButton::clicked, this, [this]() {
        if (!m_model || m_currentId < 0) return;

        const SceneNode* node = m_model->findNode(m_currentId);
        if (!node) return;

        const QColor color = QColorDialog::getColor(node->color, this, "Choose color");
        if (color.isValid()) {
            m_model->setNodeColor(m_currentId, color);
        }
    });

    connect(m_displayModeCombo, &QComboBox::currentIndexChanged, this, [this](int idx) {
        if (!m_model || m_currentId < 0) return;
        m_model->setNodeDisplayMode(m_currentId, idx == 0 ? DisplayMode::Wireframe : DisplayMode::Shaded);
    });
}

void PropertyDock::setModel(ProjectModel* model)
{
    m_model = model;
}

void PropertyDock::showNode(int id)
{
    m_currentId = id;

    QSignalBlocker visibleBlocker(m_visibleCheck);
    QSignalBlocker displayBlocker(m_displayModeCombo);

    if (!m_model || id < 0) {
        m_nameLabel->setText("-");
        m_typeLabel->setText("-");
        m_posLabel->setText("X=0.00, Y=0.00, Z=0.00");
        m_visibleCheck->setChecked(false);
        m_displayModeCombo->setCurrentIndex(1);
        return;
    }

    const SceneNode* node = m_model->findNode(id);
    if (!node) return;

    m_nameLabel->setText(node->name);
    m_typeLabel->setText(node->kind);
    m_posLabel->setText(AppStrings::formatVector3(node->posX, node->posY, node->posZ));
    m_visibleCheck->setChecked(node->visible);
    m_displayModeCombo->setCurrentIndex(node->displayMode == DisplayMode::Wireframe ? 0 : 1);
}
