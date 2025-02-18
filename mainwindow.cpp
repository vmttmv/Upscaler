#include <QApplication>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImport_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter("Images (*.png *.jpg *.jpeg *.bmp)");
    if (dialog.exec()) {
        auto paths = dialog.selectedFiles();
        ui->label_source_image->setPixmap(QPixmap(paths[0]));
        ui->label_preview_image->clear();
        ui->pushButton_upscale->setEnabled(true);
        ui->actionExport->setEnabled(false);
    }
}

void MainWindow::on_actionExport_triggered()
{
    QFileDialog dialog(this);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter("Images (*.png *.jpg *.jpeg *.bmp)");
    if (dialog.exec()) {
        const QPixmap &pixmap = ui->label_preview_image->pixmap();
        auto paths = dialog.selectedFiles();
        pixmap.save(paths[0], nullptr, 100);
    }
}


void MainWindow::on_actionExit_triggered()
{
    QCoreApplication::quit();
}


void MainWindow::on_pushButton_upscale_clicked()
{
    QImage image = ui->label_source_image->pixmap().toImage();
    if (image.format() != QImage::Format_RGB888) {
        image.convertTo(QImage::Format_RGB888);
    }

    bool x16 = false;
    int width, height;
    if (ui->radioButton_scale_x4->isChecked()) {
        width = image.width() * 4;
        height = image.height() * 4;
    } else {
        width = image.width() * 16;
        height = image.height() * 16;
        x16 = true;
    }

    ui->label_preview_image->clear();
    qApp->processEvents();

    uint8_t *output = new uint8_t[width * height * 3];
    esrgan.upscale(output, image.constBits(), image.width(), image.height(), image.bytesPerLine(), x16);

    QImage img(output, width, height, QImage::Format_RGB888);
    ui->label_preview_image->setPixmap(QPixmap::fromImage(img));

    delete[] output;

    ui->actionExport->setEnabled(true);
}

