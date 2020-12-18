#include "compress.h"
#include "tar.h"
#include "mainwindow.h"

#include <QMap>
#include <QDebug>
#include <QBitArray>



TreeNode::TreeNode(QByteArray byte, int freq)
{
    this->byte = byte;
    this->freq = freq;
}

void Compress::compress(QFileInfoList list)
{
    QFile *read = Tar::tar(list, false);
    QString name = read->fileName();
    name.chop(6);
    QFile *write = new QFile(name+".xc");
    compress(read, write);
    read->remove();
}

void Compress::compress(QFile *read, QFile *write)
{
    //读取压缩文件字符
//    qDebug() << "read bytes";
    read->open(QIODevice::ReadOnly);
    QMap<QByteArray, int> map;    
    QByteArray t;
    qDebug() <<"---------";
    while ((t = read->read(1)).length() != 0) {
        if (map.contains(t)) map[t]++;
        else map[t] = 1;
    }
    read->close();


    //生成有序队列
//    qDebug() <<"create queue";
    TreeNode *head = new TreeNode("0" , 0), *cur = head;
    head->next = NULL;
    QMap<QByteArray, int>::Iterator i;
    for (i = map.begin(); i != map.end(); i++){
        TreeNode *node = new TreeNode(i.key(), i.value());
        node->left = NULL;
        node->right = NULL;
        while (cur->next != NULL && cur->next->freq < node->freq) {
            cur = cur->next;
        }
        node->next = cur->next;
        cur->next = node;
        cur = head;
    }

    //构造huffman树
//    qDebug() <<"create huffman tree";
    cur = head;
    TreeNode *node1, *node2;
    while (head->next->next != NULL) {
        node1 = head->next;
        node2 = head->next->next;
        TreeNode *node = new TreeNode("", node1->freq + node2->freq);
        node->left = node1;
        node->right = node2;
        head->next = node2->next;
        cur = head;
        while (cur->next != NULL && cur->next->freq < node->freq) {
            cur = cur->next;
        }
        node->next = cur->next;
        cur->next = node;
    }
    makeHuffcode(head->next, "");

    //按编码写入文件
//    qDebug() << "write file";
    write->open(QIODevice::Append);
    QMap<QByteArray, QString> huffMap;
    getHuffcodeMap(head->next, &huffMap);
    write->write(QByteArray::number(read->size())+ " " + QByteArray::number(huffMap.size()) + "\n");          //写入文件大小与表的大小，用于验证解压
    for (QMap<QByteArray, QString>::Iterator i = huffMap.begin(); i != huffMap.end();i++){                      //写入编码表
        write->write(QByteArray().append(i.value()+" "));
        write->write(i.key());
        write->write(" \n");
    }
    read->open(QIODevice::ReadOnly);
    char tempchar = 0;
    int l = 8;
    QByteArray temp = "";
    while ((t = read->read(1)).length() != 0) {
        QString code = huffMap[t];
        for (int i = 0;i < code.size(); i++){
            l--;
            if (code[i] == '0')
                tempchar = tempchar | (0 << l);
            else
                tempchar = tempchar | (1 << l);
            if (l == 0){
                l = 8;
                temp += tempchar;
                tempchar = 0;
            }
        }
        if (temp.size() >= 1024){
            write->write(temp);
            temp.clear();
        }
    }
    if (l != 8) temp += tempchar;
    write->write(temp);
    read->close();
    write->close();
}

void Compress::unCompress(QFileInfo fileInfo)
{
    QFile *read = new QFile(fileInfo.filePath());
    QFile *write = new QFile(fileInfo.path() + "/" + fileInfo.baseName() + ".temp");
    unCompress(read, write);
    Tar::untar(write, MainWindow::newDir(fileInfo));
    write->remove();
}

void Compress::unCompress(QFile *read, QFile *write)
{
    read->open(QIODevice::ReadOnly);
    write->open(QIODevice::Append);

    //读取huffman表
    QMap<QString, QByteArray> map;
    QStringList list = QString(read->readLine()).split(" ");
    int mapSize = list[1].toInt();
    for (int i = 0;i < mapSize; i++){
//        QByteArray item = read.readLine();
        QList<QByteArray> item = read->readLine().split(' ');
        if (item.length() == 2){        //特殊字符换行和换页，会多生成一行空白
            read->readLine();
        }
        if (item.length() == 4){        //特殊字符空格，与分隔符相同，
            map.insert(item[0], " ");
            continue;
        }
        if (item[1] == ""){
            map.insert(item[0], QByteArray(1, 0));
            continue;
        }
        map.insert(item[0], item[1]);
    }

    //读取压缩文件
    QByteArray t;
    QString code;
    while ((t = read->read(1)).length() != 0) {
        for (int i = 7; i >= 0;i--){
            code += (t.at(0) & (1 << i)) ? "1" : "0";
            if (map.contains(code)){
                write->write(map[code]);
                code = "";
            }
        }
    }
    read->close();
    write->close();
}

void Compress::makeHuffcode(TreeNode *root, QString code)
{
    if (root == NULL) return;
    root->huffCode = code;
    makeHuffcode(root->left, code + "0");
    makeHuffcode(root->right, code + "1");
}

void Compress::getHuffcodeMap(TreeNode* root, QMap<QByteArray, QString>* map)
{
    if (root == NULL) return;
    if (root->left == NULL && root->right == NULL) map->insert(root->byte, root->huffCode);
    getHuffcodeMap(root->left, map);
    getHuffcodeMap(root->right, map);
}

void Compress::printTree(TreeNode *root)
{
    if (root == NULL) return;
    if (root->left == NULL && root->right == NULL)  qDebug() << "value:" + QString(root->byte) << "freq:" << root->freq << "code:" << root->huffCode;
    printTree(root->left);
    printTree(root->right);
}







