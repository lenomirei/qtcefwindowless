#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "include/cef_browser.h"
#include "qtcefclient.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::OnClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnClicked()
{
    CefWindowInfo window_info;
    HWND hwnd = (HWND)ui->widget->winId();
    window_info.parent_window = hwnd;
    window_info.SetAsChild(hwnd, CefRect(0, 0, 500, 500));

    CefBrowserSettings browser_settings;
    QtCefClient* client = new QtCefClient();
    CefBrowserHost::CreateBrowser(window_info, client, "https://www.baidu.com", browser_settings, nullptr, nullptr);
}

