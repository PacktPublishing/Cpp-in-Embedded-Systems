# Use Ubuntu 20.04 LTS as the base image
FROM ubuntu:20.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    wget \
    xz-utils \
    libncursesw5 \
    software-properties-common \
    cmake \ 
    make \
    && add-apt-repository ppa:deadsnakes/ppa \
    && apt-get update \
    && apt-get install -y python3.8 python3.8-distutils \
    && rm -rf /var/lib/apt/lists/*

# Set Python 3.8 as the default python3
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python3.8 1

# Download and extract arm-none-eabi-gcc
RUN wget -O arm-gnu-toolchain.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/13.2.rel1/binrel/arm-gnu-toolchain-13.2.rel1-x86_64-arm-none-eabi.tar.xz?rev=e434b9ea4afc4ed7998329566b764309&hash=CA590209F5774EE1C96E6450E14A3E26" \
    && tar -xf arm-gnu-toolchain.tar.xz -C /opt/ \
    && rm arm-gnu-toolchain.tar.xz

# Download and unpack Renode
RUN wget https://github.com/renode/renode/releases/download/v1.15.0/renode-1.15.0.linux-portable.tar.gz \
    && tar -xzvf renode-1.15.0.linux-portable.tar.gz -C /opt/ \
    && rm renode-1.15.0.linux-portable.tar.gz

# Add the toolchain and Renode bin directories to PATH
ENV PATH="/opt/arm-gnu-toolchain-13.2.Rel1-x86_64-arm-none-eabi/bin:/opt/renode_1.15.0_portable:${PATH}"

# Set the working directory
WORKDIR /workspace

# Keep the container running
CMD ["sleep", "infinity"]

