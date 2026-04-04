#include "ViewportWidget.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <Aspect_DisplayConnection.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <AIS_ViewCube.hxx>

#ifdef _WIN32
#include <WNT_Window.hxx>
#elif defined(__APPLE__)
#include <Cocoa_Window.hxx>
#else
#include <Xw_Window.hxx>
#endif

ViewportWidget::ViewportWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NativeWindow);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(500, 400);
}

ViewportWidget::~ViewportWidget() = default;

Handle(AIS_InteractiveContext) ViewportWidget::context() const
{
    return m_context;
}

void ViewportWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    if (!m_initialized) {
        initViewer();
        m_initialized = true;
        emit viewerReady();
    }
}

void ViewportWidget::initViewer()
{
    Handle(Aspect_DisplayConnection) displayConnection = new Aspect_DisplayConnection();
    Handle(OpenGl_GraphicDriver) driver = new OpenGl_GraphicDriver(displayConnection);
    m_viewer = new V3d_Viewer(driver);
    m_viewer->SetDefaultLights();
    m_viewer->SetLightOn();

    m_context = new AIS_InteractiveContext(m_viewer);
    m_view = m_viewer->CreateView();

#ifdef _WIN32
    Handle(WNT_Window) wind = new WNT_Window((Aspect_Handle)winId());
#elif defined(__APPLE__)
    Handle(Cocoa_Window) wind = new Cocoa_Window((NSView*)winId());
#else
    Handle(Xw_Window) wind = new Xw_Window(displayConnection, (Window)winId());
#endif

    m_view->SetWindow(wind);
    if (!wind->IsMapped()) wind->Map();

    m_view->SetBackgroundColor(Quantity_NOC_GRAY20);
    m_view->TriedronDisplay(Aspect_TOTP_LEFT_LOWER, Quantity_NOC_WHITE, 0.08, V3d_ZBUFFER);

    Handle(AIS_ViewCube) cube = new AIS_ViewCube();
    cube->SetSize(70.0);
    cube->SetBoxColor(Quantity_NOC_GRAY70);
    cube->SetTransparency(0.15f);
    m_context->Display(cube, Standard_False);

    m_view->MustBeResized();
    m_view->SetProj(V3d_XposYnegZpos);
    m_view->FitAll();
    m_view->Redraw();
}

void ViewportWidget::fitAll()
{
    if (!m_view.IsNull()) {
        m_view->FitAll();
        m_view->ZFitAll();
        m_view->Redraw();
    }
}

void ViewportWidget::viewIso()
{
    if (!m_view.IsNull()) {
        m_view->SetProj(V3d_XposYnegZpos);
        m_view->FitAll();
        m_view->Redraw();
    }
}

void ViewportWidget::viewFront()
{
    if (!m_view.IsNull()) {
        m_view->SetProj(V3d_Yneg);
        m_view->FitAll();
        m_view->Redraw();
    }
}

void ViewportWidget::viewRight()
{
    if (!m_view.IsNull()) {
        m_view->SetProj(V3d_Xpos);
        m_view->FitAll();
        m_view->Redraw();
    }
}

void ViewportWidget::viewTop()
{
    if (!m_view.IsNull()) {
        m_view->SetProj(V3d_Zpos);
        m_view->FitAll();
        m_view->Redraw();
    }
}

void ViewportWidget::paintEvent(QPaintEvent*)
{
    if (!m_view.IsNull()) m_view->Redraw();
}

void ViewportWidget::resizeEvent(QResizeEvent*)
{
    if (!m_view.IsNull()) m_view->MustBeResized();
}

void ViewportWidget::mousePressEvent(QMouseEvent* event)
{
    if (m_view.IsNull() || m_context.IsNull()) return;
    setFocus();
    m_lastPos = event->pos();

    if (event->button() == Qt::LeftButton && !m_spacePressed) {
        m_context->MoveTo(event->position().x(), event->position().y(), m_view, Standard_True);
        m_context->SelectDetected(AIS_SelectionScheme_Replace);
        emit selectionChanged();
    } else if (event->button() == Qt::RightButton) {
        m_view->StartRotation(event->position().x(), event->position().y());
    }
}

void ViewportWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_view.IsNull() || m_context.IsNull()) return;

    if ((event->buttons() & Qt::LeftButton) && m_spacePressed) {
        const QPoint delta = event->pos() - m_lastPos;
        const double factor = m_view->Scale() / 220.0;
        emit objectDragRequested(delta.x() * factor, -delta.y() * factor, 0.0);
    } else if (event->buttons() & Qt::RightButton) {
        m_view->Rotation(static_cast<int>(event->position().x()), static_cast<int>(event->position().y()));
    } else if (event->buttons() & Qt::MiddleButton) {
        m_view->Pan(static_cast<int>(event->position().x() - m_lastPos.x()),
                    static_cast<int>(m_lastPos.y() - event->position().y()));
    } else {
        m_context->MoveTo(static_cast<int>(event->position().x()),
                          static_cast<int>(event->position().y()),
                          m_view,
                          Standard_True);
    }

    m_lastPos = event->pos();
}

void ViewportWidget::wheelEvent(QWheelEvent* event)
{
    if (m_view.IsNull()) return;
    const Standard_Real factor = (event->angleDelta().y() > 0) ? 1.1 : 0.9;
    m_view->SetScale(m_view->Scale() / factor);
    m_view->Redraw();
}

void ViewportWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space) {
        m_spacePressed = true;
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void ViewportWidget::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space) {
        m_spacePressed = false;
        event->accept();
        return;
    }
    QWidget::keyReleaseEvent(event);
}
