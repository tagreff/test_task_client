#include "storewindow.h"
#include "ui_storewindow.h"
#include <QtXml>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QtWidgets>
#include <QtNetwork>
#include <QString>

StoreWindow::StoreWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StoreWindow)
{
    ui->setupUi(this);
    ui->sendRequestButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
    fileLoadState = false;



    StoreWindow::getConnectionAttributes();// Получаем адрес и порт из xml файла

    tcpSocket = new QTcpSocket(); // создаем новый экземпляр сокета
    connect(tcpSocket, &QIODevice::readyRead, this, &StoreWindow::onReadyRead); //сигнал на получение сообщения от сервера
    connect(tcpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), //сигнал для обработки ошибки подключения
                this, &StoreWindow::displayError);
}

StoreWindow::~StoreWindow()
{
    delete ui;
}

void StoreWindow::on_StoreWindow_rejected() //
{
    emit storeWindowClosed();
}

void StoreWindow::getConnectionAttributes() // Получаем адрес и порт из xml файла
{
    QDomDocument attrsFile;
    QFile attrs(qApp->applicationDirPath() + "/attributes.xml"); //открываем файл с портом и адресом
    attrsFile.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\""); // задаем инструкции для обработки
    attrs.open(QIODevice::ReadOnly);
    attrsFile.setContent(&attrs);
    QDomNodeList fileAttributes = attrsFile.elementsByTagName("attributes"); //получаем родительские элемент адреса и порта
    QDomNode attribute = fileAttributes.item(0); //задаем переменную QDomNode значением первого элемента
    address = attribute.firstChildElement("ip").text(); //получаем адрес
    port = attribute.firstChildElement("port").text().toUShort();//получаем порт
    connectionState = false;
}

void StoreWindow::on_fileOpenButton_clicked()
{
    QString filename =  QFileDialog::getOpenFileName( //открываем файловый диалог
            this,
            "Выберите файл с товарами",
            QDir::currentPath(),
            "XML files (*.xml)");

  if( !filename.isNull() )
  {
        QDomDocument itemsFile;

        QFile file(filename);//записываем имя выбранного файла
        itemsFile.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
        if (!file.open(QIODevice::ReadOnly )) //обрабатываем ошибку
        {
        QMessageBox Msgbox;
          Msgbox.setText("Ошибка при открытии файла!");
          Msgbox.exec();
        }

        itemsFile.setContent(&file);

        items = itemsFile.elementsByTagName("item");


        QString out;
        ui->listWidget->clear();
        StoreWindow::clearPickedItems();
        for (int i = 0; i<items.size(); i++)//обрабатываем xml файл
        {
            QDomNode item = items.item(i);
            out = item.firstChildElement("name").text();
            itemsList = new QListWidgetItem(out, ui->listWidget);
            itemsList->setFlags(itemsList->flags() | Qt::ItemIsUserCheckable);
            itemsList->setCheckState(Qt::Unchecked);
        }

        if(!fileLoadState) //костыль, но я не знаю, как по другому исправить эту ошибку
        {
            QObject::connect(ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), //создаем connection для элементов списка
                                 this, SLOT(highlightChecked(QListWidgetItem*)));
            fileLoadState = true;
        }

        file.close();

        ui->sendRequestButton->setEnabled(true); //разблокируем кнопки
        ui->resetButton->setEnabled(true);
    }
}

void StoreWindow::highlightChecked(QListWidgetItem *item){
    if(item->checkState() == Qt::Checked)
    {
        pickedItems.append(item->text());// добавляем выбранные элементы в QList
        qDebug() << item->text();
    }
    else{
         pickedItems.removeOne(item->text());
    }
}

void StoreWindow::onReadyRead()
{
    QByteArray response = tcpSocket->readAll(); //считываем данные из сокета
    if(response == "true"){
        QMessageBox::information(this, tr("Результат запроса"), tr("Запрос принят"));
        ui->resetButton->setEnabled(true);
    }
    else if(response == "false") {
        QMessageBox::information(this, tr("Результат запроса"), tr("Запрос отклонен"));
        ui->sendRequestButton->setEnabled(true);
    } else{
        qDebug() << response;
    }
}

void StoreWindow::displayError(QAbstractSocket::SocketError socketError) //обрабатываем возможные исключения при подключении к серверу
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




void StoreWindow::on_sendRequestButton_clicked() //отправляем запрос на сервер
{
    if(!connectionState)
    {
        tcpSocket->connectToHost(QHostAddress(address), port);
        connectionState = true;
    }
    QByteArray output;
    int num = pickedItems.length();
    for(int i =0; i< num; i++)
    {
       output.append(pickedItems.at(i) + ";");
    }
    tcpSocket->write(QByteArray(output)); //записываем строку в QByteArray
    output.clear();
    ui->sendRequestButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
}

void StoreWindow::on_resetButton_clicked() //ресет списка и кнопок
{
    ui->listWidget->clear();
    StoreWindow::clearPickedItems();
    ui->sendRequestButton->setEnabled(false);
    ui->resetButton->setEnabled(false);
}

void StoreWindow::clearPickedItems()
{
    int num = pickedItems.length();
    for(int i =0; i< num; i++)
    {
        pickedItems.removeAt(i);
    }
    pickedItems.clear();
}
