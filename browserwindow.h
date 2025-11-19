#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMutex>
#include <QWidget>
#include <unordered_map>

#include "qtcefclient.h"

class BrowserTabBar;
class CefWidget;
class TitleBar;

class BrowserWindow : public QWidget, public QtCefClient::Delegate {
  Q_OBJECT

 public:
  BrowserWindow(QWidget* parent = nullptr);
  virtual ~BrowserWindow();

  void CreateBrowser(const QUrl& url, int tab_index);

 protected:
  void showEvent(QShowEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void InitUI();
  void SwitchToTabBrowser(int index);

  // Delegate
  // override from CefRenderHandler
  virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                       CefRenderHandler::PaintElementType type,
                       const CefRenderHandler::RectList& dirtyRects,
                       const void* buffer, int width, int height) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser,
                           CefRect& rect) override;
  virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser,
                             CefScreenInfo& screen_info) override;

  // override from CefDisplayHandler
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                              CefCursorHandle cursor, cef_cursor_type_t type,
                              const CefCursorInfo& custom_cursor_info) override;

  // override from CefLifeSpanHandler
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // override from CefLoadHandler
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           CefLoadHandler::TransitionType type) override;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int http_status_code) override;
  void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool is_loading,
                            bool can_goback, bool can_goforward);

  // override from CefClient
  virtual bool OnProcessMessageReceived(
      CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override;

 protected slots:
  void ReallyClose();
  void OnBackButtonClicked();
  void OnForwardButtonClicked();
  void OnReloadButtonClicked();
  void OnAddressBarEnterPressed(const QUrl& url);
  void OnTabBarClicked(int index);
  void OnNewTabClicked();
  void OnTabCloseRequested(int index);

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
  BrowserTabBar* tab_bar_ = nullptr;
  TitleBar* title_bar_ = nullptr;
  CefRefPtr<QtCefClient> client_;

  std::unordered_map<int /*CefBrowser ID*/, CefRefPtr<CefBrowser>> browser_map_;
  std::unordered_map<int /*tab index*/, CefRefPtr<CefBrowser>> tab_browser_map_;
  CefRefPtr<CefBrowser> current_browser_ = nullptr;
  int current_browser_id_ = -1;

  QMutex mt_;
};
#endif  // MAINWINDOW_H
