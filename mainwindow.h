#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "esrgan.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionImport_triggered();

    void on_actionExport_triggered();

    void on_actionExit_triggered();

    void on_pushButton_upscale_clicked();

private:
    Ui::MainWindow *ui;
    esrgan esrgan;
};
#endif // MAINWINDOW_H
