#pragma once

#include <QIcon>
#include <QString>
#include <QMap>

/**
 * @brief Contains icon utilities.
 */
class IconUtil {
public:
    /**
     * @brief Loads the icon with the specified path.
     * @note This method lowers the memory consumption as
     * it prevents the duplication of identical icons.
     * @param path The icon file path.
     * @return The icon at the specified path.
     */
    static const QIcon &load(const QString &path);

private:
    static QMap<QString, QIcon> cache;
};
