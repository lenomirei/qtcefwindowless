#include "cefwidget.h"

#include <QBitmap>
#include <QDatetime>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QWheelEvent>
#include <QWindow>

CefWidget::CefWidget(QWidget *parent)
    : QWidget{parent}
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

CefWidget::~CefWidget()
{
    if (client_)
    {
        client_->CloseBrowser();
        client_ = nullptr;
    }
}

void CefWidget::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    QImage frame = QImage(static_cast<const uchar*>(buffer), width, height, QImage::Format_ARGB32_Premultiplied);
    frame.setDevicePixelRatio(ratio_);
    // 这里单独改用一个结构体存储width和height是因为cef计算出来的width和height的取整方式可能和qt int计算浮点数后存放到int里面的取整方式不同，导致paint时有一丢丢不同会出现斜着的图片
    QPixmap pixmap = QPixmap::fromImage(frame);
    QMetaObject::invokeMethod(this, [this, pm = std::move(pixmap)](){
        pixmap_ = std::move(pm);
    });
    update();
}

void CefWidget::showEvent(QShowEvent* event)
{
    CreateBrowser();
}

void CefWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawPixmap(QRect(QPoint(0,0), geometry().size()), pixmap_);

//    QString filename = QString("D:/work/testimage/lalala_%1").arg(QDateTime::currentMSecsSinceEpoch());
//    frame.save(filename, "PNG");
//    painter.drawImage(QRect(QPoint(0,0), QSize(frame.width(), frame.height())), frame);
}

void CefWidget::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    const QRect& q_rect = this->geometry();
    rect.x = q_rect.x();
    rect.y = q_rect.y();
    rect.width = q_rect.width();
    rect.height = q_rect.height();
}

void CefWidget::mouseDoubleClickEvent(QMouseEvent* event)
{

}

void CefWidget::mouseMoveEvent(QMouseEvent* event)
{
    QPointF point = event->position();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
    if (client_ && client_->GetBrowser() && client_->GetBrowser()->GetHost())
        client_->GetBrowser()->GetHost()->SendMouseMoveEvent(mouse_event, false);
}

void CefWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF point = event->position();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    if (event->button() == CefBrowserHost::MouseButtonType::MBT_RIGHT)
    {
        mouse_event.x = event->globalPosition().x();
        mouse_event.y = event->globalPosition().y();
    }

    CefBrowserHost::MouseButtonType type = event->button() == Qt::LeftButton ? CefBrowserHost::MouseButtonType::MBT_LEFT : CefBrowserHost::MouseButtonType::MBT_RIGHT;
    if (client_ && client_->GetBrowser())
        client_->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, true, 1);
}

void CefWidget::mousePressEvent(QMouseEvent* event)
{
    QPointF point = event->position();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    CefBrowserHost::MouseButtonType type = event->button() == Qt::LeftButton ? CefBrowserHost::MouseButtonType::MBT_LEFT : CefBrowserHost::MouseButtonType::MBT_RIGHT;
    if (client_ && client_->GetBrowser())
        client_->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, false, 1);
}

void CefWidget::CreateBrowser()
{
    CefWindowInfo window_info;
    HWND hwnd = (HWND)winId();
    window_info.parent_window = hwnd;
    window_info.windowless_rendering_enabled = 1;

    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 30;
    client_ = new QtCefClient(this);
    CefBrowserHost::CreateBrowser(window_info, client_, "https://www.baidu.com", browser_settings, nullptr, nullptr);
}

void CefWidget::keyPressEvent(QKeyEvent* event)
{
    CefKeyEvent key_event;
    key_event.type = KEYEVENT_CHAR;
    key_event.windows_key_code = event->key();

    if (client_ && client_->GetBrowser() && client_->GetBrowser()->GetHost())
        client_->GetBrowser()->GetHost()->SendKeyEvent(key_event);

    QWidget::keyPressEvent(event);
}

void CefWidget::keyReleaseEvent(QKeyEvent* event)
{
//    CefKeyEvent key_event;
//    key_event.type = KEYEVENT_CHAR;

//    key_event.windows_key_code = event->key();
//    if (client_ && client_->GetBrowser() && client_->GetBrowser()->GetHost())
//        client_->GetBrowser()->GetHost()->SendKeyEvent(key_event);
}

void CefWidget::wheelEvent(QWheelEvent* event)
{
    CefMouseEvent mouse_event;
    QPointF pos = event->position();
    mouse_event.x = pos.x();
    mouse_event.y = pos.y();
    if (client_ && client_->GetBrowser())
        client_->GetBrowser()->GetHost()->SendMouseWheelEvent(mouse_event, event->angleDelta().x(), event->angleDelta().y());
}

bool CefWidget::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info)
{
    Qt::CursorShape shape;
    switch (type) {
    case CT_POINTER:
        shape = Qt::CursorShape::ArrowCursor;
        break;
    case CT_HAND:
        shape = Qt::CursorShape::PointingHandCursor;
        break;
    case CT_IBEAM:
        shape = Qt::CursorShape::IBeamCursor;
        break;
    default:
        break;
    }

    QMetaObject::invokeMethod(this, [this, shape](){
        setCursor(shape);
    });

    return true;
}

void CefWidget::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    QWindow* window = windowHandle();
    QScreen* screen = window->screen();
    ratio_ = screen->devicePixelRatio();
    screen_info.device_scale_factor = ratio_;
}

void CefWidget::closeEvent(QCloseEvent* event)
{
    event->accept();
}

void CefWidget::resizeEvent(QResizeEvent* event)
{
    if (client_ && client_->GetBrowser())
    {
        client_->GetBrowser()->GetHost()->WasResized();
    }
}
