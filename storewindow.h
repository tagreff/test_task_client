#ifndef STOREWINDOW_H
#define STOREWINDOW_H

#include <QDialog>
#include <QDataStream>
#include <QTcpSocket>
#include <QListWidget>
#include <QDomNodeList>

class QTcpSocket;
class QNetworkSession;

namespace Ui {
class StoreWindow;
}

class StoreWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StoreWindow(QWidget *parent = nullptr);
    ~StoreWindow();

public slots:
     void onReadyRead();

signals:
    void storeWindowClosed();
//    void readyRead();

private slots:
    void on_StoreWindow_rejected();
    void displayError(QAbstractSocket::SocketError socketError);
    void on_fileOpenButton_clicked();
    void on_sendRequestButton_clicked();
    void highlightChecked(QListWidgetItem* item);

private:
     Ui::StoreWindow *ui;
     QString address;
     QTcpSocket *tcpSocket = nullptr;
     QList<QString> pickedItems;
     QListWidget *listWidget;
     QListWidgetItem* itemsList;
     QDomNodeList items;

};

#endif // STOREWINDOW_H
