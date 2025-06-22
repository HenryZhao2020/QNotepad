#pragma once

#include <QStatusBar>
#include <QLabel>

// Forward declarations
class MainWindow;

/**
 * @brief Displays the editor status.
 */
class StatusBar : public QStatusBar {
    Q_OBJECT

public:
    /**
     * @brief Initializes a new 'StatusBar' instance.
     * @param win The parent 'MainWindow' instance.
     */
    StatusBar(MainWindow *win);

    /**
     * @brief Updates the cursor position.
     */
    void updateCursorPos();

    /**
     * @brief Updates the zoom percentage of the editor font size.
     */
    void updateZoom();

private:
    MainWindow *win;

    // Display the text cursor position
    QLabel *posLabel;
    // Display the zoom percentage
    QLabel *zoomLabel;
};
