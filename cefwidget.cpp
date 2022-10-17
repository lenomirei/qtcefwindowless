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
    setStyleSheet("background-color: red");
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

CefWidget::~CefWidget()
{

}

void CefWidget::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (!buffer_)
    {
        buffer_ = new uchar[width*height*4];
    }
    memcpy(buffer_,buffer,width * height * 4);
    update();
}

void CefWidget::showEvent(QShowEvent* event)
{
    this->resize(500,500);
}

void CefWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QImage frame = QImage(static_cast<const uchar*>(buffer_), 500 * ratio_, 500 * ratio_, QImage::Format_ARGB32_Premultiplied);
    frame.setDevicePixelRatio(ratio_);
    painter.drawImage(QRect(QPoint(0,0), QSize(frame.width(), frame.height())), frame);
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
    mouse_event.x = point.x() / ratio_;
    mouse_event.y = point.y() / ratio_;
    mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
    if (client_ && client_->GetBrowser() && client_->GetBrowser()->GetHost())
        client_->GetBrowser()->GetHost()->SendMouseMoveEvent(mouse_event, false);
}

void CefWidget::mouseReleaseEvent(QMouseEvent* event)
{
    QPointF point = event->position();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x() / ratio_;
    mouse_event.y = point.y() / ratio_;
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
    mouse_event.x = point.x() / ratio_;
    mouse_event.y = point.y() / ratio_;
    CefBrowserHost::MouseButtonType type = event->button() == Qt::LeftButton ? CefBrowserHost::MouseButtonType::MBT_LEFT : CefBrowserHost::MouseButtonType::MBT_RIGHT;
    if (client_ && client_->GetBrowser())
        client_->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, false, 1);
}

void CefWidget::CreateBrowser()
{
    CefWindowInfo window_info;
    HWND hwnd = (HWND)winId();
//    window_info.parent_window = hwnd;
//    window_info.SetAsChild(hwnd, CefRect(0, 0, 500, 500));
    window_info.windowless_rendering_enabled = 1;

    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 30;
    client_ = new QtCefClient(static_cast<QSharedPointer<QtCefClient::Delegate>>(this));
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
