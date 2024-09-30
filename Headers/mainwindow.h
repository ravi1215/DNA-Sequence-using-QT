#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"  // Make sure to include this line
#include "trie.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void loadSequencesFromFile(const QString& filename, Trie& trie, MainWindow* mainWindow);
    void clearResults(); // Method to clear the output results
    void displayMessage(const QString& message); // Method to display messages
    void appendToResults(const QString& message) {
        ui->textEdit_results->append(message);
    }
    Trie& getTrie() { return trie; }
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchButtonClicked();

private:
    Ui::MainWindow *ui;
    Trie trie;
};

#endif // MAINWINDOW_H
