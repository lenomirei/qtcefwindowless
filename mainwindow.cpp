/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-06-09 14:43:31
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-09-15 12:33:44
 * @FilePath: \qtcefwindowless\mainwindow.cpp
 * @Description: 
 * 
 */
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <cef_browser.h>
#include "qtcefclient.h"
#include "cefwidget.h"

#include <QHBoxLayout>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHBoxLayout* layout = new QHBoxLayout(ui->centralwidget);
    layout->setContentsMargins(0,0,0,0);
    ui->centralwidget->setLayout(layout);
    cef_widget_ = new CefWidget(ui->centralwidget);
    connect(cef_widget_, &CefWidget::browserReadyToClose, this, &MainWindow::ReallyClose);
    ui->centralwidget->layout()->addWidget(cef_widget_);
    cef_widget_->show();
    can_close_ = false;
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

void MainWindow::closeEvent(QCloseEvent* event) {
    if (can_close_) {
        event->accept();
    }
    else {
        cef_widget_->Close();
        event->ignore();
    }
}

void MainWindow::ReallyClose() {
    can_close_ = true;
    close();
}