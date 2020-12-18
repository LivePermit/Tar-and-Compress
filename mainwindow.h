#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileInfo>
#include <QLineEdit>
#include <QLinkedList>
#include <QListWidget>
#include <QMainWindow>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static QString newDir(QFileInfo);
    static QString sizeToString(qint64 size, int digits);

private:
    Ui::MainWindow *ui;
    QLineEdit *lineEditPath;
    QMenu *menu;
    QAction *action_open;
    QAction *action_tar;
    QAction *action_compress;
    QAction *action_untar;
    QAction *action_uncompress;

    QFileInfoList fileList;
    int i = 0;
    QLinkedList<QString> *trace();

    void list();
    void iconList();
    void listList();
    void updateFileInfoList(QString path);

private slots:
    void slotIconTriggered();
    void slotListTriggered();
    void slotRefreshTriggered();
    void slotHigherTriggered();

    void slotListItemClicked(QListWidgetItem*);
    void slotListItemDoubleClicked(QModelIndex);
    void slotTableItemClicked(QTableWidgetItem*);
    void slotTableItemDoubleClicked(QModelIndex);

    void slotLineEditReturnPressed();
    void on_listWidget_customContextMenuRequested(const QPoint &pos);
    void on_tableWidget_customContextMenuRequested(const QPoint &pos);
    void slotOpen();
    void slotTar();
    void slotUntar();
    void slotCompress();
    void slotUncompress();
};
#endif // MAINWINDOW_H
