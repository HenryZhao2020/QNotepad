#pragma once

#include <QString>
#include <QList>
#include <QMap>
#include <QLocale>

/**
 * @brief Supported languages.
 */
enum class Lang {
    ENGLISH,
    CHINESE_SIMPLIFIED,
    CHINESE_TRADITIONAL,
};

/**
 * @brief Utility functions for Lang enum.
 */
class LangUtil {
public:
    /// Returns the display name of the given language (e.g., "English", "简体中文").
    static QString getLangName(Lang lang);

    /// Returns the language code (e.g., "zh-CN").
    static QString getLangCode(Lang lang);

    /// Returns the list of all supported languages.
    static const QList<Lang> &getLanguages();

private:
    static const QMap<Lang, QLocale> localeMap;
};
