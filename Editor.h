#pragma once

#include <QPlainTextEdit>
#include <QSyntaxHighlighter>
#include <QRegularExpression>

// Forward declarations
class MainWindow;
class LineBar;

/**
 * @brief Interface for text editing.
 */
class Editor : public QPlainTextEdit {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'Editor' instance.
     * @param win The parent 'MainWindow' instance.
     */
    Editor(MainWindow *win);

    /**
     * @brief Provides access to the 'LineBar' instance.
     * @return The 'LineBar' instance.
     */
    LineBar *getLineBar();

    /**
     * @brief Finds the previous occurrence of the specified text snippet.
     * @return The text cursor in the editor,
     * positioned where the text snippet was previously located.
     */
    QTextCursor findPrev();

    /**
     * @brief Finds the next occurrence of the specified text snippet.
     * @return The text cursor in the editor,
     * positioned where the text snippet was next located.
     */
    QTextCursor findNext();

    /**
     * @brief Replaces the next occurrence of the specified text snippet
     * with something else.
     */
    void replace();

    /**
     * @brief Replaces all occurrences of the specified text snippet
     * with something else.
     */
    void replaceAll();

    /**
     * @brief Displays an error message,
     * notifying that the specified text snippet is not found.
     */
    void showFindError();

    /**
     * @brief Moves the text cursor to a specific line in the editor.
     * @note This method will only work if word wrap is disabled.
     * @param line The line to go to.
     */
    void goTo(int line);

    /**
     * @brief Updates the width of the line bar.
     */
    void updateLineBarWidth();

    /**
     * @brief Handles the paint event of the line bar.
     * @param event The paint event of the line bar
     */
    void lineBarPaintEvent(QPaintEvent *event);

    /**
     * @brief Enables or disables word wrap.
     * @param wrap Whether to enable word wrap.
     */
    void setWordWrap(bool wrap);

    /**
     * @brief Sets the zoom percentage of the editor font size.
     * @param zoom The zoom percentage of the editor font size.
     */
    void setZoom(int zoom);

    /**
     * @brief Sets the font of the text editor and the line numbers.
     * @param font The font of the text editor and the line numbers.
     */
    void setFont(const QFont &font);

protected:
    void resizeEvent(QResizeEvent *event) override;

    // Enable drag & drop of files
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    MainWindow *win;
    LineBar *lineBar;

    /**
     * @brief Configures the search flags based on the preferences.
     * @return The configured search flags.
     */
    QTextDocument::FindFlags findFlags();

    /**
     * @brief Calculates the width of the line bar.
     * @return The width of the line bar.
     */
    int lineBarWidth();

    /**
     * @brief Updates the line bar.
     * @param rect The covered viewport area.
     * @param dy The amount of pixels the viewport was scrolled.
     */
    void updateLineBar(const QRect &rect, int dy);
};

/**
 * @brief Highlights a text snippet inside the editor in yellow.
 */
class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'Highlighter' instance.
     * @param editor The parent 'Editor' instance.
     */
    Highlighter(Editor *editor);

    /**
     * @brief Updates the text snippet to search and highlight.
     */
    void updateTarget();

protected:
    void highlightBlock(const QString &text) override;

private:
    Editor *editor;

    // The target to highlight
    QRegularExpression ex;
    // The highlighting format
    QTextCharFormat format;
};

/**
 * @brief Displays line numbers on the left side of the editor.
 */
class LineBar : public QFrame {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'LineBar' instance.
     * @param editor The parent 'Editor' instance.
     */
    LineBar(Editor *editor);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Editor *editor;
};
