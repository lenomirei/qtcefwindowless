#include "browserwindow.h"

#include "qtcefapp.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    CefMainArgs main_args(GetModuleHandle(0));
    CefRefPtr<CefApp> app = new QtCefApp();

    int exit_code = CefExecuteProcess(main_args, app.get(), nullptr);
    if (exit_code >= 0) {
      // The sub-process terminated, exit now.
      return exit_code;
    }

    CefSettings settings;
    settings.multi_threaded_message_loop = 1;
    settings.windowless_rendering_enabled = 1;
    CefInitialize(main_args, settings, app.get(), nullptr);

    QApplication a(argc, argv);
    BrowserWindow w;
    w.show();
    auto res = a.exec();
    CefShutdown();
    return res;
}
