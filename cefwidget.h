#ifndef CEFWIDGET_H
#define CEFWIDGET_H

#include <QWidget>
#include "qtcefclient.h"

class CefWidget : public QWidget, public QtCefClient::Delegate
{
    Q_OBJECT
public:
    CefWidget(QWidget *parent = nullptr);

protected:
    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent* event) override;

private:
    uchar* buffer_;

};

#endif // CEFWIDGET_H
