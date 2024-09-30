#include "trie.h"
#include "mainwindow.h"  // Include the MainWindow header
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <memory>


using namespace std;

// Convert character to index for the Trie
int Trie::charToIndex(char c, MainWindow* mainWindow, QString& unsupportedChars) {
    switch (c) {
    case 'A': return 0;
    case 'C': return 1;
    case 'G': return 2;
    case 'T': return 3;
    default:
        unsupportedChars += QString(c) + " ";  // Collect unsupported character
        // QString errorMsg = "Error: Unsupported character '" + QString(c) + "' in sequence.";
        // mainWindow->displayMessage(errorMsg);  // Display error in output box
        return -1;
    }
}

// Insert a sequence into the Trie
void Trie::insert(const string& sequence, MainWindow* mainWindow) {
    TrieNode* node = root.get();
    QString unsupportedChars; // Declare the QString to track unsupported characters

    // Use the public method to display messages
    mainWindow->displayMessage("Inserting: " + QString::fromStdString(sequence));

    for (char c : sequence) {
        int index = charToIndex(c, mainWindow, unsupportedChars);
        if (index == -1) continue;  // Skip unsupported characters
        if (!node->children[index]) {
            node->children[index] = make_unique<TrieNode>();
        }
        node = node->children[index].get();
    }
    node->isEndofSequence = true;

    // After inserting, check for unsupported characters
    if (!unsupportedChars.isEmpty()) {
        mainWindow->displayMessage("Error: Unsupported characters found during insertion: " + unsupportedChars);
    }
}

// Helper function to search from a given position
bool Trie::searchFromPosition(TrieNode* node, const string& sequence, size_t index, MainWindow* mainWindow) {
    if (!node) return false;
    QString unsupportedChars;

    for (size_t j = index; j < sequence.length(); j++) {
        int charIndex = charToIndex(sequence[j], mainWindow, unsupportedChars);
        if (charIndex == -1) continue;

        if (!node->children[charIndex]) {
            return false;
        }
        node = node->children[charIndex].get();
    }
    return node->isEndofSequence;
}

// Search for asequence in the Trie
bool Trie::search(const string& sequence, MainWindow* mainWindow) {
    QString unsupportedChars;  // String to collect unsupported characters
    mainWindow->displayMessage("Searching for: " + QString::fromStdString(sequence)); // Debug message
    for (size_t i = 0; i < sequence.length(); i++) {
        int result = charToIndex(sequence[i], mainWindow, unsupportedChars);
        if (result == -1) continue;  // Stop if an unsupported character is found
        if (searchFromPosition(root.get(), sequence, i, mainWindow)) {
            return true;
        }
    }

    // If unsupported characters were found, display the message
    if (!unsupportedChars.isEmpty()) {
        mainWindow->displayMessage("Error: Unsupported characters found: " + unsupportedChars);
    }

    return false;
}

