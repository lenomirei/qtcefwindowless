#ifndef QTCEFCLIENT_H
#define QTCEFCLIENT_H
#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include <QSharedPointer>

class QtCefClient : public CefClient, public CefRenderHandler, public CefLifeSpanHandler
{
public:
    class Delegate
    {
    public:
        virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) = 0;
         virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
    };
    QtCefClient(QSharedPointer<Delegate> delegate);

    CefRefPtr<CefBrowser> GetBrowser();

protected:
    CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;

    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

private:
    QSharedPointer<Delegate> delegate_;
    CefRefPtr<CefBrowser> browser_;

private:
    IMPLEMENT_REFCOUNTING(QtCefClient);
};

#endif // QTCEFCLIENT_H
