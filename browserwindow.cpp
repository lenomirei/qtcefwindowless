/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-11-12 10:53:19
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-11-13 14:27:15
 * @FilePath: \qtcefwindowless\browserwindow.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "browserwindow.h"

#include <cef_browser.h>

#include <QCloseEvent>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>
#include <QWindow>

#include "addressbar.h"
#include "browsertabbar.h"
#include "cef/include/base/cef_callback.h"
#include "cef/include/wrapper/cef_closure_task.h"
#include "cefwidget.h"
#include "qtcefclient.h"
#include "titlebar.h"

#define DEFAULT_TAB_URL "https://www.baidu.com"

BrowserWindow::BrowserWindow(QWidget* parent) : QWidget(parent) {
  client_ = new QtCefClient(this);
  // can_close_ = false;
  InitUI();
}

BrowserWindow::~BrowserWindow() { cef_widget_ = nullptr; }

void BrowserWindow::InitUI() {
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  tab_bar_ = new BrowserTabBar(this);
  tab_bar_->setExpanding(false);
  tab_bar_->setTabsClosable(true);
  layout->addWidget(tab_bar_);
  connect(tab_bar_, &QTabBar::tabBarClicked, this, &BrowserWindow::OnTabBarClicked);
  connect(tab_bar_, &BrowserTabBar::newTabClicked, this, &BrowserWindow::OnNewTabClicked);
  connect(tab_bar_, &BrowserTabBar::tabCloseRequested, this, &BrowserWindow::OnTabCloseRequested, Qt::QueuedConnection);

  tab_bar_->addTab("New Tab");
  tab_bar_->setCurrentIndex(0);

  title_bar_ = new TitleBar(this);
  layout->addWidget(title_bar_);
  title_bar_->setFixedHeight(50);

  connect(title_bar_, &TitleBar::backButtonClicked, this,
          &BrowserWindow::OnBackButtonClicked);
  connect(title_bar_, &TitleBar::forwardButtonClicked, this,
          &BrowserWindow::OnForwardButtonClicked);
  connect(title_bar_, &TitleBar::reloadButtonClicked, this,
          &BrowserWindow::OnReloadButtonClicked);
  connect(title_bar_, &TitleBar::addressBarEnterPressed, this,
          &BrowserWindow::OnAddressBarEnterPressed);

  cef_widget_ = new CefWidget(this);
  connect(cef_widget_, &CefWidget::browserReadyToClose, this,
          &BrowserWindow::ReallyClose);
  connect(cef_widget_, &CefWidget::cefWidgetShow, this,
          &BrowserWindow::onContentShow);
  connect(cef_widget_, &CefWidget::cefWidgetMouseMove, this,
          &BrowserWindow::onContentMouseMoveEvent);
  connect(cef_widget_, &CefWidget::cefWidgetmousePress, this,
          &BrowserWindow::onContentMousePressEvent);
  connect(cef_widget_, &CefWidget::cefWidgetRelease, this,
          &BrowserWindow::onContentMouseReleaseEvent);
  connect(cef_widget_, &CefWidget::cefWidgetKeyPress, this,
          &BrowserWindow::onContentKeyPressEvent);
  connect(cef_widget_, &CefWidget::cefWidgetKeyRelease, this,
          &BrowserWindow::onContentKeyReleaseEvent);
  connect(cef_widget_, &CefWidget::cefWidgetWheel, this,
          &BrowserWindow::onContentMouseWheelEvent);
  connect(cef_widget_, &CefWidget::cefWidgetSizeChanged, this,
          &BrowserWindow::onContentSizeChanged);

  layout->addWidget(cef_widget_);
  cef_widget_->show();

  if (current_browser_ == nullptr) {
    CreateBrowser(QUrl(DEFAULT_TAB_URL), 0);
  }
}

void BrowserWindow::showEvent(QShowEvent* event) {
  setMinimumSize(QSize(500, 500));
}

void BrowserWindow::closeEvent(QCloseEvent* event) {
  mt_.lock();
  if (browser_map_.empty()) {
    event->accept();
  } else {
    for (const auto& [tab_index, browser] : tab_browser_map_) {
      emit tab_bar_->tabCloseRequested(tab_index);
      // CefPostTask(CefThreadId::TID_UI,
      //             base::BindOnce(
      //                 [](CefRefPtr<CefBrowser> browser) {
      //                   browser->GetHost()->CloseBrowser(false);
      //                 },
      //                 browser));
    }
    event->ignore();
  }
  mt_.unlock();
  // if (can_close_) {
  //   event->accept();
  // } else {
  //   cef_widget_->Close();
  //   event->ignore();
  // }
}