// Helper function to search with allowed mismatches
bool Trie::searchWithMismatch(TrieNode* node, const string& sequence, size_t index, int mismatchCount, int allowedMismatches, MainWindow* mainWindow, QString& unsupportedChars) {
    if (!node) return false;

    if (index == sequence.length()) {
        mainWindow->displayMessage("Reached end of sequence with mismatch count: " + QString::number(mismatchCount));
        return node->isEndofSequence && mismatchCount <= allowedMismatches;
    }

    int charIndex = charToIndex(sequence[index], mainWindow, unsupportedChars);

    // Handle unsupported characters
    if (charIndex == -1) {
        if (mismatchCount < allowedMismatches) {
            for (int i = 0; i < 4; i++) { // A, C, G, T
                if (node->children[i]) {
                    if (searchWithMismatch(node->children[i].get(), sequence, index + 1, mismatchCount + 1, allowedMismatches, mainWindow, unsupportedChars)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    // Exact match
    if (node->children[charIndex]) {
        if (searchWithMismatch(node->children[charIndex].get(), sequence, index + 1, mismatchCount, allowedMismatches, mainWindow, unsupportedChars)) {
            return true;
        }
    }

    // Try mismatches
    if (mismatchCount < allowedMismatches) {
        for (int i = 0; i < 4; i++) { // A, C, G, T
            if (i != charIndex && node->children[i]) { // Exclude exact match
                if (searchWithMismatch(node->children[i].get(), sequence, index + 1, mismatchCount + 1, allowedMismatches, mainWindow, unsupportedChars)) {
                    return true;
                }
            }
        }
    }

    // Display unsupported characters if found
    if (!unsupportedChars.isEmpty()) {
        mainWindow->displayMessage("Error: Unsupported characters found: " + unsupportedChars);
    }


    return false;
}

// Search for a sequence allowing a certain number of mismatches
bool Trie::searchWithAllowedMismatches(const string& sequence, int allowedMismatches, MainWindow* mainWindow, QString& unsupportedChars) {
    //QString unsupportedChars;  // String to collect unsupported characters
    mainWindow->displayMessage("Searching for: " + QString::fromStdString(sequence)); // Debug message
    for (size_t i = 0; i < sequence.length(); i++) {
        int result = charToIndex(sequence[i], mainWindow, unsupportedChars);
        if (result == -1) continue;  // Stop if an unsupported character is found
        if (searchWithMismatch(root.get(), sequence.substr(i), 0, 0, allowedMismatches, mainWindow, unsupportedChars)) {
            return true;
        }
    }

    // If unsupported characters were found, display the message
    if (!unsupportedChars.isEmpty()) {
        mainWindow->displayMessage("Error: Unsupported characters found: " + unsupportedChars);
    }

    return false;
}



// Function to search for a DNA sequence with an optional number of allowed mismatches
string searchDNASequence(Trie& trie, const string& sequence, int allowedMismatches, MainWindow* mainWindow) {
    QString unsupportedChars; // Declare the QString to collect unsupported characters

    // Check for exact match first
    if (allowedMismatches == 0) {
        return trie.search(sequence, mainWindow) ? "Found" : "Not Found";
    }

    // If allowed mismatches are greater than zero, check for matches with mismatches
    for (int i = 1; i <= allowedMismatches; ++i) {
        if (trie.searchWithAllowedMismatches(sequence, i, mainWindow, unsupportedChars)) {
            return "Found with " + to_string(i) + " mismatch" + (i > 1 ? "es" : "") + ".";
        }
    }

    // If unsupported characters were found, add them to the result
    if (!unsupportedChars.isEmpty()) {
        return "Not found. Unsupported characters: " + unsupportedChars.toStdString();
    }

    return "Not found with mismatches.";
}

// Function to read the FASTA file
vector<string> readFastaFile(const string& filename) {
    ifstream file(filename);
    vector<string> sequences;
    string line;
    string currentSequence;

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return sequences;
    }

    while (getline(file, line)) {
        if (line.empty()) continue; // Skip empty lines
        if (line[0] == '>') {
            if (!currentSequence.empty()) {
                sequences.push_back(currentSequence);
                currentSequence.clear();
            }
        } else {
            currentSequence += line;
        }
    }
    if (!currentSequence.empty()) {
        sequences.push_back(currentSequence);
    }

    file.close();
    return sequences;
}

// Function to load sequences from a FASTA file and insert them into the Trie
void loadSequencesFromFile(const string& filename, Trie& trie, MainWindow* mainWindow) {
    // Clear the previous output before starting new insertions
    mainWindow->clearResults();

    vector<string> sequences = readFastaFile(filename);
    if (sequences.empty()) {
        mainWindow->displayMessage("No sequences found in the file.");
        return;
    }

    for (const string& seq : sequences) {
        trie.insert(seq, mainWindow);  // Pass mainWindow to show in GUI
    }
    mainWindow->displayMessage("All sequences loaded into the Trie.");
}






























