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

private slots:
    void OnClicked();

private:
    Ui::MainWindow *ui;

    QSharedPointer<CefWidget> cef_widget_ = nullptr;
};
#endif // MAINWINDOW_H
