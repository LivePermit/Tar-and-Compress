#include "tar.h"
#include "progress.h"
#include "mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QDateTime>
#include <QStack>
#include <QMessageBox>


QFile* Tar::tar(QFileInfoList list, bool isCreateFile)
{
    QString path = list[0].path();
    char count = '1';
    QFile *write;
    if (list.length() == 1){
        write = new QFile(path +"/" + list[0].baseName() + ".mytar");
        while (write->exists()) {
            write->setFileName(list[0].baseName() + "(" + count + ")" + ".mytar");
            count++;
        }
        if (isCreateFile)
            write->open(QIODevice::WriteOnly | QIODevice::Append);
        else
            write->open(QIODevice::Append);
        tar(list[0], write);
    } else {
        QString name = path.mid(path.lastIndexOf("/")+1);
        write = new QFile(path + "/" + name + ".mytar");
        path += "/";
        while (write->exists()) {
            write->setFileName(path + name + "(" + count + ")" + ".mytar");
            count++;
        }
        if (isCreateFile)
            write->open(QIODevice::WriteOnly | QIODevice::Append);
        else
            write->open(QIODevice::Append);
        for (QFileInfo info:list){
            tar(info, write);
        }
    }
    write->write(QByteArray(512,0));
    write->close();
    return write;
}

void Tar::tar(QFileInfo fileInfo, QFile *write)
{
    Recored *recored = new Recored(fileInfo);
    write->write(recored->getRecored());
    if (fileInfo.isDir()){
        QFileInfoList list = QDir(fileInfo.filePath()+"/").entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        for (QFileInfo info: list){
            tar(info, write);
        }
        write->write(QByteArray(512, 0));
    }else if (fileInfo.isSymLink()){

    }else if (fileInfo.isSymbolicLink()){

    }else {
        writeFile(fileInfo, write);
    }

}

void Tar::writeFile(QFileInfo fileInfo, QFile *write)
{
    QFile read(fileInfo.filePath());
    read.open(QIODevice::ReadOnly);
    QByteArray temp;
    while ((temp = read.read(512)).length() == 512) {
        write->write(temp);
    }
    if (temp.size() != 0){
        QByteArray end(512, 0);
        end.replace(0, temp.size(), temp);
        write->write(end);
    }
    read.close();
}

void Tar::untar(QFileInfo fileInfo)
{
    QFile *read = new QFile(fileInfo.filePath());
    untar(read, MainWindow::newDir(fileInfo));
}

void Tar::untar(QFile *read, QString path)
{
    QStack<QString> stack;
    QByteArray temp;
    read->open(QIODevice::ReadOnly);
    QString basePath = path;

    while((temp = read->read(512)).length() != 0){
        if (temp == QByteArray(512, 0)){
            if(!stack.isEmpty()) {
                basePath.chop(stack.pop().length()+1);
            }
            else return;
            continue;
        }
        Recored recored(temp);
        int s = recored.type.toInt();
        switch (s) {
        case 0:{
            int size = recored.size.toInt();
            int count = size / 512;
            QFile *file = new QFile(basePath + '/' + QString(recored.name));
            file->open(QIODevice::WriteOnly | QIODevice::Append);
            setRecored(file, recored);
            for (int i = 0; i < count; i++){
                file->write(read->read(512));
            }
            if (size % 512 != 0){
                file->write(read->read(512).mid(0, size % 512));
            }
            file->close();
            break;
        }
        case 1:{
            break;
        }
        case 2:{
            break;
        }
        case 3:{
            break;
        }
        case 4:{
            break;
        }
        case 5:{
            stack.push(recored.name);
            basePath += "/" + recored.name;
            QDir *dir = new QDir();
            dir->mkdir(basePath);
            break;
        }
        case 6:{
            break;
        }
        default:{
            QMessageBox::question(NULL, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("文件错误"));
            read->close();
            return;
        }
        }
    }
    read->close();
}

