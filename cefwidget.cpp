#include "cefwidget.h"

#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QDatetime>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

CefWidget::CefWidget(QWidget *parent)
    : QWidget{parent}
{
    setStyleSheet("background-color: red");
    buffer_ = new uchar[500*500*4];
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}


void CefWidget::OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    memcpy(buffer_,buffer,500*500*4);
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
    QImage frame = QImage(static_cast<const uchar*>(buffer_), 500, 500, QImage::Format_ARGB32_Premultiplied);
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
    client_->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, true, 1);
}

void CefWidget::mousePressEvent(QMouseEvent* event)
{
    QPointF point = event->position();
    CefMouseEvent mouse_event;
    mouse_event.x = point.x();
    mouse_event.y = point.y();
    CefBrowserHost::MouseButtonType type = event->button() == Qt::LeftButton ? CefBrowserHost::MouseButtonType::MBT_LEFT : CefBrowserHost::MouseButtonType::MBT_RIGHT;
    client_->GetBrowser()->GetHost()->SendMouseClickEvent(mouse_event, type, false, 1);
}

void CefWidget::CreateBrowser()
{
    CefWindowInfo window_info;
    HWND hwnd = (HWND)winId();
    window_info.parent_window = hwnd;
    window_info.SetAsChild(hwnd, CefRect(0, 0, 500, 500));
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
    client_->GetBrowser()->GetHost()->SendMouseWheelEvent(mouse_event, event->angleDelta().x(), event->angleDelta().y());
}
