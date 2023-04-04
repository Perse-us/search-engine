
#include "searchServer.h"

////////////////////////////////////////
/// Class interface
////////////////////////////////////////

SearchServer::SearchServer(const InvertedIndex& wordsIndexIn) :
        wordsIndex(wordsIndexIn) {}

SearchServer::SearchServer(const SearchServer& source) { copyAction(source); }

SearchServer &SearchServer::operator=(const SearchServer& source) {
    if (this == &source) return *this;
    copyAction(source);
    return *this;
}

std::vector<std::vector<std::pair<size_t, float>>> SearchServer::search(
        const std::vector<std::string>& queriesInput) const {

    std::vector<std::vector<std::pair<size_t, float>>> result;

    for (const std::string& currReqString : queriesInput) {

        // Get uniq words for request
        const std::vector<std::string> requestedWords = getUniqWords(currReqString);

        // Get absolute relevance & back sorted frequency of words in index results...
        // <docId, absRel> // Absolute relevance of the documents
        std::map<size_t, size_t> absDocsRel;
        // <sumFreq, <<docId>>> // Back sum of frequency of words in the documents entry
        std::map<size_t, std::vector<std::unordered_set<size_t>>> backSumFreqWords;
        // Fill source data for calculate relevance
        fillSourceDataForRelevance(wordsIndex, requestedWords,
                                   absDocsRel, backSumFreqWords);

        // Form using documents by words frequency
        // <docId, nonUsed> // Documents for result out and calculate relevance
        auto usedDocs = getUsedDocuments(backSumFreqWords);

        // Reverse absolute relevance of used docs (for ordered and filter)
        // <absRel, <docId>> // Absolute relevance of the documents (sorted by relevance)
        auto absRelOfDocs = getAbsRelUsedDocs(usedDocs, absDocsRel);

        // Prepare result set
        result.push_back(getOutResultPart(
                (resultSize == -1 ? usedDocs.size() : resultSize),
                absRelOfDocs));

    }

    return result;
}

void SearchServer::setResultSize(int resultSizeIn) {
    if (resultSizeIn < wholeResultSize) resultSizeIn = wholeResultSize;
    resultSize = resultSizeIn;
}

int SearchServer::getResultSize() const { return resultSize; }

////////////////////////////////////////
/// Private zone
////////////////////////////////////////

std::vector<std::string> SearchServer::getUniqWords(const std::string& string) {

    std::vector<std::string> result;
    std::unordered_set<std::string> usedWords;
    std::string currWord;
    auto stringIt = string.cbegin();
    while (stringIt != string.cend()) {
        bool isLetter = *stringIt >= 'A' && *stringIt <= 'Z'
                || *stringIt >= 'a' && *stringIt <= 'z';
        // Separate word
        if (isLetter) currWord += *stringIt;
        // Add word to collection
        if ((!isLetter || stringIt == string.end() - 1)
            && !currWord.empty()) {
            if (!usedWords.count(currWord)) {
                usedWords.emplace(currWord);
                result.push_back(currWord);
            }
            currWord.clear();
        }
        stringIt++;
    }

    return result;
}

// Fill data for calculate relevance from index results
// absDocsRel:          <docId, absRel>
// backSumFreqWords:    <sumFreq, <<docId>>>
void SearchServer::fillSourceDataForRelevance(
        const InvertedIndex& index,
        const std::vector <std::string>& requestedWords,
        std::map <size_t, size_t>& absDocsRel,
        std::map <size_t, std::vector<std::unordered_set < size_t>>>& backSumFreqWords) {

    // Get index result for every word of the request
    for (const std::string& currWord : requestedWords) {
        size_t wordFreqCount = 0;
        std::unordered_set<size_t> currDocIds;
        // Get and progress request result of every word
        std::vector<Entry> indexResForWord = index.getWordCount(currWord);
        for (const Entry& currEntry : indexResForWord) {
            // Calc abs relevance for every doc
            auto absDocRelIt = absDocsRel.find(currEntry.docId);
            if (absDocRelIt == absDocsRel.end())
                absDocRelIt = absDocsRel.emplace(currEntry.docId, 0).first;
            absDocRelIt->second += currEntry.count;
            // Add unique doc in to docs list
            currDocIds.emplace(currEntry.docId);
            // Calculate frequency of word
            wordFreqCount += currEntry.count;
        }
        // Add doc IDs in to frequency list
        backSumFreqWords[wordFreqCount].push_back(currDocIds);
    }
}

