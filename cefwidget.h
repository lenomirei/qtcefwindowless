#ifndef CEFWIDGET_H
#define CEFWIDGET_H

#include <QWidget>
#include "qtcefclient.h"

class CefWidget : public QWidget, public QtCefClient::Delegate
{
    Q_OBJECT
public:
    CefWidget(QWidget *parent = nullptr);

    void CreateBrowser();

protected:
    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

private:
    uchar* buffer_;
    CefRefPtr<QtCefClient> client_;

};

#endif // CEFWIDGET_H
