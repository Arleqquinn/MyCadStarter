#include "MainWindow.h"

#include "../core/project/ProjectModel.h"
#include "../core/commands/CommandManager.h"
#include "../core/utils/AppStrings.h"
#include "../geometry/occ/OccPrimitiveBuilder.h"
#include "../geometry/occ/OccImportExport.h"
#include "../ui/viewport/ViewportWidget.h"
#include "../ui/docks/SceneTreeDock.h"
#include "../ui/docks/PropertyDock.h"

#include <QToolBar>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QDockWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QInputDialog>
#include <QAction>
#include <QIcon>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("MyCadStarter");
    resize(1360, 840);

    m_viewport = new ViewportWidget(this);
    setCentralWidget(m_viewport);

    m_commands = new CommandManager(this);

    createActions();
    createMenus();
    createToolBar();
    createDocks();
    applyTheme();

    connect(m_viewport, &ViewportWidget::viewerReady, this, [this]() {
        if (m_model) return;

        m_model = new ProjectModel(m_viewport->context(), this);
        m_sceneDock->setModel(m_model);
        m_propertyDock->setModel(m_model);

        connect(m_sceneDock, &SceneTreeDock::nodeActivated, this, &MainWindow::onTreeActivated);
        connect(m_viewport, &ViewportWidget::selectionChanged, this, &MainWindow::onViewportSelectionChanged);
        connect(m_viewport, &ViewportWidget::objectDragRequested, this, &MainWindow::onViewportObjectDrag);
        connect(m_model, &ProjectModel::selectionChanged, m_propertyDock, &PropertyDock::showNode);
    });

    connect(m_commands, &CommandManager::historyChanged, this, [this]() {
        m_logList->clear();
        m_logList->addItems(m_commands->history());
        if (m_logList->count() > 0) m_logList->scrollToBottom();
    });

    statusBar()->showMessage("Ready. Space + left mouse drags selected object in XY plane.");
}