void BrowserWindow::ReallyClose() {
  // can_close_ = true;
  // close();
}

void BrowserWindow::OnBackButtonClicked() {
  if (current_browser_) {
    current_browser_->GoBack();
  }
}

void BrowserWindow::OnForwardButtonClicked() {
  if (current_browser_) {
    current_browser_->GoForward();
  }
}

void BrowserWindow::OnReloadButtonClicked() {
  if (current_browser_) {
    current_browser_->Reload();
  }
}

void BrowserWindow::OnAddressBarEnterPressed(const QUrl& url) {
  if (current_browser_ && current_browser_->GetMainFrame()) {
    current_browser_->GetMainFrame()->LoadURL(url.toString().toStdString());
  }
}

void BrowserWindow::CreateBrowser(const QUrl& url, int tab_index) {
  CefBrowserSettings browser_settings;
  browser_settings.windowless_frame_rate = 30;
  CefPostTask(
      CefThreadId::TID_UI,
      base::BindOnce(
          [](CefRefPtr<CefClient> client, CefBrowserSettings browser_settings, QUrl url, int tab_index) {
            CefWindowInfo window_info;
            window_info.windowless_rendering_enabled = 1;
            auto extra_info = CefDictionaryValue::Create();
            extra_info->SetInt("tab_index", tab_index);
            CefBrowserHost::CreateBrowser(window_info, client,
                                          url.toString().toStdString(),
                                          browser_settings, extra_info, nullptr);
          },
          client_, browser_settings, url, tab_index));
}

void BrowserWindow::onContentShow() {}

void BrowserWindow::onContentMouseMoveEvent(QMouseEvent* event) {
  if (nullptr == current_browser_) return;
  QPointF point = event->pos();
  CefMouseEvent mouse_event;
  mouse_event.x = point.x();
  mouse_event.y = point.y();
  mouse_event.modifiers = EVENTFLAG_LEFT_MOUSE_BUTTON;
  CefPostTask(CefThreadId::TID_UI,
              base::BindOnce(
                  [](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event) {
                    if (browser && browser->GetHost())
                      browser->GetHost()->SendMouseMoveEvent(mouse_event,
                                                             false);
                  },
                  current_browser_, mouse_event));
}

void BrowserWindow::onContentMouseReleaseEvent(QMouseEvent* event) {
  if (nullptr == current_browser_) return;
  QPointF point = event->pos();
  CefMouseEvent mouse_event;
  mouse_event.x = point.x();
  mouse_event.y = point.y();
  if (event->button() == CefBrowserHost::MouseButtonType::MBT_RIGHT) {
    mouse_event.x = event->globalPos().x();
    mouse_event.y = event->globalPos().y();
  }

  CefBrowserHost::MouseButtonType type =
      event->button() == Qt::LeftButton
          ? CefBrowserHost::MouseButtonType::MBT_LEFT
          : CefBrowserHost::MouseButtonType::MBT_RIGHT;
  CefPostTask(CefThreadId::TID_UI,
              base::BindOnce(
                  [](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event,
                     CefBrowserHost::MouseButtonType type) {
                    if (browser && browser->GetHost())
                      browser->GetHost()->SendMouseClickEvent(mouse_event, type,
                                                              true, 1);
                  },
                  current_browser_, mouse_event, type));
}

void BrowserWindow::onContentMousePressEvent(QMouseEvent* event) {
  if (nullptr == current_browser_) return;
  QPointF point = event->pos();
  CefMouseEvent mouse_event;
  mouse_event.x = point.x();
  mouse_event.y = point.y();
  CefBrowserHost::MouseButtonType type =
      event->button() == Qt::LeftButton
          ? CefBrowserHost::MouseButtonType::MBT_LEFT
          : CefBrowserHost::MouseButtonType::MBT_RIGHT;
  CefPostTask(CefThreadId::TID_UI,
              base::BindOnce(
                  [](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event,
                     CefBrowserHost::MouseButtonType type) {
                    if (browser && browser->GetHost())
                      browser->GetHost()->SendMouseClickEvent(mouse_event, type,
                                                              false, 1);
                  },
                  current_browser_, mouse_event, type));
}

void MyFunc(int arg) { int leno = arg; }

