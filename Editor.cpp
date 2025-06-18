#include "Editor.h"
#include "MainWindow.h"
#include "StatusBar.h"
#include "Attr.h"
#include "AppInfo.h"

#include <QMessageBox>
#include <QPainter>
#include <QMimeData>

Editor::Editor(MainWindow *win) : QPlainTextEdit{win}, win{win} {
    // Do not dim the selection background when the editor is out of focus
    auto newPalette{palette()};
    newPalette.setColor(QPalette::Inactive, QPalette::Highlight,
                        newPalette.color(QPalette::Active, QPalette::Highlight));
    newPalette.setColor(QPalette::Inactive, QPalette::HighlightedText,
                        newPalette.color(QPalette::Active, QPalette::HighlightedText));
    setPalette(newPalette);

    // Set up the line bar
    lineBar = new LineBar{this};
    connect(this, &Editor::blockCountChanged, this, &Editor::updateLineBarWidth);
    connect(this, &Editor::updateRequest, this, &Editor::updateLineBar);

    setWordWrap(Attr::get().wordWrap);
    setZoom(Attr::get().zoom);
}

Editor::~Editor() {}

LineBar *Editor::getLineBar() {
    return lineBar;
}

QTextCursor Editor::findPrev() {
    QTextCursor cursor{textCursor()};
    int start = cursor.position() - cursor.selectedText().size();
    auto flags = findFlags() | QTextDocument::FindBackward;
    cursor = document()->find(Attr::get().findTarget, start, flags);

    // Try to find the target from the current cursor position
    // If found, skip additional search
    if (!cursor.isNull()) {
        setTextCursor(cursor);
        return cursor;
    }

    // If not found, try again from the end of the document
    start = toPlainText().size() - 1;
    cursor = document()->find(Attr::get().findTarget, start, flags);

    if (!cursor.isNull()) {
        setTextCursor(cursor);
    }

    return cursor;
}

QTextCursor Editor::findNext() {
    QTextCursor cursor{textCursor()};
    int start = cursor.position();
    auto flags = findFlags();
    cursor = document()->find(Attr::get().findTarget, start, flags);

    // Try to find the target from the current cursor position
    // If found, skip additional search
    if (!cursor.isNull()) {
        setTextCursor(cursor);
        return cursor;
    }

    // If not found, try again from the beginning of the document
    start = 0;
    cursor = document()->find(Attr::get().findTarget, start, flags);

    if (!cursor.isNull()) {
        setTextCursor(cursor);
    }

    return cursor;
}

void Editor::replace() {
    QTextCursor cursor{findNext()};

    // If the text snippet is not found, display an error message
    if (cursor.isNull()) {
        showFindError();
        return;
    }

    int length = Attr::get().findTarget.length();
    int start = cursor.position() - length;

    cursor.setPosition(start);
    cursor.setPosition(start + length, QTextCursor::KeepAnchor);
    cursor.insertText(Attr::get().replaceTarget);
    cursor.setPosition(start, QTextCursor::KeepAnchor);
    setTextCursor(cursor);
}

void Editor::replaceAll() {
    QTextCursor cursor{findNext()};

    // If the text snippet is not found, display an error message
    if (cursor.isNull()) {
        showFindError();
        return;
    }

    // Ensure that every occurrence of the text snippet is replaced
    while (!cursor.isNull()) {
        int length = Attr::get().findTarget.length();
        int start = cursor.position() - length;

        cursor.setPosition(start);
        cursor.setPosition(start + length, QTextCursor::KeepAnchor);
        cursor.insertText(Attr::get().replaceTarget);

        cursor = findNext();
    }
}

void Editor::showFindError() {
    QMessageBox::critical(this, AppInfo::name(),
                          tr("'%0' not found!").arg(Attr::get().findTarget));
}

void Editor::goTo(int line) {
    QTextCursor cursor{textCursor()};
    cursor.movePosition(QTextCursor::Start);
    cursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, line - 1);
    cursor.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, 0);
    setTextCursor(cursor);
}

void Editor::lineBarPaintEvent(QPaintEvent *event) {
    QPainter painter{lineBar};

    QTextBlock block{firstVisibleBlock()};
    int blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const QString &number = QString::number(blockNumber + 1) + " ";
            painter.drawText(0, top, lineBar->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        blockNumber++;
    }
}

void Editor::setWordWrap(bool wrap) {
    setWordWrapMode(wrap ? QTextOption::WordWrap : QTextOption::NoWrap);
}

void Editor::setZoom(int zoom) {
    QFont font{Attr::get().editorFont};
    int size = font.pointSize() * zoom / 100.0;

    // The editor font size must be a positive value
    if (size > 0) {
        font.setPointSize(size);
    }

    setFont(font);
    lineBar->setFont(font);
}

void Editor::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);

    const QRect &rect{contentsRect()};
    lineBar->setGeometry(rect.left(), rect.top(), lineBarWidth(), rect.height());
}

void Editor::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();          // Accept file drop
    } else {
        QPlainTextEdit::dragEnterEvent(event);  // Accept normal text
    }
}

void Editor::dropEvent(QDropEvent *event) {
    const auto &mimeData = event->mimeData();
    if (!mimeData->hasUrls()) {
        QPlainTextEdit::dropEvent(event);
        return;
    }

    const auto urls = mimeData->urls();
    for (const auto &url : urls) {
        MainWindow::open(url.toLocalFile());
    }
}

QTextDocument::FindFlags Editor::findFlags() {
    QTextDocument::FindFlags flags;
    if (Attr::get().matchCase) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (Attr::get().matchWholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    return flags;
}

int Editor::lineBarWidth() {
    // Hide the line bar by setting its width to 0
    if (!Attr::get().showLine) {
        return 0;
    }

    int digits = QString::number(blockCount()).size() + 2;
    return fontMetrics().averageCharWidth() * digits;
}

void Editor::updateLineBarWidth() {
    int width = lineBarWidth();
    setViewportMargins(width, 0, 0, 0);
}

void Editor::updateLineBar(const QRect &rect, int dy) {
    if (dy) {
        lineBar->scroll(0, dy);
    } else {
        lineBar->update(0, rect.y(), lineBar->width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineBarWidth();
    }
}

void Editor::setFont(const QFont &font) {
    QPlainTextEdit::setFont(font);
    lineBar->setFont(font);
}

Highlighter::Highlighter(Editor *editor)
    : QSyntaxHighlighter{editor->document()}, editor{editor} {
    // Set the highlighter background to yellow
    format.setBackground(QColor{255, 255, 0, 90});

    updateTarget();
}

Highlighter::~Highlighter() {}

void Highlighter::updateTarget() {
    // Temporarily stop detecting changes in the editor
    editor->blockSignals(true);

    const QString &pattern = Attr::get().matchWholeWord ? "\\b%0\\b" : "%0";
    ex = QRegularExpression{pattern.arg(Attr::get().findTarget)};

    if (!Attr::get().matchCase) {
        ex.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    }

    rehighlight();
    editor->blockSignals(false);
}

void Highlighter::highlightBlock(const QString &text) {
    auto matches = ex.globalMatch(text);
    int size = Attr::get().findTarget.size();

    while (matches.hasNext()) {
        auto match = matches.next();
        setFormat(match.capturedStart(), size, format);
    }
}

LineBar::LineBar(Editor *editor) : QFrame{editor}, editor{editor} {}

LineBar::~LineBar() {}

void LineBar::paintEvent(QPaintEvent *event) {
    QFrame::paintEvent(event);
    editor->lineBarPaintEvent(event);
}
