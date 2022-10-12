#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "include/cef_browser.h"
#include "qtcefclient.h"
#include "cefwidget.h"

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

void MainWindow::showEvent(QShowEvent* event)
{
     cef_widget_ = QSharedPointer<CefWidget>::create(ui->widget);
     cef_widget_->setAttribute(Qt::WA_StyledBackground, true);
     cef_widget_->setStyleSheet("background-color: red");
     cef_widget_->resize(500,500);
     cef_widget_->show();
}

void MainWindow::OnClicked()
{
    CefWindowInfo window_info;
    HWND hwnd = (HWND)ui->widget->winId();
    window_info.parent_window = hwnd;
    window_info.SetAsChild(hwnd, CefRect(0, 0, 500, 500));
    window_info.windowless_rendering_enabled = 1;

    CefBrowserSettings browser_settings;
    browser_settings.windowless_frame_rate = 30;
    QtCefClient* client = new QtCefClient(static_cast<QSharedPointer<QtCefClient::Delegate>>(cef_widget_));
    CefBrowserHost::CreateBrowser(window_info, client, "https://www.baidu.com", browser_settings, nullptr, nullptr);
}

