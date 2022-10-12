#include "qtcefclient.h"

QtCefClient::QtCefClient(QSharedPointer<Delegate> delegate) : delegate_(delegate)
{

}

CefRefPtr<CefRenderHandler> QtCefClient::GetRenderHandler()
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
