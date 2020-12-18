#ifndef TAR_H
#define TAR_H

#include <QFile>
#include <QFileInfo>

class Recored{
public:

//    name + mode + uid + gid + size + mtime + check + type+ link + ustar + owner + group + major + minor + prefix;
    //UStar format (POSIX IEEE P1003.1)
    QByteArray name;               //file name
    const int name_length = 100;
    const int name_index = 0;

    QByteArray mode;               //permissions
    const int mode_length = 8;
    const int mode_index = 100;

    QByteArray uid;                //user id(octal)
    const int uid_length = 10;
    const int uid_index = 108;

    QByteArray gid;                //group id(octal)
    const int gid_length = 10;
    const int gid_index = 118;

    QByteArray size;               //size(octal)
    const int size_length = 12;
    const int size_index = 128;

    QByteArray mtime;              //modification time(octal)
    const int mtime_length = 19;
    const int mtime_index = 140;

    QByteArray check;              //sum of unsigned character in block(octal)
    const int check_length = 8;
    const int check_index = 159;

//        char old[156];           //first 156 octets of Pre-POSIX.1-1988 format
    QByteArray type;               //file type
    const int type_length = 1;
    const int type_index = 167;

    QByteArray link;               //name of linked file
    const int link_length = 100;
    const int link_index = 168;

    QByteArray ustar;              //ustar\000
    const int ustar_length = 8;
    const int ustar_index = 268;

    QByteArray owner;              //user name (String)
    const int owner_length = 32;
    const int owner_index = 276;

    QByteArray group;              //group name (String)
    const int group_length = 32;
    const int group_index = 308;

    QByteArray major;              //device major number
    const int major_length = 8;
    const int major_index = 340;

    QByteArray minor;              //device minor number
    const int minor_length = 8;
    const int minor_index = 348;

    QByteArray prefix;
    const int prefix_length = 156;
    const int prefix_index = 356;

    Recored(QFileInfo info);
    Recored(QByteArray head);
    QByteArray getRecored();
    QChar setType(QFileInfo fileInfo);
    static void writeZeroRecored(QFile *file);
};

class Tar
{  
public:
    Tar();
    ~Tar();
    static QFile* tar(QFileInfoList list, bool isCreateFile = true);
    static void tar(QFileInfo fileInfo, QFile *write);
    static void writeRecored(Recored *recored, QFile *write);
    static void writeFile(QFileInfo fileInfo, QFile *write);

    static void untar(QFileInfo fileInfo);
    static void untar(QFile *read, QString path);
    static void setRecored(QFile*, Recored);
};


#endif // TAR_H
