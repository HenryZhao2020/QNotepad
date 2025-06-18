#pragma once

#include "Lang.h"

#include <QDir>
#include <QFont>

/**
 * @brief Contains shared attributes for saving and loading program state.
 */
class Attr {
public:
    /// Directory that the most recent file is opened or saved to.
    QString recentDir{QDir::homePath()};
    /// Paths of recently opened files.
    QStringList recentPaths;
    /// Text snippet to be searched.
    QString findTarget;
    /// Text snippet to be replaced.
    QString replaceTarget;
    /// Whether to match case upon searching.
    bool matchCase{false};
    /// Whether to match whole word upon searching.
    bool matchWholeWord{false};
    /// Whether to show line numbers on the left side of the editor.
    bool showLine{true};
    /// Whether to show the status bar on the bottom.
    bool showStatus{true};
    /// Whether to wrap words when the editor resizes.
    bool wordWrap{false};
    /// Zoom percentage of the editor font size.
    int zoom{100};
    /// Editor font.
    QFont editorFont{{"Cascadia Code", "Microsoft Yahei UI"}, 12};
    /// Display language.
    Lang lang{Lang::ENGLISH};

    /**
     * @brief Saves all attributes to the program folder.
     */
    void save();
    /**
     * @brief Loads all attributes from the program folder.
     * @return true if successful; false otherwise.
     */
    bool load();

    /**
     * @brief Provides access to the singleton instance.
     * @return The singleton instance.
     */
    static Attr &get();

private:
    // Private constructor to prevent external instantiation
    Attr() = default;
};
