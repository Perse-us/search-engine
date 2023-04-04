
#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

struct Entry {
    size_t docId, count;

    // For test compare
    bool operator==(const Entry& other) const {
        return (docId == other.docId && count == other.count);
    }
};

class InvertedIndex {
public:
    // Class constructors
    InvertedIndex() = default;
    InvertedIndex(const InvertedIndex& source);
    InvertedIndex& operator=(const InvertedIndex& source);
    ~InvertedIndex() = default;

    void updateDocumentBase(const std::vector<std::string>& inputDocs);
    std::vector<Entry> getWordCount(const std::string& word) const;

private:
    std::map<std::string, std::vector<Entry>> wordsIndex;
    std::mutex wordsIndexAccess;

    static bool isLetter(char checkedChar);
    static char getLowCaseChar(char modChar);
    static std::string getLowCaseString(const std::string& modString);
    static void indexText(const std::string& text,
                          size_t docIndex,
                          std::map<std::string, std::vector<Entry>>* currWordsIndex,
                          std::mutex* currMutex);

    void copyAction(const InvertedIndex& source);

};
