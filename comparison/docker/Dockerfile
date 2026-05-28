FROM ubuntu:24.04

# 1. Install EVERYTHING (Build tools, runtime dependencies, and tracing tools)
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    build-essential \
    autoconf \
    automake \
    libtool \
    pkg-config \
    libssl-dev \
    libssl3 \
    iproute2 \
    iptables \
    iputils-ping \
    tcpdump \
    tshark \
    bash \
    git \
    && rm -rf /var/lib/apt/lists/*

# 2. Clone, compile, and install libcoap right here in the same environment
RUN git clone https://github.com/obgm/libcoap.git /build/libcoap
WORKDIR /build/libcoap
RUN ./autogen.sh && \
    ./configure --enable-dtls --with-openssl --disable-documentation && \
    make && \
    make install

# 3. Update the shared library cache so the system knows where to find libcoap
RUN ldconfig

# 4. Set up the active workspace directory for your custom application volumes
WORKDIR /app