void Tar::setRecored(QFile *file, Recored recored)
{
//    ReadOwner = 0x4000, WriteOwner = 0x2000, ExeOwner = 0x1000,
//    ReadUser  = 0x0400, WriteUser  = 0x0200, ExeUser  = 0x0100,
//    ReadGroup = 0x0040, WriteGroup = 0x0020, ExeGroup = 0x0010,
//    ReadOther = 0x0004, WriteOther = 0x0002, ExeOther = 0x0001

//    QFlags(0x0002 | 0x0004 | 0x0020 | 0x0040 | 0x0200 | 0x0400 | 0x2000 | 0x4000)
    QFile::Permissions permission;
    int mode = recored.mode.toInt();
    for (QFile::Permission e = QFile::ReadOwner; e <= QFile::ExeOther; e = (QFile::Permission)(e >> 1)){
        if ((e & mode) == e){
            permission.setFlag(e, 1);
        }
    }
    file->setPermissions(permission);

//    file->setPermissions()
//  QUuid
//    gid

//    file->setFileTime(QDate)
//    link
//    user
//    group
//    major
//    minor
//    prefix
}

Recored::Recored(QFileInfo fileInfo)
{
    name.append(fileInfo.fileName());
    name.insert(name.length(), name_length - name.length(), 0);

    mode.append(QByteArray::number(fileInfo.permissions()));
    mode.insert(mode.length(), mode_length - mode.length(), 0);

    uid.append(QByteArray::number(fileInfo.ownerId()));
    uid.insert(uid.length(), uid_length - uid.length(), 0);

    gid.append(QByteArray::number(fileInfo.groupId()));
    gid.insert(gid.length(), gid_length - gid.length(), 0);

    size.append(QByteArray::number(fileInfo.size()));
    size.insert(size.length(), size_length - size.length(), 0);

    mtime.append(fileInfo.lastModified().toString("yyyy/MM/dd HH:mm:ss"));
    mtime.insert(mtime.length(), mtime_length - mtime.length(), 0);

//    check

//    check.append(QByteArray::number(512 - fileInfo.size() % 512));
    check.insert(check.length(), check_length - check.length(), 0);

//    type
    type.append(setType(fileInfo));
    type.insert(type.length(), type_length - type.length(), 0);

    if (fileInfo.isSymLink() || fileInfo.isSymbolicLink()){
        link.append(fileInfo.symLinkTarget());
    }
    link.insert(link.length(), link_length - link.length(), 0);

    ustar.append("ustar\0");
    ustar.insert(ustar.length(), ustar_length - ustar.length(), 0);

    owner.append(fileInfo.owner());
    owner.insert(owner.length(), owner_length - owner.length(), 0);

    group.append(fileInfo.group());
    group.insert(group.length(), group_length - group.length(), 0);


//    major.append("major");
    major.insert(major.length(), major_length - major.length(), 0);

//    minor.append("minor");
    minor.insert(minor.length(), minor_length - minor.length(), 0);

    prefix.append(fileInfo.path());
    prefix.insert(prefix.length(), prefix_length - prefix.length(), 0);
}

Recored::Recored(QByteArray head)
{
    name = head.mid(name_index, name_length);
    mode = head.mid(mode_index, mode_length);
    uid = head.mid(uid_index, uid_length);
    gid = head.mid(gid_index, gid_length);
    size = head.mid(size_index, size_length);
    mtime = head.mid(mtime_index, mtime_length);
    check = head.mid(check_index, check_length);
    type = head.mid(type_index, type_length);
    link = head.mid(link_index, link_length);
    ustar = head.mid(ustar_index, ustar_length);
    owner = head.mid(owner_index, owner_length);
    group = head.mid(group_index, group_length);
    major = head.mid(major_index, major_length);
    minor = head.mid(minor_index, minor_length);
    prefix = head.mid(prefix_index, prefix_length);
}

QByteArray Recored::getRecored()
{
    QByteArray a = name + mode + uid + gid + size + mtime + check + type
            + link + ustar + owner + group + major + minor + prefix;
    return a.insert(a.length(), 512 - a.length(), 0);
}

QChar Recored::setType(QFileInfo fileInfo)
{
    QChar c = 0;
    if (fileInfo.isDir()){
        c = '5';
    }else if (fileInfo.isFile()) {
        c = '0';
    }else if (fileInfo.isSymLink()) {
        c = '1';
    }else if (fileInfo.isSymbolicLink()) {
        c = '2';
    }
    return c;
}
