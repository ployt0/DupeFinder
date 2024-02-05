#include "dupeFinder.h"
#include <iostream>
#include <vector>


#ifndef GIT_COMMIT
   #define GIT_COMMIT "unversioned"
#endif

#ifndef BUILD_DATE
   #define BUILD_DATE "2024"
#endif


int main( int argc, char *argv[] ) {
    bool imagesOnly = true;
    bool verbose = false;
    bool quiet = false;
    bool shallow = false;
    int rootsIndex = 1;
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            rootsIndex = i;
            break;
        }
        if (argv[i][1] == 'a') {
            imagesOnly = false;
        } else if (argv[i][1] == 'v') {
            verbose = true;
        } else if (argv[i][1] == 'q') {
            quiet = true;
        } else if (argv[i][1] == 's') {
            shallow = true;
        } else {
            showUsage(argv[0], GIT_COMMIT, BUILD_DATE, std::cout);
            return 1;
        }
    }
    if ( argc < rootsIndex + 1 ) {
        showUsage(argv[0], GIT_COMMIT, BUILD_DATE, std::cout);
        return 1;
    }
    std::unordered_map<std::string, std::vector<std::string>> hashes{};
    std::unordered_set<std::string> visitedPaths;
    WalkOptions walkOpts {
        .imagesOnly = imagesOnly,
        .quiet = quiet,
        .shallow = shallow
    };
    for (int i = rootsIndex; i < argc; ++i) {
        // std::cout << "Searching " << argv[i] <<" for duplicate images." << std::endl;
        shaWalk(
            hashes, visitedPaths,
            std::filesystem::canonical(argv[i]).string(), walkOpts
        );
    }
    int scanCount = jsonifyMap(hashes, verbose, std::cout);
    std::string description = imagesOnly ? " images." : " files.";
    // std::cout << "Scanned " << scanCount << description << std::endl;
}