#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CefWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

protected slots:
    void ReallyClose();

private:
    Ui::MainWindow *ui;
    CefWidget* cef_widget_ = nullptr;
    bool can_close_ = false;
};
#endif // MAINWINDOW_H
