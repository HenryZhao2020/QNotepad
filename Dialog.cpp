#include "Dialog.h"
#include "AppInfo.h"
#include "MainWindow.h"
#include "Editor.h"
#include "Attr.h"

#include <QPushButton>
#include <QShortcut>
#include <QCheckBox>
#include <QLabel>
#include <QTextBrowser>
#include <QMessageBox>
#include <QDesktopServices>

Dialog::Dialog(MainWindow *win)
    : QDialog{win}, win{win}, editor{win->getEditor()} {
    // Free memory on close
    setAttribute(Qt::WA_DeleteOnClose);

    // Set up the layout
    mainLayout = new QGridLayout{this};
    mainLayout->setHorizontalSpacing(5);
    mainLayout->setVerticalSpacing(5);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    // Use <Cmd+W> to close window in macOS
    auto *closeShortcut = new QShortcut{QKeySequence::Close, this};
    connect(closeShortcut, &QShortcut::activated, this, &Dialog::close);
}

Dialog::~Dialog() {}

void Dialog::show() {
    // Make this window not resizable
    setFixedSize(sizeHint());
    QDialog::show();
}

FindDialog::FindDialog(MainWindow *win) : Dialog{win} {
    setWindowTitle(tr("Find"));

    highlighter = new Highlighter{editor};

    findField = new QLineEdit{Attr::get().findTarget, this};
    findField->setFixedWidth(200);
    findField->setPlaceholderText(tr("Find..."));
    // Disable the buttons if the field is empty
    connect(findField, &QLineEdit::textChanged, this, [this] (const QString &text) {
        findPrevButton->setEnabled(!text.isEmpty());
        findNextButton->setEnabled(!text.isEmpty());

        // Update the text snippet to search for
        Attr::get().findTarget = text;
        highlighter->updateTarget();
    });
    mainLayout->addWidget(findField, 0, 0);

    findPrevButton = new QPushButton{tr("Find Prev"), this};
    findPrevButton->setEnabled(!findField->text().isEmpty());
    connect(findPrevButton, &QPushButton::clicked, this, [this] {
        // If the text snippet is not found, display an error message
        if (editor->findPrev().isNull()) {
            editor->showFindError();
        }
    });
    mainLayout->addWidget(findPrevButton, 0, 1);

    findNextButton = new QPushButton{tr("Find Next"), this};
    findNextButton->setDefault(true);
    findNextButton->setEnabled(!findField->text().isEmpty());
    connect(findNextButton, &QPushButton::clicked, this, [this] {
        // If the text snippet is not found, display an error message
        if (editor->findNext().isNull()) {
            editor->showFindError();
        }
    });
    mainLayout->addWidget(findNextButton, 0, 2);
    mainLayout->addItem(new QSpacerItem{0, 40}, 2, 0);

    // Whether to match case upon searching
    newOption(tr("Match Case"), Attr::get().matchCase);

    // Whether to match whole word upon searching
    newOption(tr("Match Whole Word"), Attr::get().matchWholeWord);
}

FindDialog::~FindDialog() {
    // Temporarily stop detecting changes in the editor
    editor->blockSignals(true);

    // Remove the highlighter
    highlighter->setDocument(nullptr);
    highlighter->deleteLater();

    editor->blockSignals(false);
}

void FindDialog::newOption(const QString &text, bool &state) {
    auto box = new QCheckBox{text, this};
    box->setChecked(state);
    connect(box, &QCheckBox::checkStateChanged, this, [this, &state] (bool newState) {
        // Update the preference
        state = newState;
        // Search for the text snippet again
        highlighter->updateTarget();
    });
    mainLayout->addWidget(box, mainLayout->rowCount(), 0, 1, 3);
}

