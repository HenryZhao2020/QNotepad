#include "FileUtil.h"

#include <QFile>

QString FileUtil::readAll(const QString &path) {
    QFile file{path};

    // If the file fails to open, return an empty string
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return "";
    }

    // Read file content
    QString content{file.readAll()};
    file.close();

    return content;
}

void FileUtil::writeAll(const QString &path, const QString &text) {
    QFile file{path};

    // If the file fails to open, exit the function
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    // Write to file
    file.write(text.toUtf8());
    file.close();
}