void BrowserWindow::onContentKeyPressEvent(QKeyEvent* event) {
  if (nullptr == current_browser_) return;
  CefKeyEvent key_event;
  key_event.type = KEYEVENT_RAWKEYDOWN;
  key_event.windows_key_code = event->nativeVirtualKey();
  key_event.native_key_code = event->nativeScanCode();

  CefPostTask(CefThreadId::TID_UI,
              base::BindOnce(
                  [](CefRefPtr<CefBrowser> browser, CefKeyEvent key_event) {
                    if (browser && browser->GetHost())
                      browser->GetHost()->SendKeyEvent(key_event);
                  },
                  current_browser_, key_event));

  if (!event->text().isEmpty() && event->text()[0].isPrint()) {
    CefKeyEvent char_event = key_event;
    char_event.type = KEYEVENT_CHAR;
    char_event.character = event->text()[0].unicode();
    CefPostTask(CefThreadId::TID_UI,
                base::BindOnce(
                    [](CefRefPtr<CefBrowser> browser, CefKeyEvent key_event) {
                      if (browser && browser->GetHost())
                        browser->GetHost()->SendKeyEvent(key_event);
                    },
                    current_browser_, char_event));
  }

  QWidget::keyPressEvent(event);
}

void BrowserWindow::onContentKeyReleaseEvent(QKeyEvent* event) {
  if (nullptr == current_browser_) return;
  CefKeyEvent key_event;
  key_event.type = KEYEVENT_KEYUP;
  key_event.windows_key_code = event->nativeVirtualKey();
  key_event.native_key_code = event->nativeScanCode();

  CefPostTask(CefThreadId::TID_UI,
              base::BindOnce(
                  [](CefRefPtr<CefBrowser> browser, CefKeyEvent key_event) {
                    if (browser && browser->GetHost())
                      browser->GetHost()->SendKeyEvent(key_event);
                  },
                  current_browser_, key_event));
}

void BrowserWindow::onContentMouseWheelEvent(QWheelEvent* event) {
  if (nullptr == current_browser_) return;
  CefMouseEvent mouse_event;
  QPointF pos = event->position();
  mouse_event.x = pos.x();
  mouse_event.y = pos.y();

  int delta_x = event->angleDelta().x();
  int delta_y = event->angleDelta().y();
  CefPostTask(CefThreadId::TID_UI,
              base::BindOnce(
                  [](CefRefPtr<CefBrowser> browser, CefMouseEvent mouse_event,
                     int delta_x, int delta_y) {
                    if (browser && browser->GetHost())
                      browser->GetHost()->SendMouseWheelEvent(mouse_event,
                                                              delta_x, delta_y);
                  },
                  current_browser_, mouse_event, delta_x, delta_y));
}

void BrowserWindow::onContentSizeChanged() {
  mt_.lock();
  for (const auto& [browser_id, browser] : browser_map_) {
    CefPostTask(CefThreadId::TID_UI, base::BindOnce(
                                         [](CefRefPtr<CefBrowser> browser) {
                                           if (browser) {
                                             browser->GetHost()->WasResized();
                                           }
                                         },
                                         browser));
  }
  mt_.unlock();
}

void BrowserWindow::OnPaint(CefRefPtr<CefBrowser> browser,
                            CefRenderHandler::PaintElementType type,
                            const CefRenderHandler::RectList& dirtyRects,
                            const void* buffer, int width, int height) {
  if (cef_widget_)
    cef_widget_->OnPaint(browser, type, dirtyRects, buffer, width, height);
}

void BrowserWindow::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
  if (cef_widget_) {
    const QRect& q_rect = cef_widget_->geometry();
    rect.x = q_rect.x();
    rect.y = q_rect.y();
    rect.width = q_rect.width();
    rect.height = q_rect.height();
  }
}

void BrowserWindow::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                  CefScreenInfo& screen_info) {
  QWindow* window = windowHandle();
  if (window != nullptr) {
    QScreen* screen = window->screen();
    ratio_ = screen->devicePixelRatio();
    screen_info.device_scale_factor = ratio_;
  }
}

void BrowserWindow::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  if (nullptr == browser) return;

  if (current_browser_ != nullptr) {
    current_browser_->GetHost()->WasHidden(true);
  }
  current_browser_ = browser;
  current_browser_id_ = browser->GetIdentifier();

  if (!browser->IsPopup()) {
    // TODO(lenomirei): Add to map. Maybe set current browser
    int browser_id = browser->GetIdentifier();
    mt_.lock();
    if (browser_map_.find(browser_id) == browser_map_.end()) {
      browser_map_.emplace(browser_id, browser);
      mt_.unlock();
    } else {
      // TODO(lenomiei): add some warn logs here
    }
  }
}

