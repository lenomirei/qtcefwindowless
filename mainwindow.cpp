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
    cef_widget_ = QSharedPointer<CefWidget>::create(ui->widget);
    cef_widget_->setAttribute(Qt::WA_StyledBackground, true);
    cef_widget_->setStyleSheet("background-color: red");
    cef_widget_->resize(500,500);
    cef_widget_->show();
}

MainWindow::~MainWindow()
{
    cef_widget_->close();
    cef_widget_ = nullptr;
    delete ui;
}

void MainWindow::showEvent(QShowEvent* event)
{
}

void MainWindow::OnClicked()
{
    cef_widget_->CreateBrowser();
}

