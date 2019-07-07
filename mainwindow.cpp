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

//Метод для блокирование кнопки, открывающей окно торговой точки, если такое окно уже существует
void MainWindow::on_openStoreWindowButton_clicked()
{
    storeWindow = new StoreWindow(this);
    QObject::connect(storeWindow, SIGNAL(storeWindowClosed()), this, SLOT(unblockStoreButton())); // при сигнале закрытия окна вызываем слот
    storeWindow->show();
    ui->openStoreWindowButton->setEnabled(false);
}

// Метод разблокиует кнопку при закрытии окна торговой очки
void MainWindow::unblockStoreButton()
{
    ui->openStoreWindowButton->setEnabled(true);
}


