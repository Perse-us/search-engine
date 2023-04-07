/////////////////////////////////
/// Technical module
/////////////////////////////////

#pragma once

#include <iostream>
#include <fstream>

class FileConvertor {
public:
    static void convertFile(const std::string& fileName) {
        saveTextToFile(getConvertedTextFromFile(fileName), fileName);
    }
    static void convertFileList(const std::vector<std::string>& files) {
        for (const std::string& fileName : files)
            convertFile(fileName);
    }
private:
    static std::string getAddStringByChar(char currChar) {

        std::string validChars = " .,!?;-\n";
        std::string result;

        if (validChars.find(currChar) != std::string::npos
            || currChar >= 'A' && currChar <= 'Z'
            || currChar >= 'a' && currChar <= 'z') {
            int repeatCnt = (currChar == ' ' ? 1 + rand() % 5 : 1);
            result.append(repeatCnt, currChar);
        }

        return result;
    }
    static std::string getConvertedTextFromFile(const std::string& fileName) {

        std::string result;
        std::string validChars = " .,!?;-\n";
        std::ifstream file(fileName, std::ios::binary);

        if (file.is_open()) {
            char currChar;
            while (!file.eof()) {
                file.read(&currChar, 1);
                if (file.gcount()) result.append(getAddStringByChar(currChar));
            }
            file.close();

            // Cleared final space
            while (!result.empty()) {
                char endChar = result.back();
                if (endChar == '\n' || endChar == ' ') result.pop_back();
                else break;
            }
        }

        return result;
    }
    static void saveTextToFile(const std::string& text,
                               const std::string& fileName) {
        std::ofstream file(fileName, std::ios::binary);
        if (file.is_open()) {
            file.write(text.c_str(), text.size());
            file.close();
            std::cout << "File \"" << fileName << "\" converted\n";
        }
    }
};

