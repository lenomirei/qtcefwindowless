/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-06-09 14:43:31
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-11-05 12:10:05
 * @FilePath: \qtcefwindowless\mainwindow.cpp
 * @Description:
 *
 */
#include "browserwindow.h"

#include <cef_browser.h>

#include <QCloseEvent>
#include <QPushButton>
#include <QVBoxLayout>

#include "addressbar.h"
#include "cefwidget.h"
#include "qtcefclient.h"

BrowserWindow::BrowserWindow(QWidget* parent) : QWidget(parent) {
  can_close_ = false;
  InitUI();
}

BrowserWindow::~BrowserWindow() {
  cef_widget_->close();
  cef_widget_ = nullptr;
}

void BrowserWindow::InitUI() {
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  setLayout(layout);

  title_bar_ = new QWidget(this);
  layout->addWidget(title_bar_);
  title_bar_->setFixedHeight(50);

  QHBoxLayout* titlebarlayout = new QHBoxLayout(title_bar_);
  title_bar_->setLayout(titlebarlayout);
  buttons_container_ = new QWidget(title_bar_);
  titlebarlayout->addWidget(buttons_container_);

  QHBoxLayout* buttons_layout = new QHBoxLayout(buttons_container_);
  buttons_layout->setSpacing(5);
  buttons_layout->setContentsMargins(0, 0, 10, 0);
  buttons_container_->setLayout(buttons_layout);

  QPushButton* back_button_ = new QPushButton(buttons_container_);
  buttons_layout->addWidget(back_button_);
  back_button_->setText("Back");
  connect(back_button_, &QPushButton::clicked, this, &BrowserWindow::OnBackButtonClicked);
  QPushButton* forward_button_ = new QPushButton(buttons_container_);
  buttons_layout->addWidget(forward_button_);
  forward_button_->setText("Forward");
  connect(forward_button_, &QPushButton::clicked, this, &BrowserWindow::OnForwardButtonClicked);
  QPushButton* refresh_button_ = new QPushButton(buttons_container_);
  buttons_layout->addWidget(refresh_button_);
  refresh_button_->setText("Refresh");
  connect(refresh_button_, &QPushButton::clicked, this, &BrowserWindow::OnRefreshButtonClicked);

  AddressBar* addressbar = new AddressBar(buttons_container_);
  buttons_layout->addWidget(addressbar);
  connect(addressbar, &AddressBar::navigateToUrl, this, &BrowserWindow::OnAddressBarEnterPressed);

  buttons_container_->setMinimumWidth(buttons_container_->sizeHint().width());

  cef_widget_ = new CefWidget(this);
  connect(cef_widget_, &CefWidget::browserReadyToClose, this,
          &BrowserWindow::ReallyClose);
  layout->addWidget(cef_widget_);
  cef_widget_->show();
}

void BrowserWindow::showEvent(QShowEvent* event) {
  setMinimumSize(QSize(500, 500));
}

void BrowserWindow::closeEvent(QCloseEvent* event) {
  if (can_close_) {
    event->accept();
  } else {
    cef_widget_->Close();
    event->ignore();
  }
}

void BrowserWindow::ReallyClose() {
  can_close_ = true;
  close();
}

void BrowserWindow::OnBackButtonClicked() {
  if (cef_widget_) {
    cef_widget_->Back();
  }
}

void BrowserWindow::OnForwardButtonClicked() {
  if (cef_widget_) {
    cef_widget_->Forward();
  }
}

void BrowserWindow::OnRefreshButtonClicked() {
  if (cef_widget_) {
    cef_widget_->Refresh();
  }
}

void BrowserWindow::OnAddressBarEnterPressed(const QUrl& url) {
  if (cef_widget_) {
    cef_widget_->Navigate(url);
  }
}