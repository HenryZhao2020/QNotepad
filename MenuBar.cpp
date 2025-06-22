#include "MenuBar.h"
#include "AppInfo.h"
#include "MainWindow.h"
#include "Editor.h"
#include "StatusBar.h"
#include "Dialog.h"
#include "Attr.h"

#include <QActionGroup>
#include <QMessageBox>

MenuBar::MenuBar(MainWindow *win)
    : QMenuBar{win}, win{win}, editor{win->getEditor()} {
    // Create menus and actions
    makeFileMenu();
    makeEditMenu();
    makeViewMenu();
    makeHelpMenu();
}

void MenuBar::addRecent(const QString &path) {
    auto action = new QAction{path, this};
    // Open the file at the corresponding path
    connect(action, &QAction::triggered, this, [path] {
        MainWindow::open(path);
    });
    // Add the action right on top of the separator
    recentMenu->insertAction(recentSep, action);

    recentAction->setEnabled(true);
}

void MenuBar::clearRecent() {
    const auto &actions = recentMenu->actions();
    // Remove all recent actions
    for (int i = 0; i < actions.size() - 2; ++i) {
        recentMenu->removeAction(actions[i]);
    }

    recentAction->setEnabled(false);
}

void MenuBar::makeFileMenu() {
    auto fileMenu = addMenu(tr("&File"));

    // Open a new window
    fileMenu->addAction(tr("&New..."), QKeySequence("Ctrl+N"), [] {
        MainWindow::newWindow();
    });

    // Open a file
    fileMenu->addAction(tr("&Open..."), QKeySequence("Ctrl+O"), [this] {
        win->open();
    });

    // Display the paths of recently opened files
    recentMenu = fileMenu->addMenu(tr("Open &Recent"));
    // Separate the paths and the 'Clear Recent' action
    recentSep = recentMenu->addSeparator();
    // Clear all recent paths
    recentAction = recentMenu->addAction(tr("&Clear Menu"), &MainWindow::clearRecent);
    recentAction->setEnabled(!Attr::get().recentPaths.isEmpty());
    // Add all recent paths
    for (const auto &path : Attr::get().recentPaths) {
        addRecent(path);
    }

    // Save a file
    fileMenu->addAction(tr("&Save"), QKeySequence("Ctrl+S"), [this] {
        win->save();
    });

    // Save a file to a custom location
    fileMenu->addAction(tr("Save &As..."),
                        QKeySequence("Ctrl+Shift+S"), [this] {
        win->saveAs();
    });

    fileMenu->addSeparator();

    // Close the window (on Windows only; macOS has a built-in 'Quit' action)
#ifdef Q_OS_WINDOWS
    fileMenu->addAction(tr("E&xit"), [this] {
        win->close();
    });
#endif
}

void MenuBar::makeEditMenu() {
    auto editMenu = addMenu(tr("&Edit"));
    // Enable or disable actions upon opening the editing menu
    connect(editMenu, &QMenu::aboutToShow, this, [this] {
        bool hasSelection = editor->textCursor().hasSelection();
        cutAction->setEnabled(hasSelection);
        copyAction->setEnabled(hasSelection);
        pasteAction->setEnabled(editor->canPaste());
        delAction->setEnabled(hasSelection);
        findPrevAction->setEnabled(!Attr::get().findTarget.isEmpty());
        findNextAction->setEnabled(!Attr::get().findTarget.isEmpty());
        goAction->setEnabled(!Attr::get().wordWrap);
    });

    // Undo a change
    auto undoAction = editMenu->addAction(tr("&Undo"), QKeySequence("Ctrl+Z"), [this] {
        editor->undo();
    });
    undoAction->setEnabled(false);
    connect(editor, &Editor::undoAvailable, this, [undoAction] (bool b) {
        undoAction->setEnabled(b);
    });

    // Redo a change
    auto redoAction = editMenu->addAction(tr("&Redo"), QKeySequence("Ctrl+Y"), [this] {
        editor->redo();
    });
    redoAction->setEnabled(false);
    connect(editor, &Editor::redoAvailable, this, [redoAction] (bool b) {
        redoAction->setEnabled(b);
    });

    editMenu->addSeparator();

    // Cut the selected text into the clipboard
    cutAction = editMenu->addAction(tr("Cu&t"), QKeySequence("Ctrl+X"), [this] {
        editor->cut();
    });

    // Copy the selected text into the clipboard
    copyAction = editMenu->addAction(tr("&Copy"), QKeySequence("Ctrl+C"), [this] {
        editor->copy();
    });

    // Paste the clipboard into the editor
    pasteAction = editMenu->addAction(tr("&Paste"), QKeySequence("Ctrl+V"), [this] {
        editor->paste();
    });

    // Delete the selected text
    delAction = editMenu->addAction(tr("De&lete"), QKeySequence("Del"), [this] {
        editor->textCursor().removeSelectedText();
    });

    editMenu->addSeparator();

    // Find a text snippet
    editMenu->addAction(tr("&Find..."), QKeySequence("Ctrl+F"), [this] {
        auto dialog = new FindDialog(win);
        dialog->show();
    });

    // Find the previous occurrence of a text snippet
    findPrevAction = editMenu->addAction(tr("Find Pre&vious"), QKeySequence("Shift+F3"), [this] {
        // If the text snippet is unspecified,
        // prompt the user to enter one in the find dialog
        if (Attr::get().findTarget.isEmpty()) {
            auto dialog = new FindDialog(win);
            dialog->show();
        } else if (editor->findPrev().isNull()) {
            editor->showFindError();
        }
    });

    // Find the next occurrence of a text snippet
    findNextAction = editMenu->addAction(tr("Find &Next"), QKeySequence("F3"), [this] {
        // If the text snippet is unspecified,
        // prompt the user to enter one in the find dialog
        if (Attr::get().findTarget.isEmpty()) {
            auto dialog = new FindDialog(win);
            dialog->show();
        } else if (editor->findNext().isNull()) {
            editor->showFindError();
        }
    });

    // Replace a text snippet
    editMenu->addAction(tr("&Replace..."), QKeySequence("Ctrl+H"), [this] {
        auto dialog = new ReplaceDialog(win);
        dialog->show();
    });

    editMenu->addSeparator();

    // Select all text in the editor
    editMenu->addAction(tr("Select &All"), QKeySequence("Ctrl+A"), [this] {
        editor->selectAll();
    });

    // Go to a specific line in the editor
    goAction = editMenu->addAction(tr("&Go To..."), QKeySequence("Ctrl+G"), [this] {
        auto dialog = new GoToDialog(win);
        dialog->show();
    });
}

