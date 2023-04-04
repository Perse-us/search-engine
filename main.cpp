
// In project used some texts from book of
// J.K. Rowling : "Harry Potter and the Sorcerer's Stone"
// Without any commercial purposes

// Work library
#include "converterJson.h"
#include "invertedIndex.h"
#include "searchServer.h"

// Service library
#include "testSuits.h"
#include "fileConvertor.h"

int main(int argc, char** argv) {

    // Initialization
    const char* engineVersion = "1.0";

    // Switch mode
    // Convert files for testing
    if (false) {
        ConverterJSON jsonConverter(engineVersion);
        FileConvertor::convertFileList(jsonConverter.getFileList());
    }

    try {
        ConverterJSON jsonConverter(engineVersion);

        // Create index
        InvertedIndex index;
        index.updateDocumentBase(jsonConverter.getTextDocuments());

        // Create searcher
        SearchServer search(index);
        search.setResultSize(jsonConverter.getResponsesLimit());

        // Engine search
        jsonConverter.putAnswer(search.search(
                jsonConverter.getRequests()));

    }
    catch (const std::invalid_argument& err) {
        // Critical errors
        std::cerr << err.what() << std::endl;
    }

    // Init and run Google tests
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
