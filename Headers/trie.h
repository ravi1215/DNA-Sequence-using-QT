#ifndef TRIE_H
#define TRIE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <QString>
#include <fstream>

// Forward declaration of MainWindow to avoid circular dependencies
class MainWindow;

using namespace std;

struct TrieNode {
    unique_ptr<TrieNode> children[4];
    bool isEndofSequence;

    TrieNode() : isEndofSequence(false) {
        for (int i = 0; i < 4; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
private:
    unique_ptr<TrieNode> root;

public:
    Trie() : root(make_unique<TrieNode>()) {}

    // Modify insert and searchWithAllowedMismatches to accept a MainWindow pointer
    void insert(const string& sequence, MainWindow* mainWindow);
    bool search(const string& sequence, MainWindow* mainWindow);
    bool searchFromPosition(TrieNode* node, const string& sequence, size_t index, MainWindow* mainWindow);
    bool searchWithAllowedMismatches(const string& sequence, int allowedMismatches, MainWindow* mainWindow, QString& unsupportedChars);

private:

    bool searchWithMismatch(TrieNode* node, const string& sequence, size_t index, int mismatchCount, int allowedMismatches, MainWindow* mainWindow, QString& unsupportedChars);
    int charToIndex(char c, MainWindow* mainWindow,  QString& unsupportedChars);
};

// Function declarations (only declarations)
vector<string> readFastaFile(const string& filename);
void loadSequencesFromFile(const string& filename, Trie& trie, MainWindow* mainWindow);
string searchDNASequence(Trie& trie, const string& sequence, int allowedMismatches, MainWindow* mainWindow);

#endif // TRIE_H

