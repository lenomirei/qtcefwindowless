/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-11-05 11:59:18
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-11-05 12:17:25
 * @FilePath: \qtcefwindowless\AddressBar.cpp
 * @Description: custom address bar
 */
#include "addressbar.h"

#include <QUrl>

AddressBar::AddressBar(QWidget* parent) : QLineEdit{parent} {}

void AddressBar::keyPressEvent(QKeyEvent* key_event) {
  auto key = key_event->key();
  if (key == Qt::Key_Enter || key == Qt::Key_Return) {
    emit navigateToUrl(QUrl(text()));
  } else {
    QLineEdit::keyPressEvent(key_event);
  }
}
