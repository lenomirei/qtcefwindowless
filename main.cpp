#include "mainwindow.h"

#include "qtcefapp.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    CefMainArgs main_args(GetModuleHandle(0));
    int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
    if (exit_code >= 0) {
        // The sub-process has completed so return here.
        return exit_code;
    }
    CefSettings settings;
    settings.multi_threaded_message_loop = 1;
    settings.windowless_rendering_enabled = 1;
    CefRefPtr<CefApp> app = new QtCefApp();
    CefInitialize(main_args, settings, app.get(), nullptr);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    auto res = a.exec();
    CefShutdown();
    return res;
}
