#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "include/cef_browser.h"
#include "qtcefclient.h"
#include "cefwidget.h"

#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHBoxLayout* layout = new QHBoxLayout(ui->centralwidget);
    layout->setContentsMargins(0,0,0,0);
    ui->centralwidget->setLayout(layout);
    cef_widget_ = new CefWidget(ui->centralwidget);
    ui->centralwidget->layout()->addWidget(cef_widget_);
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