bool BrowserWindow::OnCursorChange(CefRefPtr<CefBrowser> browser,
                                   CefCursorHandle cursor,
                                   cef_cursor_type_t type,
                                   const CefCursorInfo& custom_cursor_info) {
  if (cef_widget_) {
    cef_widget_->OnCursorChange(browser, cursor, type, custom_cursor_info);
    return false;
  }
}

void BrowserWindow::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  int browser_id = browser->GetIdentifier();
  mt_.lock();
  if (current_browser_ && current_browser_->IsSame(browser)) {
    current_browser_ = nullptr;
    current_browser_id_ = -1;
  }
  if (auto find_res = browser_map_.find(browser_id); find_res != browser_map_.end()) {
    browser_map_.erase(find_res);
  } else {
    // TODO(lenomirei): Add error log here.
  }

  if (browser_map_.empty()) {
    QMetaObject::invokeMethod(this, [this]() { close(); });
  }
  mt_.unlock();
}

void BrowserWindow::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType type) {
  if (!current_browser_->IsSame(browser)) {
    if (frame->IsMain()) {
      // TODO(lenomirei): Update tab label
    }
    return;
  } else {
    if (frame->IsMain()) {
      std::string url = frame->GetURL();
      QMetaObject::invokeMethod(this, [this, url]() {
        title_bar_->UpdateAddressBarText(QString::fromStdString(url));
        title_bar_->SetReloadButtonLoading(true);
      });
    }
  }
}

void BrowserWindow::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame, int http_status_code) {
  if (!current_browser_->IsSame(browser)) {
    if (frame->IsMain()) {
      // TODO(lenomirei): Update tab label
    }
    return;
  } else {
    if (frame->IsMain()) {
      std::string url = frame->GetURL();
      QMetaObject::invokeMethod(this, [this, url]() {
        title_bar_->SetReloadButtonLoading(false);
      });
    }
  }
}

void BrowserWindow::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                                         bool is_loading, bool can_goback,
                                         bool can_goforward) {
  if (!current_browser_->IsSame(browser)) {
    return;
  } else {
    QMetaObject::invokeMethod(
        this, [this, is_loading, can_goback, can_goforward]() {
          title_bar_->SetBackButtonDisabled(!can_goback);
          title_bar_->SetForwardButtonDisabled(!can_goforward);
        });
  }
}

void BrowserWindow::OnTabBarClicked(int index) {
  if (index == tab_bar_->count() - 1) {
  } else {
    // switch to specific browser
    SwitchToTabBrowser(index);
  }
}

void BrowserWindow::SwitchToTabBrowser(int index) {
  mt_.lock();
  if (nullptr != current_browser_)
    current_browser_->GetHost()->WasHidden(true);
  
  if (auto find_res = tab_browser_map_.find(index); find_res != tab_browser_map_.end()) {
    auto next_browser = tab_browser_map_[index];
    next_browser->GetHost()->WasHidden(false);
    current_browser_ = next_browser;
    current_browser_id_ = next_browser->GetIdentifier();

    title_bar_->UpdateAddressBarText(QString::fromStdString(
        current_browser_->GetMainFrame()->GetURL().ToString()));
  }
  
  mt_.unlock();
}

bool BrowserWindow::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
  
  auto argument_list = message->GetArgumentList();
  auto dic = argument_list->GetDictionary(0);
  int tab_index = dic->GetInt("tab_index");
  mt_.lock();
  tab_browser_map_.emplace(tab_index, browser);
  mt_.unlock();
  return true;
}

void BrowserWindow::OnNewTabClicked() {
  // click new tab button
  int index = tab_bar_->count() - 1;
  CreateBrowser(QUrl(DEFAULT_TAB_URL), index);
  tab_bar_->addTab("New Tab");
  tab_bar_->setCurrentIndex(index);
}

void BrowserWindow::OnTabCloseRequested(int index) {
  if (auto find_res = tab_browser_map_.find(index); find_res != tab_browser_map_.end()) {
    CefRefPtr<CefBrowser> browser = find_res->second;
    tab_browser_map_.erase(find_res);
    tab_bar_->removeTab(index);
    if (index == tab_bar_->currentIndex()) {
      int next_index = (index - 1 >= 0) ? index - 1 : index;
      tab_bar_->setCurrentIndex(next_index);
      SwitchToTabBrowser(next_index);
    }
    CefPostTask(CefThreadId::TID_UI,
                  base::BindOnce(
                      [](CefRefPtr<CefBrowser> browser) {
                        browser->GetHost()->CloseBrowser(false);
                      },
                      browser));
  } else {
    // TODO(lenomirei): Add warning log here
  }
}