#include "qtcefapp.h"

QtCefApp::QtCefApp() {}

void QtCefApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDictionaryValue> extra_info) {
  auto main_frame = browser->GetMainFrame();
  if (nullptr != main_frame) {
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create(std::to_string(browser->GetIdentifier()) + "_created");
    auto argument_list = message->GetArgumentList();

    int index = extra_info->GetInt("tab_index");

    auto dic = CefDictionaryValue::Create();
    dic->SetInt("tab_index", index);
    argument_list->SetDictionary(0, dic);

    main_frame->SendProcessMessage(CefProcessId::PID_BROWSER, message);
  }
}