
#include "invertedIndex.h"

////////////////////////////////////////
/// Class interface
////////////////////////////////////////

InvertedIndex::InvertedIndex(const InvertedIndex& source) { copyAction(source); }

InvertedIndex& InvertedIndex::operator=(const InvertedIndex &source) {
    if (this == &source) return *this;
    copyAction(source);
    return *this;
}

void InvertedIndex::updateDocumentBase(const std::vector<std::string>& inputDocs) {
    // Refresh origin data
    wordsIndex.clear();
    std::vector<std::unique_ptr<std::thread>> threads;

    // Words calculation in threads
    for (size_t docIndex = 0; docIndex < inputDocs.size(); ++docIndex) {
        threads.push_back(std::make_unique<std::thread>(indexText,
                                                        inputDocs[docIndex], docIndex,
                                                        &wordsIndex, &wordsIndexAccess));
    }

    // Wait threads join
    for (auto& currThread : threads)
        if (currThread->joinable()) currThread->join();

}

std::vector<Entry> InvertedIndex::getWordCount(const std::string& word) const {
    std::vector<Entry> result;
    auto foundWordPtr = wordsIndex.find(getLowCaseString(word));
    if (foundWordPtr != wordsIndex.end())
        result = foundWordPtr->second;

    return result;
}

////////////////////////////////////////
/// Private zone
////////////////////////////////////////

bool InvertedIndex::isLetter(char checkedChar) {
    return checkedChar >= 'a' && checkedChar <= 'z'
           || checkedChar >= 'A' && checkedChar <= 'Z';
}

char InvertedIndex::getLowCaseChar(char modChar) {
    if (modChar >= 'A' && modChar <= 'Z')
        modChar = (char) (modChar - 'A' + 'a');
    return modChar;
}

std::string InvertedIndex::getLowCaseString(const std::string& modString) {
    std::string result;
    for(char currCar : modString)
        result += getLowCaseChar(currCar);
    return result;
}

void InvertedIndex::indexText(const std::string &text, size_t docIndex,
                              std::map<std::string, std::vector<Entry>>* currWordsIndex,
                              std::mutex* currMutex){

    std::string currWord;
    std::string::const_iterator charPtr = text.cbegin();
    Entry newEntry {docIndex, 1};
    while(charPtr != text.cend()) {
        bool isCurrLetter = isLetter(*charPtr);
        if (isCurrLetter) currWord += getLowCaseChar(*charPtr);
        if ((!isCurrLetter || charPtr == text.cend() - 1)
            && !currWord.empty()) {

            // Write word to index
            currMutex->lock();
            {
                auto wordsIndexPtr = currWordsIndex->find(currWord);
                if (wordsIndexPtr == currWordsIndex->end()) {
                    currWordsIndex->insert(std::make_pair(currWord, std::vector<Entry> {newEntry}));
                } else {
                    // Find document entry
                    auto currEntryPtr = wordsIndexPtr->second.begin();
                    auto posToInsertPtr = wordsIndexPtr->second.begin();
                    while (currEntryPtr != wordsIndexPtr->second.end()) {
                        if (currEntryPtr->docId == docIndex) break;
                        if (currEntryPtr->docId < docIndex) posToInsertPtr = currEntryPtr + 1;
                        currEntryPtr++;
                    }
                    // Add new or refresh exist entry
                    if (currEntryPtr == wordsIndexPtr->second.end())
                        wordsIndexPtr->second.insert(posToInsertPtr, newEntry);
                    else currEntryPtr->count++;
                }
            }
            currMutex->unlock();

            currWord.clear();
        }
        charPtr++;
    }
}

void InvertedIndex::copyAction(const InvertedIndex &source) {
    wordsIndex = source.wordsIndex;
}
