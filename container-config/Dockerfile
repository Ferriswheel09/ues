# Use full Ubuntu as base
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# Install general workspace tools and full development environment
RUN apt update && apt install -y \
    build-essential \
    gcc \
    g++ \
    cmake \
    make \
    git \
    curl \
    wget \
    vim \
    iputils-ping \
    net-tools \
    libstdc++6 \
    libc6

# Set working directory
WORKDIR /workspace

# Copy your source
COPY ./client_server.cpp .

# Build dynamically (no -static)
RUN g++ -Wall -Wextra -o client_server client_server.cpp

# Default entrypoint
ENTRYPOINT ["./client_server", "server"]
