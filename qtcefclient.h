#ifndef QTCEFCLIENT_H
#define QTCEFCLIENT_H

#include <cef_client.h>
#include <cef_load_handler.h>
#include <cef_render_handler.h>
#include <cef_task.h>
#include <cef_thread.h>

#include <QSharedPointer>

class QtCefClient : public CefClient,
                    public CefRenderHandler,
                    public CefLifeSpanHandler,
                    public CefDisplayHandler,
                    public CefLoadHandler {
 public:
  class Delegate {
   public:
    virtual void OnPaint(CefRefPtr<CefBrowser> browser,
                         CefRenderHandler::PaintElementType type,
                         const CefRenderHandler::RectList& dirtyRects,
                         const void* buffer, int width, int height) = 0;
    virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
    virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser,
                                CefCursorHandle cursor, cef_cursor_type_t type,
                                const CefCursorInfo& custom_cursor_info) = 0;
    virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser,
                               CefScreenInfo& screen_info) = 0;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) = 0;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) = 0;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame, int http_status_code) = 0;
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                      bool is_loading, bool can_goback,
                                      bool can_goforward) = 0;
  };
  QtCefClient(Delegate* delegate);

 protected:
  // override from CefClient
  CefRefPtr<CefRenderHandler> GetRenderHandler() override;
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
  CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;
  CefRefPtr<CefLoadHandler> GetLoadHandler() override;

  // override from CefRenderHandler
  void OnPaint(CefRefPtr<CefBrowser> browser,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects, const void* buffer,
               int width, int height) override;
  virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
  virtual bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                     CefScreenInfo& screen_info) override;

  // override from CefDisplayHandler
  virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor,
                      cef_cursor_type_t type,
                      const CefCursorInfo& custom_cursor_info) override;

  // override from CefLifeSpanHandler
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) override;
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

  // override from CefLoadHandler
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                 int http_status_code) override;

  virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                    bool is_loading, bool can_goback,
                                    bool can_goforward) override;

 private:
  Delegate* delegate_ = nullptr;

 private:
  IMPLEMENT_REFCOUNTING(QtCefClient);
};

#endif  // QTCEFCLIENT_H
