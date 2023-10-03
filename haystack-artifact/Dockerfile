FROM ubuntu:18.04
# install the required dependencies
ENV TZ=Europe/Zurich
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    wget \
    python3 \
    python3-pip \
    python3-dev \
    python3-setuptools \
    python3-wheel \
    aufs-tools \
    build-essential \
    automake \
    libtool \
    pkg-config \
    cmake \
    cmake-curses-gui \
    zlib1g-dev \
    libntl-dev \
    libgmp-dev \
    libyaml-cpp-dev \
    libboost-dev \
    libboost-program-options-dev \
    llvm-dev \
    libclang-dev \
    libglib2.0-dev \
    libpixman-1-dev

RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1 \
    && update-alternatives --install /usr/bin/pip pip /usr/bin/pip3 1
RUN pip3 install --upgrade pip requests setuptools pipenv
Run pip3 install scipy pandas matplotlib
WORKDIR /home

# build the depenencies
RUN git clone https://github.com/google/googletest && \
    cd googletest && \
    cmake . && \
    make -j4 && \
    make install && \
    cd ..

RUN git clone https://github.com/spcl/haystack-isl.git && \
    cd haystack-isl && \
    git checkout a457494f90b58fddd7a71cf27e5282cfc74c55ef && \
    sed -i "s|git@github.com:|https://github.com/|" .gitmodules && \
    git submodule init && \
    git submodule update && \
    ./autogen.sh && \
    ./configure && \
    make -j4 && \
    make install && \
    cd ..    

RUN git clone git://repo.or.cz/barvinok.git && \
    cd barvinok && \
    git checkout 63131f36c955c41831e226e72621efb3182af14d && \
    git submodule init polylib && \
    git submodule update polylib && \
    ./autogen.sh && \
    ./configure --with-isl=system && \
    make -j4 && \
    make install && \
    cd ..

RUN git clone git://repo.or.cz/pet.git && \
    cd pet && \
    git checkout d378c2b674f148466719ddde6d3c7e7f9ca7455b && \
    ./autogen.sh && \
    ./configure --with-isl=system && \
    make -j4 && \
    make install && \
    cd ..

RUN wget ftp://ftp.cs.wisc.edu/markhill/DineroIV/d4-7.tar.gz && \
    tar -xf d4-7.tar.gz && \
    cd d4-7 && \
    ./configure && \
    make && \
    cp dineroIV /usr/local/bin && \
    cd ..
    
RUN git clone https://github.com/atos-tools/qemu/ && \
    cd qemu && \
    git fetch && \
    git checkout stable-3.0.plugins && \
    ./configure --disable-system --enable-tcg-plugin  --target-list=x86_64-linux-user && \ 
    make -j4 && \
    make install && \
    cd ..

ENV PATH $PATH:/usr/bin:/usr/local/bin:/home/haystack/build/src
ENV LD_LIBRARY_PATH $LD_LIBRARY_PATH:/usr/lib:/usr/local/lib
ENV REPETITIONS 1

# copy haystack and the experimental setup from the host
ADD /cache-model/ haystack
ADD /experiments/ experiments

# generate all haystack binaries needed for the experiments
RUN cd haystack && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=baseline -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/baseline && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=noequal -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/noequal && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=noraster -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/noraster && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=onecache -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/onecache && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=threecaches -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/threecaches && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=enumerateall -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/enumerateall && \
    cmake -DCMAKE_BUILD_TYPE=Release -DEXPERIMENT=polycache -DPOLY_BASE=/usr ../ && \
    make -j4 StackDistance && \
    cp ./src/StackDistance ../../experiments/executables/polycache && \
    cd ../../



	 
