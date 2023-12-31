#include "dupeFinder.h"
#include <iostream>
#include <vector>


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
            showUsage(argv);
            return 1;
        }
    }
    if ( argc < rootsIndex + 1 ) {
        showUsage(argv);
        return 1;
    }
    std::unordered_map<std::string, std::vector<std::string>> hashes{};
    for (int i = rootsIndex; i < argc; ++i) {
        // std::cout << "Searching " << argv[i] <<" for duplicate images." << std::endl;
        shaWalk(hashes, std::filesystem::canonical(argv[i]).string(),
                imagesOnly, quiet, shallow);
    }
    int scanCount = jsonifyMap(hashes, verbose, std::cout);
    std::string description = imagesOnly ? " images." : " files.";
    // std::cout << "Scanned " << scanCount << description << std::endl;
}