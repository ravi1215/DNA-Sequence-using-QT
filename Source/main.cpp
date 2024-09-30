#include "trie.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include "mainwindow.h"
#include <QApplication>

using namespace std;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;

    // Automatically load the FASTA file when the application starts
    loadSequencesFromFile("/Users/ravipandey/DNA/FASTA/sequence.fasta", w.getTrie(), &w);

    w.show();
    return a.exec();
}
