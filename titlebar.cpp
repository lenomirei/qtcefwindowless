/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-11-12 14:47:52
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-11-13 12:25:21
 * @FilePath: \qtcefwindowless\TitleBar.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "titlebar.h"

#include "addressbar.h"

#include <QHBoxLayout>

TitleBar::TitleBar(QWidget* parent) : QWidget{parent} {
  InitUI();
}

void TitleBar::InitUI() {
  QHBoxLayout* layout = new QHBoxLayout(this);
  layout->setSpacing(5);
  layout->setContentsMargins(0, 0, 10, 0);
  setLayout(layout);

  back_button_ = new QPushButton(this);
  layout->addWidget(back_button_);
  back_button_->setText("Back");
  connect(back_button_, &QPushButton::clicked, this,
          &TitleBar::backButtonClicked);
  forward_button_ = new QPushButton(this);
  layout->addWidget(forward_button_);
  forward_button_->setText("Forward");
  connect(forward_button_, &QPushButton::clicked, this,
          &TitleBar::forwardButtonClicked);
  reload_button_ = new QPushButton(this);
  layout->addWidget(reload_button_);
  reload_button_->setText("Reload");
  connect(reload_button_, &QPushButton::clicked, this,
          &TitleBar::reloadButtonClicked);

  address_bar_ = new AddressBar(this);
  layout->addWidget(address_bar_);
  connect(address_bar_, &AddressBar::navigateToUrl, this,
          &TitleBar::addressBarEnterPressed);
}

void TitleBar::SetBackButtonDisabled(bool disabled) {
  back_button_->setDisabled(disabled);
}

void TitleBar::SetForwardButtonDisabled(bool disabled) {
  forward_button_->setDisabled(disabled);
}

void TitleBar::SetReloadButtonLoading(bool loading) {
  if (loading)
    reload_button_->setText("Stop");
  else
    reload_button_->setText("Reload");
}

void TitleBar::UpdateAddressBarText(const QString& url) {
  address_bar_->setText(url);
}

void TitleBar::OnReloadButtonClicked() {
  if (reload_button_->text() == "Reload") {
    emit reloadButtonClicked();
  } else {
    emit stopLoadingButtonClicked();
  }
}