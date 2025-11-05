#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CefWidget;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

 protected:
  void showEvent(QShowEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void InitUI();

 protected slots:
  void ReallyClose();
  void OnBackButtonClicked();
  void OnForwardButtonClicked();
  void OnRefreshButtonClicked();
  void OnAddressBarEnterPressed(const QUrl& url);

 private:
  QWidget* centralWidget = nullptr;
  CefWidget* cef_widget_ = nullptr;
  bool can_close_ = false;
  QWidget* title_bar_ = nullptr;
  QWidget* buttons_container_ = nullptr;
};
#endif  // MAINWINDOW_H
