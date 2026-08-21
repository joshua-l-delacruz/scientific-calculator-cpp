# ============================================================
# STAGE 1 — BUILD
# ============================================================

FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive


# ------------------------------------------------------------
# Install build dependencies
# ------------------------------------------------------------

RUN apt-get update && \
    apt-get install -y \
        build-essential \
        cmake \
        git \
        libjsoncpp-dev \
        uuid-dev \
        zlib1g-dev \
        libssl-dev \
    && rm -rf /var/lib/apt/lists/*


# ------------------------------------------------------------
# Download Drogon
# ------------------------------------------------------------

WORKDIR /tmp


RUN git clone \
    --depth 1 \
    https://github.com/drogonframework/drogon.git


WORKDIR /tmp/drogon


RUN git submodule update \
    --init


# ------------------------------------------------------------
# Build and install Drogon
# ------------------------------------------------------------

RUN mkdir build && \
    cd build && \
    cmake .. \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_CTL=OFF \
        -DBUILD_ORM=OFF && \
    cmake --build . -j2 && \
    cmake --install .


# ------------------------------------------------------------
# Copy project
# ------------------------------------------------------------

WORKDIR /app


COPY backend/ ./backend/

COPY frontend/ ./frontend/


# ------------------------------------------------------------
# Build C++ calculator
# ------------------------------------------------------------

WORKDIR /app/backend


RUN mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . -j2


# ============================================================
# STAGE 2 — RUNTIME
# ============================================================

FROM ubuntu:24.04


ENV DEBIAN_FRONTEND=noninteractive


# ------------------------------------------------------------
# Runtime libraries
# ------------------------------------------------------------

RUN apt-get update && \
    apt-get install -y \
        libjsoncpp25 \
        libssl3 \
        zlib1g \
        uuid-runtime \
    && rm -rf /var/lib/apt/lists/*


# ------------------------------------------------------------
# Runtime working directory
# ------------------------------------------------------------

WORKDIR /app


# ------------------------------------------------------------
# Copy Drogon libraries
# ------------------------------------------------------------

COPY --from=builder \
    /usr/local/lib/ \
    /usr/local/lib/


# ------------------------------------------------------------
# Copy compiled C++ application
# ------------------------------------------------------------

COPY --from=builder \
    /app/backend/build/scientific-calculator \
    /usr/local/bin/scientific-calculator


# ------------------------------------------------------------
# Copy frontend
# ------------------------------------------------------------

COPY --from=builder \
    /app/frontend \
    /app/frontend


# ------------------------------------------------------------
# Refresh linker cache
# ------------------------------------------------------------

RUN ldconfig


# ------------------------------------------------------------
# Default port
# ------------------------------------------------------------

ENV PORT=8080


EXPOSE 8080


# ------------------------------------------------------------
# Start C++ application
# ------------------------------------------------------------

CMD ["scientific-calculator"]
