#include "qtcefclient.h"

QtCefClient::QtCefClient(Delegate* delegate) : delegate_(delegate)
{

}

CefRefPtr<CefRenderHandler> QtCefClient::GetRenderHandler()
{
    return this;
}

CefRefPtr<CefLifeSpanHandler> QtCefClient::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefDisplayHandler> QtCefClient::GetDisplayHandler()
{
    return this;
}

void QtCefClient::OnPaint( CefRefPtr< CefBrowser > browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (delegate_)
        delegate_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void QtCefClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    if (delegate_)
        delegate_->GetViewRect(browser, rect);
}

bool QtCefClient::GetScreenInfo(CefRefPtr<CefBrowser> browser, CefScreenInfo& screen_info)
{
    if (delegate_)
    {
        delegate_->GetScreenInfo(browser, screen_info);
        return true;
    }
    return false;
}

void QtCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    if (!browser->IsPopup())
        browser_ = browser;
}

CefRefPtr<CefBrowser> QtCefClient::GetBrowser()
{
    return browser_;
}

bool QtCefClient::OnCursorChange(CefRefPtr<CefBrowser> browser, CefCursorHandle cursor, cef_cursor_type_t type, const CefCursorInfo& custom_cursor_info)
{
    if (delegate_)
        return delegate_->OnCursorChange(browser, cursor, type, custom_cursor_info);
    else
        return false;
}

void QtCefClient::CloseBrowser()
{
    delegate_ = nullptr;
    bool close_result =  browser_->GetHost()->TryCloseBrowser();
}
