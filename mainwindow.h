#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "storewindow.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    void unblockStoreButton();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_openStoreWindowButton_clicked();

private:
    Ui::MainWindow *ui;
    StoreWindow *storeWindow;

};

#endif // MAINWINDOW_H
