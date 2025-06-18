#pragma once

#include <QString>

/**
 * @brief Contains file utilities.
 */
class FileUtil {
public:
    /**
     * @brief Reads the file content.
     * @param path The file path.
     * @return The file content.
     */
    static QString readAll(const QString &path);

    /**
     * @brief Writes the specified text to a file.
     * @param path The file path.
     * @param text The text to be written.
     */
    static void writeAll(const QString &path, const QString &text);
};
