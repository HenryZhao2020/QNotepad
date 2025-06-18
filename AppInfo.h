#pragma once

#include <QString>
#include <QIcon>

/**
 * @brief Stores metadata about the application.
 */
class AppInfo {
public:
    static const QString &name();
    static const QIcon &icon();
    static const QString &version();
    static const QString &developer();
    static const QString &description();
    static const QString &github();

private:
    AppInfo() = delete;     // Prevent instantiation
};
