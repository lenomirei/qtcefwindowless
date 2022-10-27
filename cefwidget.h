#ifndef CEFWIDGET_H
#define CEFWIDGET_H

#include <QWidget>
#include "qtcefclient.h"

class CefWidget : public QWidget, public QtCefClient::Delegate
{
    Q_OBJECT
public:
    CefWidget(QWidget *parent = nullptr);
    virtual ~CefWidget();

    void CreateBrowser();

protected:
    struct Frame
    {
        int width = 0;
        int height = 0;
        uchar* buffer_ = nullptr;
    };
    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
    bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;

    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    Frame frame_;
    CefRefPtr<QtCefClient> client_;
    float ratio_ = 1.25f;

};

#endif // CEFWIDGET_H
