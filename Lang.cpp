#include "Lang.h"

const QMap<Lang, QLocale> LangUtil::localeMap{
    {Lang::ENGLISH,             QLocale{QLocale::English, QLocale::World}},
    {Lang::CHINESE_SIMPLIFIED,  QLocale{QLocale::Chinese, QLocale::SimplifiedChineseScript}},
    {Lang::CHINESE_TRADITIONAL, QLocale{QLocale::Chinese, QLocale::TraditionalChineseScript}},
};

QString LangUtil::getLangName(Lang lang) {
    return localeMap.value(lang).nativeLanguageName();
}

QString LangUtil::getLangCode(Lang lang) {
    return localeMap.value(lang).name();
}

const QList<Lang> &LangUtil::getLanguages() {
    static const QList<Lang> langs{localeMap.keys()};
    return langs;
}
