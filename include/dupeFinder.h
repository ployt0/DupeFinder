#ifndef __DUPEFINDER_INCLUDED__
#define __DUPEFINDER_INCLUDED__

#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>

std::string escSeps(std::string s);

int jsonifyMap(
        const std::unordered_map<std::string, std::vector<std::string>>& m,
        const bool verbose,
        std::ostream& os
);

bool isGraphic(const std::filesystem::path& path);

std::string stringToLC(const std::string &inpStr);

std::string computeHash(const std::string& fileName);

void shaWalk(
        std::unordered_map<std::string, std::vector<std::string>>& hashes,
        const std::string& sourceDir,
        const bool imagesOnly,
        const bool quiet,
        const bool shallow
);

void showUsage(char *argv[]);

#endif // __DUPEFINDER_INCLUDED__