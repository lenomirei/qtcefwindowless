#ifndef QTCEFCLIENT_H
#define QTCEFCLIENT_H
#include "include/cef_client.h"
#include "include/cef_render_handler.h"
#include <QSharedPointer>

class QtCefClient : public CefClient, public CefRenderHandler, public CefLifeSpanHandler, public CefDisplayHandler
{
public:
    class Delegate
    {
    public:
        virtual void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) = 0;
        virtual void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) = 0;
        virtual bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) = 0;
        virtual void GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) = 0;
    };
    QtCefClient(QSharedPointer<Delegate> delegate);

    CefRefPtr<CefBrowser> GetBrowser();

protected:
    CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override;

    void OnPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height) override;
    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    bool GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info) override;

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    bool OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info) override;

private:
    QSharedPointer<Delegate> delegate_;
    CefRefPtr<CefBrowser> browser_;

private:
    IMPLEMENT_REFCOUNTING(QtCefClient);
};

#endif // QTCEFCLIENT_H
