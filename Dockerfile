# SPDX-License-Identifier: Apache-2.0
# CI container for fusain library testing

ARG ZEPHYR_SDK_VERSION=0.17.4

# Stage 1: System packages and SDK setup (root)
FROM ubuntu:24.04 AS base

ARG ZEPHYR_SDK_VERSION

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-multilib \
    g++-multilib \
    git \
    cmake \
    ninja-build \
    device-tree-compiler \
    clang-format \
    python3-pip \
    python3-venv \
    curl \
    xz-utils \
    && rm -rf /var/lib/apt/lists/*

RUN curl -sL https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz -o /tmp/sdk.tar.xz \
    && mkdir -p /opt/zephyr-sdk \
    && tar xf /tmp/sdk.tar.xz -C /opt/zephyr-sdk --strip-components=1 \
    && rm /tmp/sdk.tar.xz

ENV ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk

# Stage 2: Python environment and tools
FROM base AS ci

RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --no-cache-dir west gcovr

RUN sh -c "$(curl -sL https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin

RUN mkdir -p /workdir && chmod 777 /workdir
WORKDIR /workdir
