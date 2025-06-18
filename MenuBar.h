#pragma once

#include <QMenuBar>
#include <QAction>

// Forward declarations
class MainWindow;
class Editor;

/**
 * @brief Displays menus and actions.
 */
class MenuBar : public QMenuBar {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'MenuBar' instance.
     * @param win The parent 'MainWindow' instance.
     */
    MenuBar(MainWindow *win);
    ~MenuBar();

    /**
     * @brief Adds a recent file path to the menu.
     * @param path The path of a recently opened file.
     */
    void addRecent(const QString &path);

    /**
     * @brief Clears all recent file paths from the menu.
     */
    void clearRecent();

private:
    MainWindow *win;
    Editor *editor;

    // File menu actions
    QMenu *recentMenu;
    QAction *recentSep;
    QAction *recentAction;

    // Edit menu actions
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *delAction;
    QAction *findPrevAction;
    QAction *findNextAction;
    QAction *goAction;

    // View menu actions
    QAction *lineAction;
    QAction *statusAction;
    QAction *wrapAction;

    /**
     * @brief Creates a 'File' menu that contains
     * program actions and file operations.
     */
    void makeFileMenu();

    /**
     * @brief Creates an 'Edit' menu that contains editor actions.
     */
    void makeEditMenu();

    /**
     * @brief Creates an 'View' menu that contains view options.
     */
    void makeViewMenu();

    /**
     * @brief Creates an 'Help' menu that contains
     * preferences and program information.
     */
    void makeHelpMenu();
};
