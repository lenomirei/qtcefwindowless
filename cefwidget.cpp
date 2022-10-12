#include "cefwidget.h"

#include <QPainter>
#include <QPixmap>
#include <QBitmap>
#include <QDatetime>

CefWidget::CefWidget(QWidget *parent)
    : QWidget{parent}
{
    setStyleSheet("background-color: red");
    buffer_ = new uchar[500*500*4];
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
