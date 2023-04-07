
#include "converterJson.h"

////////////////////////////////////////
/// Class interface
////////////////////////////////////////

// ! Make throw std::invalid_argument for:
// - wrong config
// - engine version
ConverterJSON::ConverterJSON(const std::string& engineVersion) {

    // Get config information
    json jsonData = getJsonFromFile(configFilePath);
    if (!fillConfigInfo(jsonData))
        throw std::invalid_argument("config file is empty");
    if (config.version != engineVersion)
        throw std::invalid_argument(
            "config.json has incorrect file version");

    // Confirm configuration
    std::cout << '\"' << config.name << "\" v.: " << config.version << std::endl;
    std::cout << "Max responses: " << getResponsesLimit() << std::endl;

    // Added files & confirm
    fillFilesList(jsonData);
    if (files.empty()) std::cerr << "there are not files for search" << std::endl;
    else std::cout << "Added files for search: " << files.size() << std::endl;

}

ConverterJSON::ConverterJSON(const ConverterJSON& source) { copyAction(source); }

ConverterJSON &ConverterJSON::operator=(const ConverterJSON& source) {
    if (this == &source) return *this;
    copyAction(source);
    return *this;
}

std::vector<std::string> ConverterJSON::getTextDocuments() const {

    std::vector<std::string> result;

    // Get documents text
    for (const std::string& filePath : files) {

        std::string textFromFile;
        // Read file
        std::ifstream file(filePath, std::ios::binary);
        if (file.is_open()) {
            // Get file data
            const int buffSize = 500;
            char buff[buffSize + 1];
            while (!file.eof()) {
                file.read(buff, buffSize);
                buff[file.gcount()] = 0;
                textFromFile.append(buff);
            }
            // Close file
            file.close();
        }
        // Add file text to result
        result.push_back(textFromFile);
    }

    return result;
}

int ConverterJSON::getResponsesLimit() const {
    return config.maxResponses;
}

std::vector<std::string> ConverterJSON::getRequests() const {

    std::vector<std::string> result;

    try {
        json jsonData = getJsonFromFile(requestFilePath);
        json::value_type requestData = jsonData["requests"];
        if (requestData.is_array())
            // Add request strings
            for (json::value_type& requestText : requestData)
                result.push_back(requestText.is_string() ? requestText : "");
    } catch (const std::invalid_argument& err) {
        std::cerr << err.what() << std::endl;
    }

    return result;
}

void ConverterJSON::putAnswer(const std::vector<std::vector<std::pair<size_t, float>>>& answers) const {

    // Get all answers
    json jsonAnswers;
    const int reqNumSize = 3;
    const int reqNumLimit = 999;
    for (int i = 0; i < answers.size() && i < reqNumLimit; ++i) {
        // Init
        const auto& currAnswers = answers[i];
        json jsonCurrAnswer;

        // Form answer
        const bool hasResult = !currAnswers.empty();
        jsonCurrAnswer["result"] = hasResult;
        if (hasResult) {
            json jsonRelevance;
            bool writesList = currAnswers.size() > 1;
            for (const auto& currAnswer : currAnswers) {
                json jsonWrite;
                json& currWrites = (writesList ? jsonWrite : jsonCurrAnswer);
                currWrites["docid"] = currAnswer.first;
                currWrites["rank"] = (int) (currAnswer.second * 1000) / 1000.;
                if (currWrites == jsonWrite) jsonRelevance.push_back(jsonWrite);
            }
            if (!jsonRelevance.empty()) jsonCurrAnswer["relevance"] = jsonRelevance;
        }

        // Put current answer
        std::string fmtRequestNum = std::to_string(i + 1);
        fmtRequestNum.insert(0, reqNumSize - fmtRequestNum.size(), '0');
        jsonAnswers["request" + fmtRequestNum] = jsonCurrAnswer;
    }

    // Put all answers
    json jsonData;
    jsonData["answers"] = jsonAnswers;
    std::cout << "Answer JSON:\n";
    std::cout << jsonData.dump(2) << std::endl;

    // Write to file
    bool successWrite = false;
    std::ofstream file(answerFilePath);
    if (file.is_open()) {
        try {
            file << jsonData.dump(2);
            file.close();
            successWrite = true;
        } catch (...) { }
    }
    if (!successWrite) std::cerr << "file " << answerFilePath << " isn't wrote" << std::endl;
}

std::vector<std::string> ConverterJSON::getFileList() const { return files; }

////////////////////////////////////////
/// Private zone
////////////////////////////////////////

// ! Make throw std::invalid_argument for non exist file and wrong JSON format
json ConverterJSON::getJsonFromFile(const std::string& fileName) {

    std::ifstream file(fileName);
    if (!file.is_open()) throw std::invalid_argument(
                fileName + " file is missing");

    json jsonData;
    try {
        file >> jsonData;
    } catch (...) {
        file.close();
        throw std::invalid_argument(
                fileName + " file has wrong format");
    }

    file.close();
    return jsonData;

}

template <typename TJ, typename TD>
void ConverterJSON::getJsonParam(TJ& jsonSet, const char name[], TD& dest) {
    try {
        if (!jsonSet[name].is_null()) dest = jsonSet[name];
    } catch (...) { }
}

void ConverterJSON::copyAction(const ConverterJSON &source) {
    config = source.config;
    files = source.files;
}

// Return: true - success; false - empty config
bool ConverterJSON::fillConfigInfo(json& jsonData) {

    json::value_type cfgInfo = jsonData["config"];
    if (cfgInfo.empty()) return false;

    // Read config data
    getJsonParam(cfgInfo, "version", config.version);
    getJsonParam(cfgInfo, "name", config.name);
    getJsonParam(cfgInfo, "max_responses", config.maxResponses);
    getJsonParam(cfgInfo, "renew_time", config.renewTime);
    if (config.maxResponses < respMin) config.maxResponses = respMin;

    return true;
}

void ConverterJSON::fillFilesList(json& jsonData) {

    files.clear();
    json::value_type fileData = jsonData["files"];

    if (fileData.is_array()) {
        for (size_t i = 0; i < fileData.size(); ++i) {

            std::string addedFilePath;
            json::value_type filePath = fileData[i];

            // Check file
            if (filePath.is_string() && !std::string(filePath).empty()) {
                // Check existing file
                std::ifstream file(filePath);
                if (file.is_open()) addedFilePath = filePath;
                file.close();
            }

            // Add file
            files.push_back(addedFilePath);
            if (addedFilePath.empty()) {
                std::cerr << "wrong file name at pos.: " << i + 1 << ':' << std::endl
                    << to_string(filePath) << std::endl;
            }
        }
    }
}
