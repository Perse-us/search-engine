
#pragma once

#include <iostream>
#include <vector>
#include <fstream>

// Add JSON
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class ConverterJSON {
public:
    // Class constructors
    // ! Make throw - see more in .cpp
    ConverterJSON(const std::string& engineVersionIn);
    ConverterJSON(const ConverterJSON& source);
    ConverterJSON& operator=(const ConverterJSON& source);
    ~ConverterJSON() = default;

    // Get init files list
    std::vector<std::string> getTextDocuments() const;
    int getResponsesLimit() const;
    std::vector<std::string> getRequests() const;
    void putAnswer(const std::vector<std::vector<std::pair<size_t, float>>>& answers) const;
    std::vector<std::string> getFileList() const;

private:
    // Initialization set
    static const int respMin = 0;
    static const int respDefault = 5;
    struct Config {
        std::string name;
        std::string version;
        int maxResponses = respDefault;
        std::time_t renewTime = 0;
    };

    const char* configFilePath = "config.json";
    const char* requestFilePath = "requests.json";
    const char* answerFilePath = "answers.json";

    Config config;
    std::vector<std::string> files;

    // ! Make throw - see more in .cpp
    static json getJsonFromFile(const std::string& fileName);
    template <typename TJ, typename TD>
    static void getJsonParam(TJ& jsonSet, const char* name, TD& dest);

    void copyAction(const ConverterJSON& source);
    bool fillConfigInfo(json& jsonData);
    void fillFilesList(json& jsonData);

};
