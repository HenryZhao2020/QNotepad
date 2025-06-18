#include "MainWindow.h"
#include "AppInfo.h"
#include "MenuBar.h"
#include "Editor.h"
#include "StatusBar.h"
#include "Attr.h"
#include "FileUtil.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QMimeData>
#include <QMessageBox>
#include <QShortcut>

#ifdef Q_OS_WINDOWS
#include <windows.h>
#endif

QList<MainWindow *> MainWindow::windows;
const QString MainWindow::EXT_FILTER =
    QFileDialog::tr("Text Documents (*.txt)") + "\n" +
    QFileDialog::tr("All Files (*.*)");

MainWindow::MainWindow(const QString &path) {
    // Get the full file path
    file = new QFile{path};
    file->open(QFile::ReadWrite | QFile::Text);

    filePath = QFileInfo{path}.absoluteFilePath();
    // The default file name is 'Untitled' if no file is opened
    fileName = filePath.isEmpty() ? tr("Untitled") : QFileInfo{filePath}.fileName();
    // If the specified path does not exist, treat the file as 'unsaved'
    saved = filePath.isEmpty() ? true : QFileInfo::exists(filePath);

    // Register this instance
    windows.append(this);

    resize(1080, 720);
    setAttribute(Qt::WA_DeleteOnClose);

    // Place an editor in the center
    editor = new Editor(this);
    if (!filePath.isEmpty()) {
        editor->setPlainText(FileUtil::readAll(filePath));
    }
    connect(editor, &Editor::textChanged, this, &MainWindow::updateSave);
    setCentralWidget(editor);

    // Place a menu bar on the top
    menuBar = new MenuBar(this);
    setMenuBar(menuBar);

    // Place a status bar on the bottom
    statusBar = new StatusBar(this);
    setStatusBar(statusBar);

    // Display this window
    updateTitle();
    move(nextWindowPosition(size()));
    show();
    raiseWindow();

    // Use <Cmd+W> to close window in macOS
    auto *closeShortcut = new QShortcut{QKeySequence::Close, this};
    connect(closeShortcut, &QShortcut::activated, this, &MainWindow::close);
}

MainWindow::~MainWindow() {
    file->close();
    file->deleteLater();
}

Editor *MainWindow::getEditor() const {
    return editor;
}

MenuBar *MainWindow::getMenuBar() const {
    return menuBar;
}

StatusBar *MainWindow::getStatusBar() const {
    return statusBar;
}

void MainWindow::open() {
    // Prompt the user to select file(s) to open
    const QStringList &paths = QFileDialog::getOpenFileNames(
        this, QFileDialog::tr("Open"), Attr::get().recentDir, EXT_FILTER);

    // Open each file in a separate window
    for (const auto &path : paths) {
        open(path);

        // Update the recent directory
        Attr::get().recentDir = QFileInfo{path}.absolutePath();
    }
}

bool MainWindow::save() {
    // If no file is opened, choose a location to save the editor content
    if (filePath.isEmpty()) {
        saveAs();
    // Otherwise, save the file at the current path
    } else {
        save(filePath);
    }

    return saved;
}

void MainWindow::saveAs() {
    // Prompt the user to select where to save the file
    const QString &path = QFileDialog::getSaveFileName(
        this, QFileDialog::tr("Save As"), Attr::get().recentDir, EXT_FILTER);

    // Exit the function if the user closes the file dialog
    if (path.isEmpty()) {
        return;
    }

    // Get the full file path
    const QString &fullPath = QFileInfo{path}.absoluteFilePath();

    // Update the recent directory
    Attr::get().recentDir = QFileInfo{path}.absolutePath();

    // If the selected path is the same as the original path,
    // save the current file
    if (fullPath == filePath) {
        save(fullPath);
        return;
    }

    // If a file is already opened in another window, close that window
    for (const auto &win : std::as_const(windows)) {
        if (win->filePath == fullPath) {
            win->close();
            break;
        }
    }

    filePath = fullPath;
    fileName = QFileInfo{fullPath}.fileName();
    addRecent(filePath);
    save(filePath);
}

void MainWindow::selectNewFont() {
    bool ok;
    const auto &font = QFontDialog::getFont(
        &ok, Attr::get().editorFont, this, QFontDialog::tr("Select Font"));

    if (ok) {
        Attr::get().editorFont = font;
    } else {
        return;
    }

    // Update the editor font of every window
    for (const auto &win : std::as_const(windows)) {
        win->getEditor()->setFont(font);
        win->getEditor()->getLineBar()->setFont(font);
    }
}

void MainWindow::newWindow() {
    new MainWindow();
}

void MainWindow::open(const QString &path) {
    // Get the full file path
    const QString &fullPath = QFileInfo{path}.absoluteFilePath();

    // If a file is already opened in another window, switch to that window
    for (const auto &win : std::as_const(windows)) {
        if (win->filePath == fullPath) {
            win->raiseWindow();
            return;
        }
    }

    addRecent(fullPath);
    new MainWindow(fullPath);
}

