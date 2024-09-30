#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "trie.h" // Include Trie operations
#include <QFileDialog>  // For opening file dialogs
#include <QMessageBox>  // For showing error dialogs



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), trie() { // Initialize Trie member
    ui->setupUi(this);

    // Load sequences from file at startup
    loadSequencesFromFile("/Users/ravipandey/DNA/FASTA/sequence.fasta", trie, this);

    // Connect the search button to the search function
    connect(ui->pushButton_search, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);

    // Set the style for the main window
    this->setStyleSheet(
        "QMainWindow {"
        "   background-color: #f4f4f4;" // Light gray background
        "}"

        // Style for buttons
        "QPushButton {"
        "   background-color: #6200ea;" // Vibrant purple background
        "   color: white;" // Text color
        "   border: none;" // No border
        "   border-radius: 8px;" // Rounded corners
        "   padding: 12px 20px;" // Padding for buttons
        "   font-size: 16px;" // Font size
        "   font-weight: bold;" // Bold text
        "   transition: background-color 0.3s, transform 0.2s;" // Transition effect
        "}"
        "QPushButton:hover {"
        "   background-color: #3700b3;" // Darker purple on hover
        "   transform: scale(1.05);" // Slightly enlarge button on hover
        "}"

        // Style for text boxes
        "QLineEdit {"
        "   background-color: white;" // White background for text boxes
        "   color: #333;" // Dark text color for input
        "   border: 1px solid #bbb;" // Light border
        "   border-radius: 8px;" // Rounded corners
        "   padding: 10px;" // Padding for text boxes
        "   font-size: 16px;" // Font size
        "   font-weight: normal;" // Normal weight
        "   box-shadow: 0 2px 5px rgba(0, 0, 0, 0.1);" // Light shadow
        "}"

        // Style for labels
        "QLabel {"
        "   color: #333;" // Dark text color for labels
        "   font-size: 18px;" // Font size
        "   font-weight: bold;" // Bold font for labels
        "   padding: 5px;" // Padding for labels
        "}"

        // Style for group boxes (if any)
        "QGroupBox {"
        "   border: 1px solid #ccc;" // Light border for group boxes
        "   border-radius: 8px;" // Rounded corners
        "   margin-top: 20px;" // Space above group boxes
        "   padding: 10px;" // Padding inside group boxes
        "   background-color: #ffffff;" // White background
        "}"

        // Style for the overall layout
        "QVBoxLayout {"
        "   spacing: 15px;" // Space between elements
        "   margin: 15px;" // Margin around layout
        "}"
        );


}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::displayMessage(const QString& message) {
    ui->textEdit_results->append(message);
    //ui->textEdit_results->verticalScrollBar()->setValue(ui->textEdit_results->verticalScrollBar()->maximum()); // Auto-scroll to the bottom
}

void MainWindow::clearResults() {
    ui->textEdit_results->clear();
}


void MainWindow::onSearchButtonClicked() {
    QString sequence = ui->lineEdit_sequence->text().trimmed();
    QString mismatches = ui->lineEdit_mismatches->text();

    if (sequence.isEmpty()) {
        displayMessage("Error: DNA sequence cannot be empty.");
        return;
    }

    bool ok;
    int allowedMismatches = mismatches.toInt(&ok);
    if (!ok || allowedMismatches < 0 || allowedMismatches > 2) {
        displayMessage("Error: Number of mismatches must be 0, 1, or 2.");
        return;
    }

    ui->textEdit_results->clear();
    QString unsupportedChars; // Declare QString to collect unsupported characters
    QString result = QString::fromStdString(searchDNASequence(trie, sequence.toStdString(), allowedMismatches, this));


    // Check for any unsupported characters found during the search
    if (!unsupportedChars.isEmpty()) {
       result += "\nError: Unsupported characters found: " + unsupportedChars;
    }

    // Display the result message
    displayMessage(result);
}


void MainWindow::loadSequencesFromFile(const QString& filePath, Trie& trie,  MainWindow* mainWindow) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        displayMessage("Error: Unable to open file.");
        return;
    }

    QTextStream in(&file);
    QString line;
    QString currentSequence;

    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line.startsWith(">")) {
            if (!currentSequence.isEmpty()) {
                displayMessage("Inserting: " + currentSequence);
                trie.insert(currentSequence.toStdString(), this);
                currentSequence.clear();
            }
        } else {
            currentSequence += line;
        }
    }

    if (!currentSequence.isEmpty()) {
        displayMessage("Inserting: " + currentSequence);
        trie.insert(currentSequence.toStdString(), this);
    }

    file.close();
}
