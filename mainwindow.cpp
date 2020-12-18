#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tar.h"
#include "compress.h"

#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //初始化ui
    QActionGroup *ag = new QActionGroup(this);
    ag->addAction(ui->action_icon);
    ag->addAction(ui->action_list);
    ui->action_list->setChecked(true);
    lineEditPath = new QLineEdit(this);
    lineEditPath->setContextMenuPolicy(Qt::NoContextMenu);
    ui->toolBar->addWidget(lineEditPath);

    QStringList header;
    header <<QString::fromLocal8Bit("名称") <<QString::fromLocal8Bit("修改日期") << QString::fromLocal8Bit("类型") << QString::fromLocal8Bit("大小");
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->setVisible(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //右键菜单选项
    menu = new QMenu(this);
    QFile menuQss(":/style/contextMenu.qss");
    menuQss.open(QIODevice::ReadOnly);
    menu->setStyleSheet(menuQss.readAll());
    menuQss.close();
    action_open = new QAction(QString::fromLocal8Bit("打开"), this);
    action_tar = new QAction(QIcon(":/icons/tar"), QString::fromLocal8Bit("打包"), this);
    action_untar = new QAction(QIcon(":/icons/untar"), QString::fromLocal8Bit("解包"), this);
    action_compress = new QAction(QIcon(":/icons/zip"), QString::fromLocal8Bit("压缩"), this);
    action_uncompress = new QAction(QIcon(":/icons/zip"), QString::fromLocal8Bit("解压"), this);

    //建立信号槽
    connect(ui->action_icon, SIGNAL(triggered()), this, SLOT(slotIconTriggered()));
    connect(ui->action_list, SIGNAL(triggered()), this, SLOT(slotListTriggered()));
    connect(ui->action_higher, SIGNAL(triggered()), this, SLOT(slotHigherTriggered()));
    connect(ui->action_refresh, SIGNAL(triggered()), this, SLOT(slotRefreshTriggered()));

    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(slotListItemClicked(QListWidgetItem*)));
    connect(ui->tableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(slotTableItemClicked(QTableWidgetItem*)));
    connect(ui->listWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotListItemDoubleClicked(QModelIndex)));
    connect(ui->tableWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotTableItemDoubleClicked(QModelIndex))); 

    connect(lineEditPath, SIGNAL(returnPressed()), this, SLOT(slotLineEditReturnPressed()));
    connect(action_open, SIGNAL(triggered()), this, SLOT(slotOpen()));
    connect(action_tar, SIGNAL(triggered()), this, SLOT(slotTar()));
    connect(action_untar, SIGNAL(triggered()), this, SLOT(slotUntar()));
    connect(action_compress, SIGNAL(triggered()), this, SLOT(slotCompress()));
    connect(action_uncompress, SIGNAL(triggered()), this, SLOT(slotUncompress()));

    //初始化程序
    QString path = QDir::currentPath();
    lineEditPath->setText(path);
    updateFileInfoList(path);
    list();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::list()
{
    if (ui->action_icon->isChecked()){
        iconList();
    }else{
        listList();
    }
}

void MainWindow::iconList()
{
    ui->tableWidget->setHidden(true);
    ui->listWidget->setHidden(false);
    ui->listWidget->clear();

    if (fileList.isEmpty()){
        QMessageBox::about(this, NULL, QString::fromLocal8Bit("该文件夹为空"));
    }
    for (QFileInfo info: fileList){
        if (info.isDir()){
            ui->listWidget->addItem(new QListWidgetItem(QIcon(":/icons/folder.ico"), info.fileName()));
        }else {
            QString type = info.suffix();
            if (type == "xc"){
                ui->listWidget->addItem(new QListWidgetItem(QIcon(":/icons/zip.ico"), info.fileName()));
            }else if(type == "mytar") {
                ui->listWidget->addItem(new QListWidgetItem(QIcon(":/icons/tar.ico"), info.fileName()));
            }else
                ui->listWidget->addItem(new QListWidgetItem(QIcon(":/icons/file.ico"), info.fileName()));
        }
    }
}

void MainWindow::listList()
{
    ui->tableWidget->setHidden(false);
    ui->listWidget->setHidden(true);
    ui->tableWidget->setRowCount(0);

    ui->tableWidget->setRowCount(fileList.size());
    if (fileList.isEmpty()){
        QMessageBox::about(this, NULL, QString::fromLocal8Bit("该文件夹为空"));
    }
    for (int i = 0;i < fileList.size(); i++){
        QFileInfo info = fileList.at(i);
        QString name = info.fileName(), modTime = info.lastModified().toString("yyyy/MM/dd HH:mm:ss"), type, size;
        QIcon *icon;
        if (info.isDir()){
            icon = new QIcon(":/icons/folder.ico");
            type = QString::fromLocal8Bit("文件夹");
        }else{
            type = info.suffix();
            if (type == "xc")
                icon = new QIcon(":/icons/zip.ico");
            else if (type == "mytar")
                icon = new QIcon(":/icons/tar.ico");
            else
                icon = new QIcon(":/icons/file.ico");
            type += QString::fromLocal8Bit("文件");
            size = sizeToString(info.size(), 2);
        }
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(*icon, name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(modTime));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(type));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(size));
    }
}

