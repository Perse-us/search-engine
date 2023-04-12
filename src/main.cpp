
// Work library
#include "converterJson.h"
#include "invertedIndex.h"
#include "searchServer.h"

int main(int argc, char** argv) {

    // Initialization
    const char* engineVersion = "1.0";

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

    return 0;
}
