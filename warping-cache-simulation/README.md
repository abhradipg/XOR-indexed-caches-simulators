# Building and Running the Project + Some Notes

This is a CMake project. CMake version >= 3.15 is required.

## Dependencies
Only external library used is [PET](https://repo.or.cz/w/pet.git) (Polyhedral Extraction Tool).

These should be enough for downloading and installing PET:
```
sudo apt-get update && sudo apt-get install \
    git \
    wget \
    apt-utils \
    lsb-release \
    software-properties-common \
    build-essential \
    automake \
    autoconf \
    libtool \
    pkg-config \
    libyaml-cpp-dev \
    libgmp-dev \
    llvm-dev \
    libclang-dev \
    clang \
    python3 \
    python3-pip \
    python3-dev \
    python3-setuptools \
    python3-wheel \
    gpg-agent
```

Download and install PET:
```
git clone git://repo.or.cz/pet.git && \
    cd pet && \
    git checkout 12e72ffa7a7a81d695d9e71dee5e82b1cb66d98a && \
    ./get_submodules.sh && \
    ./autogen.sh && \
    ./configure && \
    make -j4 && \
    make check && \
    make install && \
    cd ..
```

## How to build
You can use `build-debug.sh` and `build-release.sh` scripts
```
./build-debug.sh
./build-release.sh
```

You can use these for building in a different folder too, e.g.:
```
cd ..
./warping-cache-simulation/build-release.sh
```

Alternatively:

* Debug build:
```
mkdir -p cmake-build-debug && cd cmake-build-debug || return
cmake ..
make
```
* Release build:
```
mkdir -p cmake-build-release && cd cmake-build-release || return
cmake ..
make
```

## How to run
The warping cache simulation takes as input a C source file, a cache config file, settings for warping, and a list of include dirs:
```
./warping-cache-simulation <SRC> <CACHE_CONFIG> -w <WARPING> -h <SMART_WARPING> -i <INCLUDE_DIRS>

# SRC: C source file
# CACHE_CONFIG: cache config file
# WARPING: true or false
# SMART_WARPING: true or false
# INCLUDE_DIRS: list of include dirs, separated by comma, default empty
```
Example:
```
# build the project
./build-release.sh

bin=cmake-build-release/src/warping-cache-simulation
src=example-source-files/example.c
cache_config=example-cache-configs/example.config

# example run
./$bin "$src" "$cache_config" -w true -h true
```

## Formatting
Format the code by running `format.sh`.

## Adding new source code inside a new folder
You should add the new files to `SOURCE_FILES` in `src/CMakeLists.txt`.

This is self-explanatory in `src/CMakeLists.txt`.
