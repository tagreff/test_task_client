#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();
    setlocale(LC_ALL,"rus");
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));


    return a.exec();
}
