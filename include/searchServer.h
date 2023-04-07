
#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <map>
#include "invertedIndex.h"

class SearchServer {
public:
    // Class constructors
    SearchServer(const InvertedIndex& wordsIndexIn);
    SearchServer(const SearchServer& source);
    SearchServer& operator=(const SearchServer& source);
    ~SearchServer() = default;

    std::vector<std::vector<std::pair<size_t, float>>> search(
            const std::vector<std::string>& queriesInput) const;
    void setResultSize(int resultSizeIn);
    int getResultSize() const;

private:
    const int wholeResultSize = -1;
    int resultSize = wholeResultSize; // -1 - whole result
    InvertedIndex wordsIndex;

    static std::vector<std::string> getUniqWords(const std::string& string);
    static void fillSourceDataForRelevance(
            const InvertedIndex& index,
            const std::vector<std::string>& requestedWords,
            std::map<size_t, size_t>& absDocsRel,
            std::map<size_t, std::vector<std::unordered_set<size_t>>>& backSumFreqWords);
    static std::map<size_t, bool> getUsedDocuments(
            const std::map<size_t, std::vector<std::unordered_set<size_t>>>& backSumFreqWords);
    static std::map<size_t, std::vector<size_t>> getAbsRelUsedDocs(
            const std::map<size_t, bool>& usedDocs,
            const std::map<size_t, size_t>& absDocsRel);
    static std::vector<std::pair<size_t, float>> getOutResultPart(
            size_t currResultSize,
            const std::map<size_t, std::vector<size_t>>& absRelOfDocs);

    void copyAction(const SearchServer& source);

};