#include "init.h"
#include "mainwindow.h"
#include "tar.h"
#include "compress.h"

#include <QApplication>
#include <QBitArray>
#include <QDebug>
#include <QDir>
#include <QStack>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //    init i;
    //    i.show();
    MainWindow w;
    w.show();

    /*压缩系列*/
//    QFileInfo fileInfo(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件.mytar"));
//    QFile *file = new QFile(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件(1).xc"));
//    Compress::compress(fileInfo, file);

//    QFileInfo fileInfo(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件(1).xc"));
//    Compress::unCompress(fileInfo);



    /*打包系列*/

//    QFileInfo fileInfo(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件/少年闰土.txt"));
//    QFile *file = new QFile(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件/少年闰土copy.txt"));

//    QFileInfo fileInfo(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件"));
//    QFile *file = new QFile(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件.mytar"));
//    file->open(QIODevice::WriteOnly);
//    file->close();
//    file->open(QIODevice::Append);
//    Tar::tar(fileInfo, file);
//    file->close();

//    QFileInfo fileInfo(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件.mytar"));
//    Tar::untar(fileInfo);

//    QFile *file = new QFile(QString::fromLocal8Bit("C:/Users/28572/Desktop/学习/实验/大四上/软件开发实验/测试文件.mytar"));
//    file->open(QIODevice::ReadOnly);
//    QByteArray t = file->read(512);
//    t = file->read(512);
//    qDebug() << t.mid(0, 10);
//    file->close();

    qDebug() << "end";
    return a.exec();
}
