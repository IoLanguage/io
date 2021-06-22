FROM ubuntu:latest

# Install build dependencies
RUN apt-get update && \
    DEBIAN_FRONTEND=noninteractive \
    apt install -y tzdata && \
    apt-get install -y \
    build-essential \
    git \
    cmake

# clone latest from Io repository into src/io
WORKDIR /src
RUN git clone --recursive https://github.com/IoLanguage/io.git

# build and install Io
WORKDIR /src/io
RUN mkdir build 
WORKDIR /src/io/build
RUN cmake ..
RUN make
RUN make install

ENTRYPOINT [ "bash" ]