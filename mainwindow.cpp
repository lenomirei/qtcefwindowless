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

#include <cef_browser.h>

#include <QCloseEvent>
#include <QVBoxLayout>

#include "cefwidget.h"
#include "qtcefclient.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  can_close_ = false;
  InitUI();
}

MainWindow::~MainWindow() {
  cef_widget_->close();
  cef_widget_ = nullptr;
}

void MainWindow::InitUI() {
  centralWidget = new QWidget(this);
  this->setCentralWidget(centralWidget);
  QVBoxLayout* layout = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  centralWidget->setLayout(layout);

  title_bar_ = new QWidget(centralWidget);
  layout->addWidget(title_bar_);
  title_bar_->setFixedHeight(50);
  buttons_container_ = new QWidget(title_bar_);
  buttons_container_->setGeometry(QRect(0, 0, width, title_bar_->height());


  cef_widget_ = new CefWidget(centralWidget);
  connect(cef_widget_, &CefWidget::browserReadyToClose, this,
          &MainWindow::ReallyClose);
  layout->addWidget(cef_widget_);
  cef_widget_->show();
  
}

void MainWindow::showEvent(QShowEvent* event) {
  setMinimumSize(QSize(500, 500));
}

void MainWindow::closeEvent(QCloseEvent* event) {
  if (can_close_) {
    event->accept();
  } else {
    cef_widget_->Close();
    event->ignore();
  }
}

void MainWindow::ReallyClose() {
  can_close_ = true;
  close();
}