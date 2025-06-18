#include "AppInfo.h"
#include "IconUtil.h"

#include <QObject>

const QString &AppInfo::name() {
    static const QString name{QObject::tr("QNotepad")};
    return name;
}

const QIcon &AppInfo::icon() {
    static const QIcon icon{IconUtil::load(":/icons/QNotepad_Logo.png")};
    return icon;
}

const QString &AppInfo::version() {
    static const QString ver{"2.1.0"};
    return ver;
}

const QString &AppInfo::developer() {
    static const QString dev{"Henry Zhao"};
    return dev;
}

const QString &AppInfo::description() {
    static const QString desc;
    return desc;
}

const QString &AppInfo::github() {
    static const QString git{"https://github.com/HenryZhao2020/QNotepad"};
    return git;
}
