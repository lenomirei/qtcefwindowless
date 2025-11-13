/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-06-09 14:43:31
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-11-13 14:07:56
 * @FilePath: \qtcefwindowless\qtcefclient.cpp
 * @Description:
 *
 */
#include "qtcefclient.h"

#include <base/cef_bind.h>
#include <base/cef_callback.h>
#include <wrapper/cef_closure_task.h>

QtCefClient::QtCefClient(Delegate* delegate) : delegate_(delegate) {}

CefRefPtr<CefRenderHandler> QtCefClient::GetRenderHandler() { return this; }

CefRefPtr<CefLifeSpanHandler> QtCefClient::GetLifeSpanHandler() { return this; }

CefRefPtr<CefDisplayHandler> QtCefClient::GetDisplayHandler() { return this; }

CefRefPtr<CefLoadHandler> QtCefClient::GetLoadHandler() { return this; }

void QtCefClient::OnPaint(CefRefPtr<CefBrowser> browser,
                          CefRenderHandler::PaintElementType type,
                          const CefRenderHandler::RectList& dirtyRects,
                          const void* buffer, int width, int height) {
  if (delegate_)
    delegate_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void QtCefClient::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  if (delegate_) delegate_->GetViewRect(browser, rect);
}

bool QtCefClient::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                CefScreenInfo& screen_info) {
  if (delegate_) {
    delegate_->GetScreenInfo(browser, screen_info);
    return true;
  }
  return false;
}

void QtCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  if (delegate_) delegate_->OnAfterCreated(browser);
}

bool QtCefClient::OnCursorChange(CefRefPtr<CefBrowser> browser,
                                 CefCursorHandle cursor, cef_cursor_type_t type,
                                 const CefCursorInfo& custom_cursor_info) {
  if (delegate_)
    return delegate_->OnCursorChange(browser, cursor, type, custom_cursor_info);
  else
    return false;
}

bool QtCefClient::DoClose(CefRefPtr<CefBrowser> browser) {
  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void QtCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  if (delegate_) {
    delegate_->OnBeforeClose(browser);
  }
}

void QtCefClient::OnLoadStart(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType type) {
  if (delegate_) {
    delegate_->OnLoadStart(browser, frame, type);
  }
}

void QtCefClient::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                            CefRefPtr<CefFrame> frame, int http_status_code) {
  if (delegate_) {
    delegate_->OnLoadEnd(browser, frame, http_status_code);
  }
}

void QtCefClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                       bool is_loading, bool can_goback,
                                       bool can_goforward) {
  if (delegate_) {
    delegate_->OnLoadingStateChange(browser, is_loading, can_goback,
                                    can_goforward);
  }
}