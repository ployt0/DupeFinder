// #define CATCH_CONFIG_MAIN // provides main(); this line is required in only one .cpp file
// #include "catch_amalgamated.hpp"
#include <catch2/catch_all.hpp>
#include "dupeFinder.h"
#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>


TEST_CASE( "Distinguishes graphic file extensions." ) {
    REQUIRE(isGraphic(std::filesystem::path("test.png")));
    REQUIRE(isGraphic(std::filesystem::path("test.webp")));
    REQUIRE(!isGraphic(std::filesystem::path("test.txt")));
}

TEST_CASE( "Hashes file contents." ) {
    REQUIRE(computeHash("test_resources/hashable_lorem1.txt") == "73c9c564ea10328e91046fbf18b8998a8aef065f567422c1fcc546e841c18584");
    REQUIRE(computeHash("test_resources/hashable_lorem2.txt") == "ab1898f2c5582b2d0d7633ed193c6de9cb781ff2bc8157597c5b07dbc6ce1f50");
    // std::filesystem::__cxx11::filesystem_error,
    REQUIRE_THROWS(
        computeHash("test_resources/unhashable_lorem.xxx")
    );
}

TEST_CASE( "Strings can become lowercase." ) {
    REQUIRE(stringToLC("DoWnInUbOnGo") == "downinubongo");
    REQUIRE(stringToLC("hashable_lorem...") == "hashable_lorem...");
}

TEST_CASE( "jsonifyMap prints _only_ duplicated hashes when not verbose." ) {
    std::ostringstream oss;
    std::unordered_map<std::string, std::vector<std::string>> hashes = {
        {"8c975a8795cf4ca50a14cfc889bb6e", {"ab", "cd"}},
        {"2b27bcc774e240d3ecfcf71a7dc064", {"ef", "gh"}},
        {"ce87c7d0de12bb9794890b44f0e1ba", {"ij"}},
    };
    REQUIRE(jsonifyMap(hashes, false, oss) == 5);
    REQUIRE(oss.str() == "{\n"
        "  \"2b27bcc774e240d3ecfcf71a7dc064\": [\n"
        "    \"ef\",\n"
        "    \"gh\"\n"
        "  ],\n"
        "  \"8c975a8795cf4ca50a14cfc889bb6e\": [\n"
        "    \"ab\",\n"
        "    \"cd\"\n"
        "  ]\n"
        "}\n");
}

TEST_CASE( "jsonifyMap prints all hashes when verbose." ) {
    std::ostringstream oss;
    std::unordered_map<std::string, std::vector<std::string>> hashes = {
        {"8c975a8795cf4ca50a14cfc889bb6e", {"ab", "cd"}},
        {"2b27bcc774e240d3ecfcf71a7dc064", {"ef", "gh"}},
        {"ce87c7d0de12bb9794890b44f0e1ba", {"ij", "kl"}},
        {"67a395aa9f8f9a8818071c7bef3dbd", {"mn", "op"}},
        {"6e7a35cd46b99e0ffbb781bb181a3e", {"qr"}},
    };
    REQUIRE(jsonifyMap(hashes, true, oss) == 9);
    for (const auto& [key, paths]: hashes) {
        REQUIRE(oss.str().find(key) != std::string::npos);
    }
}


// const std::string safePath(const char *s, const std::string sep) {
//     // const char sep = std::filesystem::path::preferred_separator;
//     const std::string result = std::regex_replace( s, std::regex( "\\" ), sep );
//     return result;
// }

const std::string safePath(const char *s, const std::string sep2) {
    const char sep = std::filesystem::path::preferred_separator;
    std::string s2(s);
    std::replace( s2.begin(), s2.end(), '\\', sep);
    return s2;
}

std::vector<std::string> sortedVect(std::vector<std::string> v) {
    std::sort(v.begin(), v.end());
    return v;
}

