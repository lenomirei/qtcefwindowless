#include "browsertabbar.h"

BrowserTabBar::BrowserTabBar(QWidget* parent) : QTabBar(parent) {
  new_tab_button_ = new QPushButton(this);
  new_tab_button_->setText("+");

  connect(new_tab_button_, &QPushButton::clicked, this,
          &BrowserTabBar::OnNewTabButtonClicked);

  addTab("");
}

void BrowserTabBar::tabInserted(int index) {
  if (count() == 1 && index == 0) {
    // new tab button index
    setTabButton(index, QTabBar::ButtonPosition::RightSide, new_tab_button_);
    QTabBar::tabInserted(index);
    return;
  }

  if (count() - 1 == index) {
    moveTab(index - 1, index);
  }

  // if (0 == index && count() == 0) {
    
  // }
  // setTabButton(index, QTabBar::ButtonPosition::RightSide, nullptr);
  // setTabButton(index + 1, QTabBar::ButtonPosition::RightSide, new_tab_button_);
}


void BrowserTabBar::OnNewTabButtonClicked() {
  emit newTabClicked();
}