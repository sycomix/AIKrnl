FROM ubuntu:22.04

# Install build tools and pin gcc-12
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends \
        build-essential gcc-12 g++-12 make ca-certificates wget git && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-12 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-12 100 && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY . /workspace

# Build artifacts in image so running the container executes tests quickly
RUN make clean && make all

CMD ["make","test"]