void MenuBar::makeViewMenu() {
    auto viewMenu = addMenu(tr("&View"));
    // Enable or disable actions upon opening the editing menu
    connect(viewMenu, &QMenu::aboutToShow, this, [this] {
        lineAction->setChecked(Attr::get().showLine);
        statusAction->setChecked(Attr::get().showStatus);
        wrapAction->setChecked(Attr::get().wordWrap);
    });

    // Show or hide the line numbers
    lineAction = viewMenu->addAction(tr("Show &Line Numbers"),
                                     QKeySequence("Alt+L"), &MainWindow::showLineNum);
    lineAction->setCheckable(true);

    // Show or hide the status bar
    statusAction = viewMenu->addAction(tr("Show &Status Bar"),
                                       QKeySequence("Alt+S"), &MainWindow::showStatus);
    statusAction->setCheckable(true);

    // Set the editor word wrap mode
    wrapAction = viewMenu->addAction(tr("Word &Wrap"),
                                     QKeySequence("Alt+W"), &MainWindow::setWordWrap);
    wrapAction->setCheckable(true);

    // The 'Zoom' menu
    auto zoomMenu = viewMenu->addMenu(tr("&Zoom"));

    // Increase the editor font size
    zoomMenu->addAction(tr("Zoom &In"), QKeySequence("Ctrl+="), &MainWindow::zoomIn);

    // Decrease the editor font size
    zoomMenu->addAction(tr("Zoom &Out"), QKeySequence("Ctrl+-"), &MainWindow::zoomOut);

    // Reset the editor font size to default
    zoomMenu->addAction(tr("&Reset Zoom"), QKeySequence("Ctrl+0"), &MainWindow::resetZoom);
}

void MenuBar::makeHelpMenu() {
    auto helpMenu = addMenu(tr("&Help"));

    // Select an editor font
    helpMenu->addAction(tr("Editor &Font"), [this] {
        win->selectNewFont();
    });

    // Select displayed language
    auto langMenu = helpMenu->addMenu(tr("&Language"));
    auto langGroup = new QActionGroup(langMenu);
    langGroup->setExclusive(true);
    connect(langGroup, &QActionGroup::triggered, this, [this] (QAction *action) {
        if (action->text() == LangUtil::getLangName(Attr::get().lang)) {
            return;
        }

        for (auto lang : LangUtil::getLanguages()) {
            if (LangUtil::getLangName(lang) == action->text()) {
                Attr::get().lang = lang;
                break;
            }
        }

        QMessageBox::information(this, AppInfo::name(),
                                 tr("For the new language to take effect, "
                                    "please relaunch the program."));
    });

    for (const auto &lang : LangUtil::getLanguages()) {
        auto action = langMenu->addAction(LangUtil::getLangName(lang));
        action->setCheckable(true);
        action->setChecked(Attr::get().lang == lang);
        langGroup->addAction(action);
    }

    helpMenu->addSeparator();

    // Display program information
    helpMenu->addAction(tr("&About"), QKeySequence("F1"), [this] {
        auto dialog = new AboutDialog(win);
        dialog->show();
    });
}
