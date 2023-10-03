FROM ubuntu:18.04

# Timezone settings
ENV TZ=Europe/Berlin
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
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
    python3 \
    python3-pip \
    python3-dev \
    python3-setuptools \
    python3-wheel \
    python3-matplotlib \
    gpg-agent \
    clang \
    vim

# Install CMake version >= 3.15
RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc \
    2>/dev/null | gpg --dearmor - | \
    tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main' && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake

# Define the working directory of the container
WORKDIR /home

# Download and install PET
RUN git clone git://repo.or.cz/pet.git && \
    cd pet && \
    git checkout 12e72ffa7a7a81d695d9e71dee5e82b1cb66d98a && \
    ./get_submodules.sh && \
    ./autogen.sh && \
    ./configure && \
    make -j4 && \
    make check && \
    make install && \
    cd ..

ENV PATH $PATH:/usr/bin:/usr/local/bin
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/usr/lib:/usr/local/lib

RUN pip3 install numpy

# Copy required files from host
ADD /benchmark/ benchmark
ADD /data/ data
ADD /plots/ plots
ADD /scripts/ scripts
ADD /warping-cache-simulation/ warping-cache-simulation

RUN chmod +x scripts/plot-generation/*.sh && \
    chmod +x scripts/plot-generation/*.py && \
    chmod +x scripts/reproduction/*.sh && \
    chmod +x scripts/reproduction/*.py && \
    chmod +x warping-cache-simulation/*.sh
