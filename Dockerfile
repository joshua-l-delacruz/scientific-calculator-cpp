# ============================================
# Build stage
# ============================================
FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# Install C++ compiler, CMake, Git,
# and libraries required by Drogon
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libjsoncpp-dev \
    uuid-dev \
    zlib1g-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Download Drogon
WORKDIR /tmp

RUN git clone --depth 1 https://github.com/drogonframework/drogon.git

# Build Drogon
WORKDIR /tmp/drogon

RUN git submodule update --init

RUN mkdir build && \
    cd build && \
    cmake .. \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_CTL=OFF \
        -DBUILD_ORM=OFF && \
    cmake --build . -j2 && \
    cmake --install .

# ============================================
# Build our scientific calculator
# ============================================

WORKDIR /app

COPY backend/ ./backend/

WORKDIR /app/backend

RUN mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . -j2

# ============================================
# Runtime stage
# ============================================
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libjsoncpp25 \
    libssl3 \
    zlib1g \
    uuid-runtime \
    && rm -rf /var/lib/apt/lists/*

# Copy Drogon libraries from builder
COPY --from=builder /usr/local/lib/ /usr/local/lib/

# Copy our compiled C++ calculator
COPY --from=builder \
    /app/backend/build/scientific-calculator \
    /usr/local/bin/scientific-calculator

RUN ldconfig

EXPOSE 8080

CMD ["scientific-calculator"]
