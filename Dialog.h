#pragma once

#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QSpinBox>

// Forward declarations
class MainWindow;
class Editor;
class Highlighter;

/**
 * @brief The base class for dialog boxes in the program.
 */
class Dialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Make this window non-resizable upon opening.
     */
    void show();

protected:
    /**
     * @brief Initializes a new 'Dialog' instance.
     * @param win The parent 'MainWindow' instance.
     */
    Dialog(MainWindow *win);
    ~Dialog();

    MainWindow *win;
    Editor *editor;

    // Main layout of the dialog
    QGridLayout *mainLayout;
};

/**
 * @brief Prompts the user to search for a text snippet.
 */
class FindDialog : public Dialog {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'FindDialog' instance.
     * @param win The parent 'MainWindow' instance.
     */
    FindDialog(MainWindow *win);
    ~FindDialog();

protected:
    // Highlight the specified text snippet
    Highlighter *highlighter;
    // Prompt the user to enter the text snippet to search for
    QLineEdit *findField;
    // Find the previous occurrence of the text snippet
    QPushButton *findPrevButton;
    // Find the next occurrence of the text snippet
    QPushButton *findNextButton;

private:
    /**
     * @brief Adds a new check box option.
     * @param text The text to be displayed beside the check box.
     * @param state The check state of the check box.
     */
    void newOption(const QString &text, bool &state);
};

/**
 * @brief Prompts the user to search for and replace a text snippet.
 */
class ReplaceDialog : public FindDialog {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'ReplaceDialog' instance.
     * @param win The parent 'MainWindow' instance.
     */
    ReplaceDialog(MainWindow *win);
    ~ReplaceDialog();

private:
    // Prompt the user to enter the text snippet to replace
    QLineEdit *replaceField;
    // Replace the next occurrence of the text snippet
    QPushButton *replaceButton;
    // Replace all occurrences of the text snippet
    QPushButton *replaceAllButton;
};

/**
 * @brief Prompts the user to go to a specific line in the editor.
 */
class GoToDialog : public Dialog {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'GoToDialog' instance.
     * @param win The parent 'MainWindow' instance.
     */
    GoToDialog(MainWindow *win);
    ~GoToDialog();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    // Prompt the user to select a line
    QSpinBox *lnField;
    // Go to the specified line in the editor on click
    QPushButton *goButton;

    /**
     * @brief Moves the text cursor to the specified line in the editor.
     */
    void go();
};

/**
 * @brief Displays program information.
 */
class AboutDialog : public Dialog {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'AboutDialog' instance.
     * @param win The parent 'MainWindow' instance.
     */
    AboutDialog(MainWindow *win);
    ~AboutDialog();

private:
    QFrame *createInfoFrame();
};