void MainWindow::slotIconTriggered()
{
    if (ui->listWidget->isHidden()){
        iconList();
    }
}

void MainWindow::slotListTriggered()
{
    if (ui->tableWidget->isHidden()){
        listList();
    }
}

void MainWindow::slotRefreshTriggered()
{
    updateFileInfoList(lineEditPath->text()+"/");
    list();
}

void MainWindow::slotHigherTriggered()
{
    QDir *dir = new QDir(lineEditPath->text());
    dir->cdUp();
    lineEditPath->setText(dir->path());
    updateFileInfoList(dir->path());
    list();
}

void MainWindow::slotListItemClicked(QListWidgetItem *item)
{
    Q_UNUSED(item);
    //从fileList中获取选择文件信息
    QFileInfo info = fileList.at(ui->listWidget->currentRow());
    QString type;
    if (info.isDir()) type = QString::fromLocal8Bit("文件夹");
    else type = info.suffix() + QString::fromLocal8Bit("文件");
    QString message = QString::fromLocal8Bit("文件类型:") + type + QString::fromLocal8Bit("   文件名:") + info.fileName() +
                      QString::fromLocal8Bit("   文件大小:") + sizeToString(info.size(), 2) +
                      QString::fromLocal8Bit("   修改时间:") + info.lastModified().toString("yyyy-MM-dd hh:mm:ss");
//                    QString::fromLocal8Bit("   访问时间:") + info.lastRead().toString("yyyy-MM-dd hh:mm:ss");
    ui->statusbar->showMessage(message);
}

void MainWindow::slotListItemDoubleClicked(QModelIndex index)
{
    Q_UNUSED(index);
    //从fileList中获取选择文件信息
    QFileInfo info = fileList.at(ui->listWidget->currentRow());
    if (info.isDir()){
        QString path = info.path() + "/" + info.fileName();
        updateFileInfoList(path);
        lineEditPath->setText(path);
        list();
    }else{
        QMessageBox::about(this, QString::fromLocal8Bit("抱歉"), QString::fromLocal8Bit("本软件不支持打开文件功能"));
    }
}

void MainWindow::slotTableItemClicked(QTableWidgetItem *item)
{
    Q_UNUSED(item);
    //从fileList中获取选择文件信息
    QFileInfo info = fileList.at(ui->tableWidget->currentRow());
    QString type;
    if (info.isDir()) type = QString::fromLocal8Bit("文件夹");
    else type = info.suffix() + QString::fromLocal8Bit("文件");
    QString message = QString::fromLocal8Bit("文件类型:") + type + QString::fromLocal8Bit("   文件名:") + info.fileName() +
                      QString::fromLocal8Bit("   文件大小:") + sizeToString(info.size(), 2) +
                      QString::fromLocal8Bit("   修改时间:") + info.lastModified().toString("yyyy-MM-dd hh:mm:ss");
//                    QString::fromLocal8Bit("   访问时间:") + info.lastRead().toString("yyyy-MM-dd hh:mm:ss");
    ui->statusbar->showMessage(message);
}

void MainWindow::slotTableItemDoubleClicked(QModelIndex index)
{
    Q_UNUSED(index);
    //从fileList中获取选择文件信息
    QFileInfo info = fileList.at(ui->tableWidget->currentRow());
    if (info.isDir()){
        QString path = info.path() + "/" + info.fileName();
        updateFileInfoList(path);
        lineEditPath->setText(path);
        list();
    }else{
        QMessageBox::about(this, QString::fromLocal8Bit("抱歉"), QString::fromLocal8Bit("本软件不支持打开文件功能"));
    }
}


