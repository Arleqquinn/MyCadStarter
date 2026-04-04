#pragma once

#include <QWidget>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <V3d_View.hxx>

class ViewportWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ViewportWidget(QWidget* parent = nullptr);
    ~ViewportWidget() override;

    Handle(AIS_InteractiveContext) context() const;
    void fitAll();
    void viewIso();
    void viewFront();
    void viewRight();
    void viewTop();

    QPaintEngine* paintEngine() const override { return nullptr; }

signals:
    void selectionChanged();
    void viewerReady();
    void objectDragRequested(double dx, double dy, double dz);

protected:
    void paintEvent(QPaintEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void showEvent(QShowEvent*) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    void initViewer();

    Handle(V3d_Viewer) m_viewer;
    Handle(V3d_View) m_view;
    Handle(AIS_InteractiveContext) m_context;
    bool m_initialized = false;
    bool m_spacePressed = false;
    QPoint m_lastPos;
};