// Get used documents by index results (from rarely words)
// backSumFreqWords:    <sumFreq, <<docId>>>
std::map<size_t, bool> SearchServer::getUsedDocuments(
        const std::map<size_t, std::vector<std::unordered_set<size_t>>>& backSumFreqWords) {

    std::map<size_t, bool> result;
    //std::unordered_set<size_t> _result;
    if (backSumFreqWords.empty()) return result;

    // Check on results found
    // Loop by rarely words sets
    for (auto& currRareWordSet : backSumFreqWords.begin()->second) {
        // Create base
        // <docId> // Base set of documents (for every rarely word)
        std::unordered_set<size_t> baseDocs;
        for (size_t currDocId : currRareWordSet) {
            baseDocs.emplace(currDocId);
            result.emplace(currDocId, false);
        }
        // Loop for less rare words
        auto freqIt = backSumFreqWords.begin();
        while (++freqIt != backSumFreqWords.end()) {
            // Loop by words set of entries
            for (auto& currWordSet : freqIt->second) {
                // Check every base documents
                bool needAdd = true;
                for (size_t checkedDocId : baseDocs)
                    if (!currWordSet.count(checkedDocId)) {
                        needAdd = false;
                        break;
                    }
                // Add documents current set if found all base documents
                if (needAdd)
                    for (size_t currDocId : currWordSet) // Loop by doc IDs of set
                        result.emplace(currDocId, false);
            }
        }
    }

    return result;
}

// Get absolute relevance of selected document (sorted (revers) by relevance) for out
// usedDocs:    <docId, nonUsed>
// absDocsRel:  <docId, absRel>
std::map<size_t, std::vector<size_t>> SearchServer::getAbsRelUsedDocs(
        const std::map<size_t, bool>& usedDocs,
        const std::map<size_t, size_t>& absDocsRel) {

    // <absRel, <docId>> // Absolute
    std::map<size_t, std::vector<size_t>> result;

    for (const auto currDocPair : usedDocs) {
        size_t currDocId = currDocPair.first, currRel = 0;
        auto absDocsRelIt = absDocsRel.find(currDocId);
        if (absDocsRelIt != absDocsRel.end())
            currRel = absDocsRelIt->second;
        result[currRel].push_back(currDocId);
    }

    return result;
}

// Get part of answer result for a request
// <absRel, <docId>> // Absolute relevance of the documents (sorted by relevance)
std::vector<std::pair<size_t, float>> SearchServer::getOutResultPart(
        size_t currResultSize,
        const std::map<size_t, std::vector<size_t>>& absRelOfDocs) {

    std::vector<std::pair<size_t, float>> result;
    if (absRelOfDocs.empty()) return result;

    auto relOfDocsIt = absRelOfDocs.rbegin();
    size_t maxAbsRel = relOfDocsIt->first;
    if (maxAbsRel > 0)
        while (relOfDocsIt != absRelOfDocs.rend()
               && currResultSize > 0) {
            for (size_t currDocId : relOfDocsIt->second) {
                result.emplace_back(currDocId,
                                    (float) relOfDocsIt->first / (float) maxAbsRel);
                if (--currResultSize <= 0) break;
            }
            relOfDocsIt++;
        }

    return result;
}

void SearchServer::copyAction(const SearchServer &source) {
    wordsIndex = source.wordsIndex;
}