ReplaceDialog::ReplaceDialog(MainWindow *win) : FindDialog{win} {
    setWindowTitle(tr("Replace"));

    // Disable the buttons if the field is empty
    connect(findField, &QLineEdit::textChanged, this, [this] (const QString &text) {
        replaceButton->setEnabled(!text.isEmpty());
        replaceAllButton->setEnabled(!text.isEmpty());
    });

    replaceField = new QLineEdit{Attr::get().replaceTarget, this};
    connect(replaceField, &QLineEdit::textChanged, this, [] (const QString &text) {
        // Update the replacement
        Attr::get().replaceTarget = text;
    });
    mainLayout->addWidget(replaceField, 1, 0);

    replaceButton = new QPushButton{tr("Replace"), this};
    replaceButton->setEnabled(!findField->text().isEmpty());
    connect(replaceButton, &QPushButton::clicked, this, [this] {
        editor->replace();
    });
    mainLayout->addWidget(replaceButton, 1, 1);

    replaceAllButton = new QPushButton{tr("Replace All"), this};
    replaceAllButton->setEnabled(!findField->text().isEmpty());
    connect(replaceAllButton, &QPushButton::clicked, this, [this] {
        editor->replaceAll();
    });
    mainLayout->addWidget(replaceAllButton, 1, 2);
}

ReplaceDialog::~ReplaceDialog() {}

GoToDialog::GoToDialog(MainWindow *win) : Dialog(win) {
    setWindowTitle(tr("Go To"));
    // Disable all background windows
    setModal(true);

    // Adjust the layout spacing
    mainLayout->setHorizontalSpacing(50);
    mainLayout->setVerticalSpacing(50);

    lnField = new QSpinBox{this};
    // Ensure the value is within the line counts
    lnField->setMinimum(1);
    lnField->setMaximum(editor->document()->blockCount());
    // Set to the current line number
    lnField->setValue(editor->textCursor().blockNumber() + 1);
    mainLayout->addWidget(new QLabel{tr("Line:")}, 0, 0);
    mainLayout->addWidget(lnField, 0, 1);

    goButton = new QPushButton{tr("Go"), this};
    goButton->setDefault(true);
    connect(goButton, &QPushButton::clicked, this, &GoToDialog::go);
    mainLayout->addWidget(goButton, 1, 1);
}

GoToDialog::~GoToDialog() {}

void GoToDialog::keyPressEvent(QKeyEvent *event) {
    Dialog::keyPressEvent(event);

    // Press <Enter> to trigger the 'Go' button
    if (event->key() == Qt::Key_Return) {
        go();
    }
}

void GoToDialog::go() {
    win->getEditor()->goTo(lnField->value());
    close();
}

AboutDialog::AboutDialog(MainWindow *win) : Dialog{win} {
    setWindowTitle(tr("About") + " " + AppInfo::name());
    // Disable all background windows
    setModal(true);

    // Adjust the layout spacing
    mainLayout->setHorizontalSpacing(25);
    mainLayout->setVerticalSpacing(40);

    // Display program info
    auto infoFrame = createInfoFrame();
    mainLayout->addWidget(infoFrame, 0, 0);

    // Close the dialog on click
    auto okButton = new QPushButton{tr("OK"), this};
    okButton->setDefault(true);
    connect(okButton, &QPushButton::clicked, this, &Dialog::close);
    mainLayout->addWidget(okButton, 1, 0, Qt::AlignCenter);
}

AboutDialog::~AboutDialog() {}

QFrame *AboutDialog::createInfoFrame() {
    auto infoFrame = new QFrame{this};
    auto infoLayout = new QVBoxLayout{infoFrame};
    infoLayout->setContentsMargins(30, 0, 30, 0);
    infoLayout->setSpacing(8);

    auto logoButton = new QPushButton{this};
    logoButton->setIcon(AppInfo::icon());
    logoButton->setIconSize(QSize{128, 128});
    logoButton->setObjectName("borderless");
    infoLayout->addWidget(logoButton, 0, Qt::AlignCenter);

    auto titleLabel = new QLabel{AppInfo::name(), this};
    titleLabel->setObjectName("title");
    infoLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    auto versionLabel = new QLabel{tr("Version") + ": " + AppInfo::version(), this};
    infoLayout->addWidget(versionLabel, 0, Qt::AlignCenter);

    auto devLabel = new QLabel{tr("Developer") + ": " + AppInfo::developer(), this};
    infoLayout->addWidget(devLabel, 0, Qt::AlignCenter);

    auto linkButton = new QPushButton{tr("Visit my GitHub"), this};
    linkButton->setObjectName("link");
    linkButton->setCursor(Qt::PointingHandCursor);
    connect(linkButton, &QPushButton::clicked, []() {
        QDesktopServices::openUrl(AppInfo::github());
    });
    infoLayout->addWidget(linkButton, 0, Qt::AlignCenter);
    infoLayout->addStretch();

    return infoFrame;
}