#ifdef _WIN32
const std::string sep1("\\");
const std::string sep2("\\\\");
TEST_CASE( "Strings get escape sequence path seps escaped." ) {
    std::string actual = escSeps("abc");
    REQUIRE("abc" == "abc");
    REQUIRE(escSeps("a\\bc") == "a\\\\bc");
    REQUIRE(escSeps("a\\b\\c") == "a\\\\b\\\\c");
}
#else
const std::string sep1("/");
const std::string sep2("/");
#endif


TEST_CASE( "Walking file hierarchy finds nested duplicates." ) {
    std::unordered_map<std::string, std::vector<std::string>> hashes{};
    shaWalk(hashes, "test_resources", true, true, false);
    REQUIRE(hashes.size() == 11);
    std::vector<std::string> diningPaths = {
        safePath("test_resources\\icons\\alfresco.jpg", sep1),
        safePath("test_resources\\icons\\promoted\\alfresco.jpg", sep1),
        safePath("test_resources\\icons\\promoted\\dining.jpg", sep1),
        safePath("test_resources\\resources\\alfresco.jpg", sep1),
    };
    REQUIRE(sortedVect(hashes["1e0d79ef6481214b9dca849fa7a7dd34b360d73cacface210a7ed571ab4da9b7"]) == sortedVect(diningPaths));
}


TEST_CASE( "Walking same file hierarchy twice ignores duplication due to same paths." ) {
    std::unordered_map<std::string, std::vector<std::string>> hashes{};
    shaWalk(hashes, "test_resources", true, true, false);
    shaWalk(hashes, "test_resources", true, true, false);
    REQUIRE(hashes.size() == 11);
    std::vector<std::string> diningPaths = {
        safePath("test_resources\\icons\\alfresco.jpg", sep1),
        safePath("test_resources\\icons\\promoted\\alfresco.jpg", sep1),
        safePath("test_resources\\icons\\promoted\\dining.jpg", sep1),
        safePath("test_resources\\resources\\alfresco.jpg", sep1),
    };
    REQUIRE(sortedVect(hashes["1e0d79ef6481214b9dca849fa7a7dd34b360d73cacface210a7ed571ab4da9b7"]) == sortedVect(diningPaths));
}


TEST_CASE( "Walking hierarchy has option to ignore subdirectories." ) {
    std::unordered_map<std::string, std::vector<std::string>> hashes{};
    shaWalk(hashes, safePath("test_resources\\icons", sep1), true, true, true);
    std::vector<std::string> path1 = sortedVect({
         safePath("test_resources\\icons\\artisan.jpg", sep1),
         safePath("test_resources\\icons\\beret.jpg", sep1),
    });
    std::vector<std::string> path2 = sortedVect({
         safePath("test_resources\\icons\\astronaut.jpg", sep1),
         safePath("test_resources\\icons\\croissant.jpg", sep1),
    });
    REQUIRE(sortedVect(hashes["a4e0c1efec3609beb79096d6c510f5f7892f3a2c88f5328ea4bc58dd4ea905d7"]) == path1);
    REQUIRE(sortedVect(hashes["00589e4d38f1c6071a5f901d2cb88a58274226e69f807bb7b7b1e21087ce79f1"]) == path2);
    int scanCount = 0;
    int dupeCount = 0;
    for (const auto& [key, paths] : hashes) {
        scanCount += paths.size();
        if (paths.size() >= 2)
            dupeCount++;
    }
    std::vector<std::string> diningPaths = {
         safePath("test_resources\\icons\\alfresco.jpg", sep1) 
    };
    REQUIRE(hashes["1e0d79ef6481214b9dca849fa7a7dd34b360d73cacface210a7ed571ab4da9b7"] == diningPaths);

    hashes.clear();
    shaWalk(hashes, "test_resources", true, true, true);
    REQUIRE(hashes.size() == 0);
}



// TEST_CASE( "Will fail, watch!" ) {
//     REQUIRE(1 == 2);
// }

