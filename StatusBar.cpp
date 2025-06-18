#include "StatusBar.h"
#include "MainWindow.h"
#include "Editor.h"
#include "Attr.h"

StatusBar::StatusBar(MainWindow *win) : QStatusBar(win), win(win) {
    // Hide the size grip on the bottom right corner
    setSizeGripEnabled(false);

    posLabel = new QLabel(this);
    updateCursorPos();
    // Update the cursor position on typing
    connect(win->getEditor(), &Editor::cursorPositionChanged,
            this, &StatusBar::updateCursorPos);
    addPermanentWidget(posLabel);

    zoomLabel = new QLabel(this);
    updateZoom();
    addPermanentWidget(zoomLabel);
}

StatusBar::~StatusBar() {

}

void StatusBar::updateCursorPos() {
    const QTextCursor &cursor = win->getEditor()->textCursor();
    // Current line
    int ln = cursor.blockNumber() + 1;
    // Current column
    int col = cursor.columnNumber() + 1;
    posLabel->setText(tr("Ln %0, Col %1").arg(ln).arg(col));
}

void StatusBar::updateZoom() {
    zoomLabel->setText(QString::number(Attr::get().zoom) + "%");
}
