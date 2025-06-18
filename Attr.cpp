#include "Attr.h"

void Attr::save() {
    QFile file{"QNotepad_Data"};
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        return;
    }

    QDataStream out{&file};
    out << recentDir << recentPaths << findTarget << replaceTarget
        << matchCase << matchWholeWord << showLine << showStatus
        << wordWrap << zoom << editorFont << lang;
    file.close();
}

bool Attr::load() {
    QFile file{"QNotepad_Data"};
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return false;
    }

    QDataStream in{&file};
    in >> recentDir >> recentPaths >> findTarget >> replaceTarget
       >> matchCase >> matchWholeWord >> showLine >> showStatus
       >> wordWrap >> zoom >> editorFont >> lang;
    file.close();
    return true;
}

Attr &Attr::get() {
    static Attr attr;
    return attr;
}
