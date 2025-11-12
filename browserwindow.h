#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMutex>
#include <QWidget>
#include <unordered_map>

#include "qtcefclient.h"

class CefWidget;

class BrowserWindow : public QWidget, public QtCefClient::Delegate {
  Q_OBJECT

 public:
  BrowserWindow(QWidget* parent = nullptr);
  virtual ~BrowserWindow();

  void CreateBrowser();

 protected:
  void showEvent(QShowEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void InitUI();

  // Delegate
  // override from CefRenderHandler
  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects, const void* buffer,
               int width, int height) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser,
                     CefScreenInfo& screen_info) override;

  // override from CefDisplayHandler
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor,
                      cef_cursor_type_t type,
                      const CefCursorInfo& custom_cursor_info) override;

  // override from CefLifeSpanHandler
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

 protected slots:
  void ReallyClose();
  void OnBackButtonClicked();
  void OnForwardButtonClicked();
  void OnRefreshButtonClicked();
  void OnAddressBarEnterPressed(const QUrl& url);

  // browser content slots
  void onContentShow();
  void onContentMouseMoveEvent(QMouseEvent* event);
  void onContentMouseReleaseEvent(QMouseEvent* event);
  void onContentMousePressEvent(QMouseEvent* event);
  void onContentKeyPressEvent(QKeyEvent* event);
  void onContentKeyReleaseEvent(QKeyEvent* event);
  void onContentMouseWheelEvent(QWheelEvent* event);
  void onContentSizeChanged();

 private:
  float ratio_ = 1.0f;
  CefWidget* cef_widget_ = nullptr;
  // bool can_close_ = false;
  QWidget* title_bar_ = nullptr;
  QWidget* buttons_container_ = nullptr;
  CefRefPtr<QtCefClient> client_;

  std::unordered_map<int /*CefBrowser ID*/, CefRefPtr<CefBrowser>> browser_map_;
  CefRefPtr<CefBrowser> current_browser_ = nullptr;
  int current_browser_id_ = -1;

  QMutex mt_;
};
#endif  // MAINWINDOW_H
