#pragma once

#include <QWidget>
#include <QPushButton>

class AddressBar;
class TitleBar : public QWidget {
  Q_OBJECT

 public:
  explicit TitleBar(QWidget* parent = nullptr);
  void InitUI();

  void SetBackButtonDisabled(bool disabled);
  void SetForwardButtonDisabled(bool disabled);
  void SetReloadButtonLoading(bool loading);
  void UpdateAddressBarText(const QString& url);

 signals:
  void backButtonClicked();
  void forwardButtonClicked();
  void reloadButtonClicked();
  void stopLoadingButtonClicked();
  void addressBarEnterPressed(const QUrl& url);

 protected slots:
  void OnReloadButtonClicked();

 protected:
  AddressBar* address_bar_ = nullptr;
  QPushButton* back_button_ = nullptr;
  QPushButton* forward_button_ = nullptr;
  QPushButton* reload_button_ = nullptr;
};
