#ifndef CEFWIDGET_H
#define CEFWIDGET_H

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMutex>
#include "qtcefclient.h"

class CefWidget : public QOpenGLWidget, public QOpenGLFunctions, public QtCefClient::Delegate
{
    Q_OBJECT
public:
    CefWidget(QWidget *parent = nullptr);
    virtual ~CefWidget();

    void CreateBrowser();

protected:
    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
    bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
    //void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void closeEvent(QCloseEvent* event) override;
    //void resizeEvent(QResizeEvent* event) override;

    void UpdateFrame(const uchar* buffer, int width, int height);

private:
    CefRefPtr<QtCefClient> client_;
    float ratio_ = 1.0f;
    QOpenGLTexture* texture_;
    uchar* frame_buffer_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    QMutex mt_;

    QOpenGLShaderProgram shader_;
    QOpenGLBuffer vbo_;
    QOpenGLVertexArrayObject vao_;
};

#endif // CEFWIDGET_H
