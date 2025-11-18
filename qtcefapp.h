#ifndef QTCEFAPP_H
#define QTCEFAPP_H

#include <cef_app.h>
#include <cef_render_process_handler.h>

class QtCefApp : public CefApp, CefRenderProcessHandler {
 public:
  class Delegate {
   public:
    virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDictionaryValue> extra_info) = 0;
  };
  QtCefApp();

 protected:
  //
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()
      override {
    return this;
  }

  // override from CefRenderProcessHandler
  virtual void OnBrowserCreated(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefDictionaryValue> extra_info) override;

 private:
  IMPLEMENT_REFCOUNTING(QtCefApp);
};

#endif  // QTCEFAPP_H