void MainWindow::slotLineEditReturnPressed()
{
    QString path(lineEditPath->text() + "/");
    if (QFile::exists(path)){
        QFileInfo *info = new QFileInfo(path);
        if (info->isDir()){
            updateFileInfoList(info->path());
            list();
        }else{
            QMessageBox::about(this, QString::fromLocal8Bit("抱歉"), QString::fromLocal8Bit("本软件不支持打开文件功能"));
        }
    }else{
        QMessageBox::question(this, QString::fromLocal8Bit("文件不存在"), QString::fromLocal8Bit("找不到") + lineEditPath->text() + QString::fromLocal8Bit(",请检查拼写后重试"));
    }
}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    menu->clear();
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    if (items.length() == 0)
        return;
    QFileInfo info = fileList.at(ui->listWidget->currentRow());
    if (items.length() == 1){
        menu->addAction(action_open);
        if (info.suffix() == "mytar")
            menu->addAction(action_untar);
        else
            menu->addAction(action_tar);
        if (info.suffix() == "xc")
            menu->addAction(action_uncompress);
        else
            menu->addAction(action_compress);
    }else {
        menu->addAction(action_tar);
        menu->addAction(action_compress);
    }
    menu->exec(QCursor::pos());
}

void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos);
    menu->clear();
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
    if (items.length() == 0)
        return;
    QFileInfo info = fileList.at(ui->tableWidget->currentRow());
    if (items.length() == 4){
        menu->addAction(action_open);
        if (info.suffix() == "mytar")
            menu->addAction(action_untar);
        else
            menu->addAction(action_tar);
        if (info.suffix() == "xc")
            menu->addAction(action_uncompress);
        else
            menu->addAction(action_compress);
    }else {
        menu->addAction(action_tar);
        menu->addAction(action_compress);
    }
    menu->exec(QCursor::pos());
}

void MainWindow::slotOpen()
{
    if (ui->listWidget->isHidden())
        slotTableItemDoubleClicked(QModelIndex());
    else
        slotListItemDoubleClicked(QModelIndex());
}

void MainWindow::slotTar()
{
    qDebug() <<"tar";
    QModelIndexList modelList;
    QFileInfoList list;
    if (ui->action_icon->isChecked()){
        modelList = ui->listWidget->selectionModel()->selectedIndexes();
    }else {
        modelList = ui->tableWidget->selectionModel()->selectedIndexes();
    }
    QVector<int> indexs;
    for (int i = 0;i < modelList.count();i+=4){
        indexs.push_back(modelList.at(i).row());
    }
    for (int i:indexs){
        list.push_back(fileList[i]);
    }
    Tar::tar(list);
    updateFileInfoList(lineEditPath->text()+"/");
    this->list();
}

void MainWindow::slotUntar()
{
    qDebug() <<"untar";
    QFileInfo info = (ui->listWidget->isHidden()) ? fileList.at(ui->tableWidget->currentRow()) :fileList.at(ui->listWidget->currentRow());;
    Tar::untar(info);
    updateFileInfoList(info.path()+"/");
    list();
}

void MainWindow::slotCompress()
{
    qDebug() <<"compress";
    QModelIndexList modelList;
    QFileInfoList list;
    if (ui->action_icon->isChecked()){
        modelList = ui->listWidget->selectionModel()->selectedIndexes();
    }else {
        modelList = ui->tableWidget->selectionModel()->selectedIndexes();
    }
    QVector<int> indexs;
    for (int i = 0;i < modelList.count();i+=4){
        indexs.push_back(modelList.at(i).row());
    }
    for (int i:indexs){
        list.push_back(fileList[i]);
    }
    Compress::compress(list);
    updateFileInfoList(lineEditPath->text()+"/");
    this->list();
}

void MainWindow::slotUncompress()
{
    qDebug() <<"uncompress";
    QFileInfo info = (ui->listWidget->isHidden()) ? fileList.at(ui->tableWidget->currentRow()) :fileList.at(ui->listWidget->currentRow());
    Compress::unCompress(info);
    updateFileInfoList(info.path()+"/");
    list();
}

QString MainWindow::newDir(QFileInfo fileInfo)
{
    char count = '1';
    QString path = fileInfo.path() + "/" + fileInfo.baseName();
    QDir dir(path);
    while (dir.exists()){
        dir.setPath(path + "(" + count + ")");
        count++;
    }
    dir.mkdir(dir.path());
    return dir.path();
}

void MainWindow::updateFileInfoList(QString path)
{
    QDir *dir = new QDir(path);
    dir->setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir->setSorting(QDir::Name | QDir::DirsFirst);
    fileList = dir->entryInfoList();
}

QString MainWindow::sizeToString(qint64 b, int digits)
{
    QString s = "";
    if (b > 999999999) {
        s = QString::number(b/(1024*1024*1024.0), 'f', digits) + " GB";
    } else {
        if (b > 999999){
            s = QString::number(b/(1024*1024.0), 'f', digits) + " MB";
        } else {
            if (b > 999) {
                s = QString::number(b/1024.0, 'f', digits) + " KB";
            } else {
                s = QString::number(b)+" B";
            }
        }
    }
    return s;
}
