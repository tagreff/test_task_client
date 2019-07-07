#include "storewindow.h"
#include "ui_storewindow.h"
#include <QtXml>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>
#include <QtNetwork>

StoreWindow::StoreWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StoreWindow)
{
    ui->setupUi(this);
    tcpSocket = new QTcpSocket();
    connect(tcpSocket, &QIODevice::readyRead, this, &StoreWindow::onReadyRead);
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
                this, &StoreWindow::displayError);
}

StoreWindow::~StoreWindow()
{
    delete ui;
}

void StoreWindow::on_StoreWindow_rejected()
{
    emit storeWindowClosed();
}

void StoreWindow::on_fileOpenButton_clicked()
{
    QString filename =  QFileDialog::getOpenFileName(
            this,
            "Выберите файл с товарами",
            QDir::currentPath(),
            "XML files (*.xml)");

  if( !filename.isNull() )
  {
        QDomDocument itemsFile;

        QFile file(filename);
        itemsFile.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
        if (!file.open(QIODevice::ReadOnly ))
        {
        QMessageBox Msgbox;
          Msgbox.setText("Ошибка при открытии файла!");
          Msgbox.exec();
        }

        itemsFile.setContent(&file);

        items = itemsFile.elementsByTagName("item");

        QString out;
        for (int i = 0; i<items.size(); i++)
        {
            QDomNode item = items.item(i);
            out = item.firstChildElement("name").text();
            itemsList = new QListWidgetItem(out, ui->listWidget);
            itemsList->setFlags(itemsList->flags() | Qt::ItemIsUserCheckable);
            itemsList->setCheckState(Qt::Unchecked);
        }

        QObject::connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)),
                             this, SLOT(highlightChecked(QListWidgetItem*)));

        file.close();
    }
}

void StoreWindow::highlightChecked(QListWidgetItem *item){
    if(item->checkState() == Qt::Checked)
    {
        pickedItems.append(item->text());

    }
    else{
         pickedItems.removeOne(item->text());
    }
}

void StoreWindow::onReadyRead()
{
    QByteArray response = tcpSocket->readAll();
    if(response == "true"){
        QMessageBox::information(this, tr("Результат запроса"), tr("Запрос принят"));
    }
    else if(response == "false") {
        QMessageBox::information(this, tr("Результат запроса"), tr("Запрос отклонен"));
    }
}

void StoreWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }
}




void StoreWindow::on_sendRequestButton_clicked()
{
    tcpSocket->connectToHost(QHostAddress("127.0.0.1"), 5000);
    QByteArray output;

    // inputArray - your QStringList
    foreach (const QString &str, pickedItems)
    {
        output.append(str + "  ");
    }
    tcpSocket->write(QByteArray(output));
}
