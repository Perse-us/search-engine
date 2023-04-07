
#include "testSuits.h"

////////////////////////////////////////
/// InvertedIndexSuit
////////////////////////////////////////

void TestInvertedIndexFunctionality(
        const std::vector<std::string>& docs,
        const std::vector<std::string>& requests,
        const std::vector<std::vector<Entry>>& expected) {

    std::vector<std::vector<Entry>> result;

    InvertedIndex idx;
    idx.updateDocumentBase(docs);
    for(auto& request : requests) {
        std::vector<Entry> word_count = idx.getWordCount(request);
        result.push_back(word_count);
    }

    ASSERT_EQ(result, expected);
}

TEST(InvertedIndexSuit, BasicTest1) {
    const std::vector<std::string> docs = {
            "london is the capital of great britain",
            "a big ben is the nickname for the Great bell of the striking a clock"
    };
    const std::vector<std::string> requests = {"london", "the", "great", "a"};
    const std::vector<std::vector<Entry>> expected = {
            { {0, 1} },
            { {0, 1}, {1, 3} },
            { {0, 1}, {1, 1} },
            { {1, 2}}
    };

    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(InvertedIndexSuit, BasicTest2) {
    const std::vector<std::string> docs = {
            "Milk milk miLk milk water water water",
            "milk water wateR",
            "milk milk milk MILK milk water water water water water",
            "americano cappuccino"
    };
    const std::vector<std::string> requests = {"milk", "water", "cappuccino"};
    const std::vector<std::vector<Entry>> expected = {
            { {0, 4}, {1, 1}, {2, 5} },
            { {0, 3}, {1, 2}, {2, 5} },
            { {3, 1} }
    };

    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(InvertedIndexSuit, nceAndMissingWordTest) {
    const std::vector<std::string> docs = {
            "a b c d e f g h i j k l",
            "statement"
    };
    const std::vector<std::string> requests = {"m", "statement"};
    const std::vector<std::vector<Entry>> expected = {
            { },
            { {1, 1} }
    };

    TestInvertedIndexFunctionality(docs, requests, expected);
}

////////////////////////////////////////
/// SearchServerSuit
////////////////////////////////////////

TEST(SearchServerSuit, SimpleTest) {
    const std::vector<std::string> docs = {
            "milk milk milk milk water water water",
            "milk water water",
            "milk milk milk milk milk water water water water water",
            "americano cappuccino"
    };
    const std::vector<std::string> request = {"milk water", "sugar"};
    const std::vector<std::vector<std::pair<size_t, float>>> expected = {
            { {2, 1}, {0, 0.7}, {1, 0.3} },
            { }
    };
    InvertedIndex idx;
    idx.updateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<std::pair<size_t, float>>> result = srv.search(request);

    ASSERT_EQ(result, expected);
}

TEST(SearchServerSuit, Top5Test) {
    const std::vector<std::string> docs = {
            "london is the capital of great britain",
            "paris is the capital of france",
            "berlin is the capital of germany",
            "rome is the capital of italy",
            "madrid is the capital of spain",
            "lisboa is the capital of portugal",
            "bern is the capital of switzerland",
            "moscow is the capital of russia",
            "kiev is the capital of ukraine",
            "minsk is the capital of belarus",
            "astana is the capital of kazakhstan",
            "beijing is the capital of china",
            "tokyo is the capital of japan",
            "bangkok is the capital of thailand",
            "welcome to moscow the capital of russia the third rome",
            "amsterdam is the capital of netherlands",
            "helsinki is the capital of finland",
            "oslo is the capital of norway",
            "stockholm is the capital of sweden",
            "riga is the capital of latvia",
            "tallinn is the capital of estonia",
            "warsaw is the capital of poland",
    };
    const std::vector<std::string> request = {"moscow is the capital of russia"};
    const std::vector<std::vector<std::pair<size_t, float>>> expected = {
            {
                    {7, 1},
                    {14, 1},
                    {0, 0.666666667},
                    {1, 0.666666667},
                    {2, 0.666666667}
            }
    };

    InvertedIndex idx;
    idx.updateDocumentBase(docs);

    SearchServer srv(idx);
    srv.setResultSize(5);
    std::vector<std::vector<std::pair<size_t, float>>> result = srv.search(request);

    ASSERT_EQ(result, expected);
}
