[![MSYS2 C++ CI](https://github.com/ployt0/DupeFinder/actions/workflows/msys2-cpp.yml/badge.svg)](https://github.com/ployt0/DupeFinder/actions/workflows/msys2-cpp.yml) [![Ubuntu C++ CI](https://github.com/ployt0/DupeFinder/actions/workflows/linux-cpp.yml/badge.svg)](https://github.com/ployt0/DupeFinder/actions/workflows/linux-cpp.yml) [![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fployt0%2FDupeFinder.svg?type=shield&issueType=license)](https://app.fossa.com/projects/git%2Bgithub.com%2Fployt0%2FDupeFinder?ref=badge_shield&issueType=license) [![codecov](https://codecov.io/gh/ployt0/DupeFinder/graph/badge.svg?token=NXCS61I997)](https://codecov.io/gh/ployt0/DupeFinder)

To help cleaning up storage.

C++ so it's fast. JSON output so Python can process further.

todo: install package in msys, cmake

Steps to build Catch2 instead of amalgamated:

```
$ git clone https://github.com/catchorg/Catch2.git
$ cd Catch2
$ cmake -Bbuild -H. -DBUILD_TESTING=OFF
#$ cmake --build build/ --target install --prefix just_for_config_hpp
```

We don't even need catch_user_config.hpp. An empty one does fine.

`Catch2\build\src` is populated by build and contains the very same static libraries that would be installed, `libCatch2.a` and `libCatch2Main.a`.

Copy these and the header (if any) into the source directory. I'll do this in GitHub actions.

