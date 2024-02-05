/**
 * Searches the given root directories for images and hashes their contents
 * to discover duplicates. Given the nested nature of hierarchies, there is
 * no option to copy out a single copy, because I can't guess which file name
 * or path you would prefer. The textual output should be fed into custom
 * scripts to decide or be processed manually.
 */

#include <iostream>
#include <vector>
#include <fstream>
#include <regex>
#include <filesystem>
#include <unordered_map>
#include <unordered_set>
#include "dupeFinder.h"
#include "sha256.h"
#include <algorithm>


std::string escSeps(std::string s) {
#ifdef _WIN32
    return std::regex_replace(s, std::regex(R"(\\)"), R"(\\)");
    // return std::regex_replace( s, std::regex( "\\" ), "\\\\" );
#else
    return std::string(s);
#endif
}


int jsonifyMap(
        const std::unordered_map<std::string, std::vector<std::string>>& m,
        const bool verbose,
        std::ostream& os
) {
    int scanCount = 0;
    bool needNL = false;
    os << "{" << std::endl;
    // C++17 iteration:
    for (const auto& [key, paths] : m) {
        scanCount += paths.size();
        if (paths.size() < 2 and !verbose)
            continue;
        if (needNL)
            os << "," << std::endl;

        os << "  \"" << key << "\": [" << std::endl;
        // todo, debating whether to double backslashes.
        // Singly, they work perfectly in UCRT64, but vanish
        // upon clicking the link shown in Git Bash.
        // Doubled up works slightly better, works with `jq '.'`,
        // but looks less natural.
        for (int i = 0; i < paths.size(); ++i) {
            os << "    \"" << escSeps(paths[i]) << "\"";
            if (i != paths.size() - 1)
                os << ",";
            os << std::endl;
        }
        os << "  ]";
        needNL = true;
    }
    if (needNL)
        os << std::endl;
    os << "}" << std::endl;
    return scanCount;
}


std::string stringToLC(const std::string &inpStr) {
    std::stringstream ss;
    for(const char &elem : inpStr)
        ss << static_cast<char>(std::tolower(elem));
    return ss.str();
}


bool isGraphic(const std::filesystem::path& path) {
    std::string extensionLC = stringToLC(path.extension().string());
    std::string endings[] = {".jpg", ".png", ".jpeg", ".webp", ".gif", ".bmp"};
    for (const std::string &ending: endings)
       if (extensionLC == ending)
           return true;
    return false;
}


/**
 * Computes SHA256 hash of file at the path provided.
 */
std::string computeHash(const std::string& fileName) {
    static const size_t BufferSize = 512*1024;
    static char buffer[BufferSize];
    static std::ifstream f;
    static SHA256 digestSha2;
    digestSha2.reset();
    f.open(fileName, std::ios::binary);
    if (!f) {
        throw std::runtime_error("no such file found: " + fileName);
    }
    std::istream* input = &f;
    while (*input) {
        input->read(buffer, BufferSize);
        std::size_t numBytesRead = size_t(input->gcount());
        digestSha2.add(buffer, numBytesRead);
    }
    f.close();
    return digestSha2.getHash();
}


void attemptDirHash(
        std::unordered_map<std::string, std::vector<std::string>>& hashes,
        std::unordered_set<std::string>& visitedPaths,
        const std::string& pathStr,
        const WalkOptions& walkOpts
) {
    if (!walkOpts.shallow) {
        try {
            shaWalk(hashes, visitedPaths, pathStr, walkOpts);
        }
        catch (std::filesystem::__cxx11::filesystem_error& e) {
            if (!walkOpts.quiet)
                std::cout << "ERROR " << e.what() << " " << std::endl;
        }
    }
}


/**
 * This is itself recursive because, at least using GNU CPP in Windows,
 * I am unable to detect symlinks, such as "\Users\me\AppData\Local\Application Data",
 * which points to the containing directory. `recursive_directory_iterator`
 * cannot progress its iteration. `is_symlink`, which wouldn't have
 * fixed it anyway, also failed.
 */
void shaWalk(
        std::unordered_map<std::string, std::vector<std::string>>& hashes,
        std::unordered_set<std::string>& visitedPaths,
        const std::string& sourceDir,
        const WalkOptions& walkOpts
) {
    std::filesystem::directory_iterator entry(sourceDir);
    std::filesystem::directory_iterator end;
    if (visitedPaths.find(sourceDir) != visitedPaths.end()) {
        return;
    }
    visitedPaths.insert(sourceDir);
    while (entry != end) {
        const std::string pathStr = entry->path().string();
        if (std::filesystem::is_directory(entry->path())) {
            attemptDirHash(hashes, visitedPaths, pathStr, walkOpts);
        } else if (!walkOpts.imagesOnly || isGraphic(entry->path())) {
            const std::string hexStr = computeHash(pathStr);
            std::vector<std::string> &paths = hashes[hexStr];
            paths.push_back(pathStr);
        }
        entry++;
    }
}


void showUsage(const char *progName, const char *commitHash, const char *buildDate, std::ostream& os) {
    os << "usage: " << progName <<" [-a] [-v] [-q] [-s] <root_directory(s)>" << std::endl;
    os << "     [-a] considers ALL files (default is just images)." << std::endl;
    os << "     [-v] VERBOSE. Output to include non-duplicates." << std::endl;
    os << "     [-q] QUIET. Doesn't report private directory errors." << std::endl;
    os << "     [-s] SHALLOW SPECIFIC. Doesn't search subdirectories." << std::endl;
    os << "     root_directory(s) will be searched for duplicate files." << std::endl;
    os << "Compiled " << buildDate << " from commit " << commitHash << std::endl;
}