void MainWindow::applyTheme()
{
    setStyleSheet(R"(
        QMainWindow, QDockWidget, QListWidget, QTreeWidget, QWidget { background: #1f2430; color: #e6e9ef; }
        QMenuBar, QMenu, QToolBar, QStatusBar { background: #232a36; color: #e6e9ef; }
        QToolBar { spacing: 6px; padding: 4px; border-bottom: 1px solid #303848; }
        QPushButton, QComboBox, QLineEdit { background: #2a3140; color: #e6e9ef; border: 1px solid #4d5a70; border-radius: 4px; padding: 4px 8px; }
        QTreeWidget, QListWidget { border: 1px solid #303848; alternate-background-color: #252c3a; }
        QHeaderView::section { background: #2a3140; color: #e6e9ef; padding: 4px; border: none; }
        QDockWidget::title { background: #232a36; padding-left: 8px; }
    )");
}

void MainWindow::createActions()
{
    m_actNew = new QAction(QIcon(":/icons/new.svg"), "New", this);
    m_actImport = new QAction(QIcon(":/icons/import.svg"), "Import...", this);
    m_actExport = new QAction(QIcon(":/icons/export.svg"), "Export STEP...", this);
    m_actExit = new QAction("Exit", this);

    m_actBox = new QAction(QIcon(":/icons/box.svg"), "Box", this);
    m_actCylinder = new QAction(QIcon(":/icons/cylinder.svg"), "Cylinder", this);
    m_actSphere = new QAction(QIcon(":/icons/sphere.svg"), "Sphere", this);
    m_actCone = new QAction(QIcon(":/icons/cone.svg"), "Cone", this);
    m_actTorus = new QAction(QIcon(":/icons/torus.svg"), "Torus", this);

    m_actDelete = new QAction(QIcon(":/icons/delete.svg"), "Delete", this);
    m_actDuplicate = new QAction(QIcon(":/icons/duplicate.svg"), "Duplicate", this);
    m_actTranslate = new QAction(QIcon(":/icons/move.svg"), "Move", this);
    m_actCut = new QAction(QIcon(":/icons/cut.svg"), "Cut", this);
    m_actFuse = new QAction(QIcon(":/icons/fuse.svg"), "Fuse", this);
    m_actCommon = new QAction(QIcon(":/icons/common.svg"), "Common", this);
    m_actViewIso = new QAction(QIcon(":/icons/view_iso.svg"), "Isometric", this);
    m_actViewFront = new QAction(QIcon(":/icons/front.svg"), "Front", this);
    m_actViewRight = new QAction(QIcon(":/icons/right.svg"), "Right", this);
    m_actViewTop = new QAction(QIcon(":/icons/top.svg"), "Top", this);
    m_actFitAll = new QAction(QIcon(":/icons/fit.svg"), "Fit All", this);
    m_actToggleLog = new QAction(QIcon(":/icons/log.svg"), "Show/Hide Log", this);

    connect(m_actNew, &QAction::triggered, this, &MainWindow::onNewProject);
    connect(m_actImport, &QAction::triggered, this, &MainWindow::onImport);
    connect(m_actExport, &QAction::triggered, this, &MainWindow::onExportSelected);
    connect(m_actExit, &QAction::triggered, this, &QWidget::close);

    connect(m_actBox, &QAction::triggered, this, &MainWindow::onCreateBox);
    connect(m_actCylinder, &QAction::triggered, this, &MainWindow::onCreateCylinder);
    connect(m_actSphere, &QAction::triggered, this, &MainWindow::onCreateSphere);
    connect(m_actCone, &QAction::triggered, this, &MainWindow::onCreateCone);
    connect(m_actTorus, &QAction::triggered, this, &MainWindow::onCreateTorus);

    connect(m_actDelete, &QAction::triggered, this, &MainWindow::onDeleteSelected);
    connect(m_actDuplicate, &QAction::triggered, this, &MainWindow::onDuplicateSelected);
    connect(m_actTranslate, &QAction::triggered, this, &MainWindow::onTranslateSelected);
    connect(m_actCut, &QAction::triggered, this, &MainWindow::onBooleanCut);
    connect(m_actFuse, &QAction::triggered, this, &MainWindow::onBooleanFuse);
    connect(m_actCommon, &QAction::triggered, this, &MainWindow::onBooleanCommon);
    connect(m_actViewIso, &QAction::triggered, this, &MainWindow::onViewIso);
    connect(m_actViewFront, &QAction::triggered, this, &MainWindow::onViewFront);
    connect(m_actViewRight, &QAction::triggered, this, &MainWindow::onViewRight);
    connect(m_actViewTop, &QAction::triggered, this, &MainWindow::onViewTop);
    connect(m_actFitAll, &QAction::triggered, this, &MainWindow::onFitAll);
    connect(m_actToggleLog, &QAction::triggered, this, &MainWindow::onToggleLog);
}

void MainWindow::createMenus()
{
    auto* fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(m_actNew);
    fileMenu->addAction(m_actImport);
    fileMenu->addAction(m_actExport);
    fileMenu->addSeparator();
    fileMenu->addAction(m_actExit);

    auto* createMenu = menuBar()->addMenu("Create");
    createMenu->addAction(m_actBox);
    createMenu->addAction(m_actCylinder);
    createMenu->addAction(m_actSphere);
    createMenu->addAction(m_actCone);
    createMenu->addAction(m_actTorus);

    auto* editMenu = menuBar()->addMenu("Edit");
    editMenu->addAction(m_actTranslate);
    editMenu->addAction(m_actDuplicate);
    editMenu->addAction(m_actDelete);

    auto* booleanMenu = menuBar()->addMenu("Boolean");
    booleanMenu->addAction(m_actCut);
    booleanMenu->addAction(m_actFuse);
    booleanMenu->addAction(m_actCommon);

    auto* viewMenu = menuBar()->addMenu("View");
    viewMenu->addAction(m_actViewIso);
    viewMenu->addAction(m_actViewFront);
    viewMenu->addAction(m_actViewRight);
    viewMenu->addAction(m_actViewTop);
    viewMenu->addAction(m_actFitAll);
    viewMenu->addAction(m_actToggleLog);
}

void MainWindow::createToolBar()
{
    auto* toolbar = addToolBar("Main");
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->addAction(m_actNew);
    toolbar->addAction(m_actImport);
    toolbar->addAction(m_actExport);
    toolbar->addSeparator();
    toolbar->addAction(m_actBox);
    toolbar->addAction(m_actCylinder);
    toolbar->addAction(m_actSphere);
    toolbar->addAction(m_actCone);
    toolbar->addAction(m_actTorus);
    toolbar->addSeparator();
    toolbar->addAction(m_actTranslate);
    toolbar->addAction(m_actDuplicate);
    toolbar->addAction(m_actCut);
    toolbar->addAction(m_actFuse);
    toolbar->addAction(m_actCommon);
    toolbar->addAction(m_actDelete);
    toolbar->addSeparator();
    toolbar->addAction(m_actViewIso);
    toolbar->addAction(m_actViewFront);
    toolbar->addAction(m_actViewRight);
    toolbar->addAction(m_actViewTop);
    toolbar->addAction(m_actFitAll);
    toolbar->addAction(m_actToggleLog);
}

void MainWindow::createDocks()
{
    m_sceneDock = new SceneTreeDock(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_sceneDock);

    m_propertyDock = new PropertyDock(this);
    addDockWidget(Qt::RightDockWidgetArea, m_propertyDock);

    m_logDock = new QDockWidget("Log", this);
    m_logList = new QListWidget(m_logDock);
    m_logList->setAlternatingRowColors(true);
    m_logDock->setWidget(m_logList);
    addDockWidget(Qt::BottomDockWidgetArea, m_logDock);
}

int MainWindow::selectedIdOrWarn()
{
    if (!m_model) return -1;
    const int id = m_model->selectedNodeId();
    if (id < 0) QMessageBox::information(this, "Action", "Select an object first.");
    return id;
}

void MainWindow::createPrimitive(const QString& kind)
{
    if (!m_model) return;

    TopoDS_Shape shape;
    if (kind == "Box") shape = OccPrimitiveBuilder::makeBox();
    else if (kind == "Cylinder") shape = OccPrimitiveBuilder::makeCylinder();
    else if (kind == "Sphere") shape = OccPrimitiveBuilder::makeSphere();
    else if (kind == "Cone") shape = OccPrimitiveBuilder::makeCone();
    else if (kind == "Torus") shape = OccPrimitiveBuilder::makeTorus();

    if (shape.IsNull()) return;

    const QString name = AppStrings::makeDefaultObjectName(kind, m_model->nodes().size() + 1);
    m_model->addNode(name, kind, NodeType::Primitive, shape);
    m_viewport->fitAll();
    m_commands->log(QString("Created %1").arg(kind));
    statusBar()->showMessage(QString("%1 created").arg(kind), 2500);
}

void MainWindow::onNewProject()
{
    if (!m_model) return;
    m_model->clear();
    m_commands->log("New project");
    statusBar()->showMessage("Project cleared", 2500);
}
void MainWindow::onCreateBox(){ createPrimitive("Box"); }
void MainWindow::onCreateCylinder(){ createPrimitive("Cylinder"); }
void MainWindow::onCreateSphere(){ createPrimitive("Sphere"); }
void MainWindow::onCreateCone(){ createPrimitive("Cone"); }
void MainWindow::onCreateTorus(){ createPrimitive("Torus"); }

void MainWindow::onImport()
{
    if (!m_model) return;

    const QString path = QFileDialog::getOpenFileName(this, "Import CAD", {}, OccImportExport::importFilter());
    if (path.isEmpty()) return;

    try {
        const TopoDS_Shape shape = OccImportExport::importFile(path);
        const QString name = QFileInfo(path).completeBaseName();
        m_model->addNode(name, "Imported", NodeType::Imported, shape);
        m_viewport->fitAll();
        m_commands->log(QString("Imported: %1").arg(path));
        statusBar()->showMessage("Import completed", 2500);
    } catch (const std::exception& ex) {
        QMessageBox::critical(this, "Import error", ex.what());
    }
}

void MainWindow::onExportSelected()
{
    if (!m_model) return;
    const int id = selectedIdOrWarn();
    if (id < 0) return;
    const SceneNode* node = m_model->findNode(id);
    if (!node) return;

    QString path = QFileDialog::getSaveFileName(this, "Export STEP", {}, OccImportExport::exportFilter());
    if (path.isEmpty()) return;
    if (!path.endsWith(".step", Qt::CaseInsensitive) && !path.endsWith(".stp", Qt::CaseInsensitive)) path += ".step";

    if (!OccImportExport::exportStep(node->shape, path)) {
        QMessageBox::critical(this, "Export error", "Failed to write STEP file.");
        return;
    }

    m_commands->log(QString("Exported STEP: %1").arg(path));
    statusBar()->showMessage("Export completed", 2500);
}

void MainWindow::onDeleteSelected()
{
    if (!m_model) return;
    const int id = selectedIdOrWarn();
    if (id < 0) return;
    m_model->removeNode(id);
    m_commands->log(QString("Deleted node id=%1").arg(id));
    statusBar()->showMessage("Object deleted", 2500);
}

void MainWindow::onDuplicateSelected()
{
    if (!m_model) return;
    const int id = selectedIdOrWarn();
    if (id < 0) return;
    if (m_model->duplicateNode(id) >= 0) {
        m_commands->log(QString("Duplicated node id=%1").arg(id));
        statusBar()->showMessage("Object duplicated", 2500);
        m_viewport->fitAll();
    }
}

void MainWindow::onTranslateSelected()
{
    if (!m_model) return;
    const int id = selectedIdOrWarn();
    if (id < 0) return;

    bool okX = false, okY = false, okZ = false;
    const double dx = QInputDialog::getDouble(this, "Move object", "dx:", 20.0, -100000.0, 100000.0, 2, &okX);
    if (!okX) return;
    const double dy = QInputDialog::getDouble(this, "Move object", "dy:", 0.0, -100000.0, 100000.0, 2, &okY);
    if (!okY) return;
    const double dz = QInputDialog::getDouble(this, "Move object", "dz:", 0.0, -100000.0, 100000.0, 2, &okZ);
    if (!okZ) return;

    if (m_model->translateNode(id, dx, dy, dz)) {
        m_commands->log(QString("Moved node id=%1 by %2").arg(id).arg(AppStrings::formatVector3(dx, dy, dz)));
        statusBar()->showMessage("Object moved", 2500);
        m_propertyDock->showNode(id);
        m_viewport->fitAll();
    }
}

void MainWindow::onBooleanCut()
{
    if (!m_model) return;
    const int baseId = selectedIdOrWarn();
    if (baseId < 0) return;

    bool ok = false;
    const int toolId = QInputDialog::getInt(this, "Cut", "Enter ID of shape to subtract:", 1, 1, 100000, 1, &ok);
    if (!ok) return;

    const int resultId = m_model->booleanCut(baseId, toolId);
    if (resultId < 0) {
        QMessageBox::warning(this, "Cut", "Boolean cut failed.");
        return;
    }

    m_commands->log(QString("Cut: %1 - %2 -> %3").arg(baseId).arg(toolId).arg(resultId));
    statusBar()->showMessage("Boolean cut completed", 2500);
    m_viewport->fitAll();
}

void MainWindow::onBooleanFuse()
{
    if (!m_model) return;
    const int firstId = selectedIdOrWarn();
    if (firstId < 0) return;

    bool ok = false;
    const int secondId = QInputDialog::getInt(this, "Fuse", "Enter ID of second shape:", 1, 1, 100000, 1, &ok);
    if (!ok) return;

    const int resultId = m_model->booleanFuse(firstId, secondId);
    if (resultId < 0) {
        QMessageBox::warning(this, "Fuse", "Boolean fuse failed.");
        return;
    }

    m_commands->log(QString("Fuse: %1 + %2 -> %3").arg(firstId).arg(secondId).arg(resultId));
    statusBar()->showMessage("Boolean fuse completed", 2500);
    m_viewport->fitAll();
}

void MainWindow::onBooleanCommon()
{
    if (!m_model) return;
    const int firstId = selectedIdOrWarn();
    if (firstId < 0) return;

    bool ok = false;
    const int secondId = QInputDialog::getInt(this, "Common", "Enter ID of second shape:", 1, 1, 100000, 1, &ok);
    if (!ok) return;

    const int resultId = m_model->booleanCommon(firstId, secondId);
    if (resultId < 0) {
        QMessageBox::warning(this, "Common", "Boolean common failed.");
        return;
    }

    m_commands->log(QString("Common: %1 & %2 -> %3").arg(firstId).arg(secondId).arg(resultId));
    statusBar()->showMessage("Boolean common completed", 2500);
    m_viewport->fitAll();
}

void MainWindow::onViewIso()
{
    m_viewport->viewIso();
    statusBar()->showMessage("Isometric view", 2000);
}

void MainWindow::onViewFront()
{
    m_viewport->viewFront();
    statusBar()->showMessage("Front view", 2000);
}

void MainWindow::onViewRight()
{
    m_viewport->viewRight();
    statusBar()->showMessage("Right view", 2000);
}

void MainWindow::onViewTop()
{
    m_viewport->viewTop();
    statusBar()->showMessage("Top view", 2000);
}

void MainWindow::onViewportObjectDrag(double dx, double dy, double dz)
{
    if (!m_model) return;
    const int id = m_model->selectedNodeId();
    if (id < 0) return;

    if (m_model->translateNode(id, dx, dy, dz)) {
        m_propertyDock->showNode(id);
    }
}

void MainWindow::onFitAll()
{
    m_viewport->fitAll();
    statusBar()->showMessage("Fit all", 2000);
}

void MainWindow::onToggleLog()
{
    m_logDock->setVisible(!m_logDock->isVisible());
}

void MainWindow::onTreeActivated(int id)
{
    if (m_model) m_model->selectNodeInContext(id);
}

void MainWindow::onViewportSelectionChanged()
{
    if (m_model) m_model->syncSelectionFromContext();
}