void MainWindow::addRecent(const QString &path) {
    // Get the full file path
    const QString &fullPath = QFileInfo{path}.absoluteFilePath();

    // Ensure there are no duplicate paths
    if (Attr::get().recentPaths.contains(fullPath)) {
        return;
    }

    // Update the recent menu of every window
    for (const auto &win : std::as_const(windows)) {
        win->menuBar->addRecent(fullPath);
    }

    Attr::get().recentPaths.append(fullPath);
}

void MainWindow::clearRecent() {
    // Update the recent menu of every window
    for (const auto &win : std::as_const(windows)) {
        win->menuBar->clearRecent();
    }

    Attr::get().recentPaths = {};
}

void MainWindow::showLineNum(bool shown) {
    Attr::get().showLine = shown;

    for (const auto &win : std::as_const(windows)) {
        win->getEditor()->updateLineBarWidth();
    }
}

void MainWindow::showStatus(bool shown) {
    Attr::get().showStatus = shown;

    for (const auto &win : std::as_const(windows)) {
        win->getStatusBar()->setVisible(shown);
    }
}

void MainWindow::setWordWrap(bool wrap) {
    Attr::get().wordWrap = wrap;

    for (const auto &win : std::as_const(windows)) {
        win->getEditor()->setWordWrap(wrap);
    }
}

void MainWindow::zoomIn() {
    setZoom(Attr::get().zoom + 10);
}

void MainWindow::zoomOut() {
    setZoom(Attr::get().zoom - 10);
}

void MainWindow::resetZoom() {
    setZoom(100);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);

    // If the file is already saved, close the window without confirmation
    if (saved) {
        windows.removeOne(this);
        event->accept();
        return;
    }

    // If the file is unsaved, prompt the user to save it
    const auto &ans = QMessageBox::question(
        this, tr("Confirm Exiting"),
        tr("Do you want to save changes to %0?").arg(fileName),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    // If the user selects 'Yes' and the saving process is successful,
    // or if the user selects 'No', close this window
    if ((ans == QMessageBox::Yes && save()) || ans == QMessageBox::No) {
        windows.removeOne(this);
        event->accept();
    // If the saving is interrupted, or if the user selects 'Cancel',
    // this window will not be closed
    } else {
        event->ignore();
    }
}

void MainWindow::save(const QString &path) {
    // Skip the operation if the file is already saved
    if (saved) {
        return;
    }

    // Save the file
    FileUtil::writeAll(path, editor->toPlainText());

    saved = true;
    updateTitle();
}

void MainWindow::updateSave() {
    // If no file is opened, treat the file as 'saved' if the editor is empty
    // Otherwise, the file is unsaved
    saved = filePath.isEmpty() ? editor->toPlainText().isEmpty() : false;
    updateTitle();
}

void MainWindow::updateTitle() {
    QString title = fileName + " - " + AppInfo::name();
    // Unsaved file starts with an asterisk symbol (*)
    if (!saved) {
        title = "*" + title;
    }
    setWindowTitle(title);
}

void MainWindow::setZoom(int zoom) {
    // Ensure the zoom percentage is between 10% and 500%
    if (zoom > 500 || zoom < 10) {
        return;
    }

    Attr::get().zoom = zoom;

    // Update the zoom percentage of every window
    for (const auto &win : std::as_const(windows)) {
        win->getEditor()->setZoom(zoom);
        win->getStatusBar()->updateZoom();
    }
}

void MainWindow::updateEditorFont() {
    for (auto win : std::as_const(windows)) {
        win->editor->setFont(Attr::get().editorFont);
        win->editor->setZoom(Attr::get().zoom);
    }
}

void MainWindow::closeAll() {
    for (auto win : std::as_const(windows)) {
        win->close();
    }
}

void MainWindow::raiseWindow() {
#ifdef Q_OS_WINDOWS
    HWND hwnd = (HWND) winId();

    // check if widget is minimized to Windows task bar
    if (::IsIconic(hwnd)) {
        ::ShowWindow(hwnd, SW_RESTORE);
    }

    ::SetForegroundWindow(hwnd);
#else
    show();
    raise();
    activateWindow();
#endif
}

QPoint MainWindow::nextWindowPosition(const QSize &windowSize) {
    static QPoint base = QPoint(100, 100);
    static QPoint offset = QPoint(30, 30);
    static QRect screen = QGuiApplication::primaryScreen()->availableGeometry();

    QPoint nextPos = base;
    base += offset;

    // Reset if it would go off screen
    if (base.x() + windowSize.width() > screen.right() ||
        base.y() + windowSize.height() > screen.bottom()) {
        base = QPoint(100, 100); // reset to origin
    }

    return nextPos;
}
