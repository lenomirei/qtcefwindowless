#ifndef CEFWIDGET_H
#define CEFWIDGET_H

#include <QMutex>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLWidget>
#include <QTimer>

#include "cef/include/cef_browser.h"
#include <cef_render_handler.h>

class CefWidget : public QOpenGLWidget,
                  public QOpenGLFunctions {
  Q_OBJECT
 public:
  CefWidget(QWidget* parent = nullptr);
  virtual ~CefWidget();


  void OnPaint(CefRefPtr<CefBrowser> browser,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects, const void* buffer,
               int width, int height);
  void OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor,
                      cef_cursor_type_t type,
                      const CefCursorInfo& custom_cursor_info);

 protected:
  void initializeGL() override;
  void paintGL() override;
  void resizeGL(int w, int h) override;
  void showEvent(QShowEvent *event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void wheelEvent(QWheelEvent* event) override;

  void UpdateFrame(const uchar* buffer, int width, int height);

 signals:
  void browserReadyToClose();
  void cefWidgetShow();
  void cefWidgetMouseMove(QMouseEvent* event);
  void cefWidgetmousePress(QMouseEvent* event);
  void cefWidgetRelease(QMouseEvent* event);
  void cefWidgetKeyPress(QKeyEvent* event);
  void cefWidgetKeyRelease(QKeyEvent* event);
  void cefWidgetWheel(QWheelEvent* event);
  void cefWidgetSizeChanged(void);

 protected slots:
  void OnTimeout();
  void NotifyResizeToCEF();

 private:
  QOpenGLTexture* texture_;
  uchar* frame_buffer_ = nullptr;
  uchar* front_frame_buffer_ = nullptr;
  int width_ = 0;
  int height_ = 0;
  bool need_recreate_texture_ = false;
  QMutex mt_;
  QTimer* timer_ = nullptr;
  QTimer* debounce_timer_ = nullptr;

  QOpenGLShaderProgram shader_;
  QOpenGLBuffer vbo_;
  QOpenGLVertexArrayObject vao_;
};

#endif  // CEFWIDGET_H
