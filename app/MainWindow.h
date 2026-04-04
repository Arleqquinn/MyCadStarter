#pragma once

#include <QMainWindow>

class ViewportWidget;
class SceneTreeDock;
class PropertyDock;
class ProjectModel;
class CommandManager;
class QAction;
class QDockWidget;
class QListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void onNewProject();
    void onImport();
    void onExportSelected();

    void onCreateBox();
    void onCreateCylinder();
    void onCreateSphere();
    void onCreateCone();
    void onCreateTorus();

    void onDeleteSelected();
    void onDuplicateSelected();
    void onTranslateSelected();
    void onBooleanCut();
    void onBooleanFuse();
    void onBooleanCommon();
    void onViewIso();
    void onViewFront();
    void onViewRight();
    void onViewTop();
    void onViewportObjectDrag(double dx, double dy, double dz);
    void onFitAll();
    void onToggleLog();

    void onTreeActivated(int id);
    void onViewportSelectionChanged();

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createDocks();
    void applyTheme();
    void createPrimitive(const QString& kind);
    int selectedIdOrWarn();

    ViewportWidget* m_viewport = nullptr;
    SceneTreeDock* m_sceneDock = nullptr;
    PropertyDock* m_propertyDock = nullptr;
    QDockWidget* m_logDock = nullptr;
    QListWidget* m_logList = nullptr;

    ProjectModel* m_model = nullptr;
    CommandManager* m_commands = nullptr;

    QAction* m_actNew = nullptr;
    QAction* m_actImport = nullptr;
    QAction* m_actExport = nullptr;
    QAction* m_actExit = nullptr;

    QAction* m_actBox = nullptr;
    QAction* m_actCylinder = nullptr;
    QAction* m_actSphere = nullptr;
    QAction* m_actCone = nullptr;
    QAction* m_actTorus = nullptr;

    QAction* m_actDelete = nullptr;
    QAction* m_actDuplicate = nullptr;
    QAction* m_actTranslate = nullptr;
    QAction* m_actCut = nullptr;
    QAction* m_actFuse = nullptr;
    QAction* m_actCommon = nullptr;
    QAction* m_actViewIso = nullptr;
    QAction* m_actViewFront = nullptr;
    QAction* m_actViewRight = nullptr;
    QAction* m_actViewTop = nullptr;
    QAction* m_actFitAll = nullptr;
    QAction* m_actToggleLog = nullptr;
};
