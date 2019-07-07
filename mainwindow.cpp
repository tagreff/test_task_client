#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <QtNetwork>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_openStoreWindowButton_clicked()
{
    storeWindow = new StoreWindow(this);
    QObject::connect(storeWindow, SIGNAL(storeWindowClosed()), this, SLOT(unblockStoreButton()));
    storeWindow->show();
    ui->openStoreWindowButton->setEnabled(false);
}


void MainWindow::unblockStoreButton()
{
    ui->openStoreWindowButton->setEnabled(true);
}


