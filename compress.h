#ifndef COMPRESS_H
#define COMPRESS_H

#include <QByteArray>
#include <QString>
#include <QChar>
#include <QFileInfo>
#include <QBitArray>
#include <QMap>

class TreeNode
{
public:
    QByteArray byte;
    int freq = 0;
    QString huffCode;

    TreeNode *left;
    TreeNode *right;
    TreeNode *next;

    TreeNode(QByteArray byte, int freq);
    ~TreeNode(){}
};

class Compress
{
public:
    static void compress(QFileInfoList);
    static void compress(QFile*, QFile*);
    static void makeHuffcode(TreeNode *root, QString);
    static void getHuffcodeMap(TreeNode*, QMap<QByteArray, QString>*);
    static void printTree(TreeNode *root);
    static void unCompress(QFileInfo);
    static void unCompress(QFile*, QFile*);

};



#endif // COMPRESS_H
