/*
 * @Author: lenomirei lenomirei@163.com
 * @Date: 2025-11-05 11:59:18
 * @LastEditors: lenomirei lenomirei@163.com
 * @LastEditTime: 2025-11-05 12:16:45
 * @FilePath: \qtcefwindowless\AddressBar.h
 * @Description: custom address bar
 */
#pragma once

#include <QKeyEvent>
#include <QLineEdit>

class AddressBar : public QLineEdit {
  Q_OBJECT

 public:
  explicit AddressBar(QWidget* parent = nullptr);

 protected:
  void keyPressEvent(QKeyEvent* key_event) override;

 signals:
  void navigateToUrl(const QUrl& url);

 signals:
};
