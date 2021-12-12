FROM ubuntu:20.04 AS build-env
RUN apt-get update && apt-get install -y tzdata
RUN apt-get install -y build-essential cmake dh-autoreconf git libc6 libicu-dev libmongoc-dev libprotobuf-dev libssl-dev libzstd-dev pkg-config

WORKDIR /work
RUN mkdir -p /work/local/include /work/local/lib

# picojson
WORKDIR /work/local/include
RUN git clone https://github.com/kazuho/picojson.git

# protoc
WORKDIR /work
RUN git clone https://github.com/protocolbuffers/protobuf.git && \
  cd protobuf && \
  git checkout "refs/tags/v3.15.8" && \
  git submodule update --init --recursive && \
  ./autogen.sh && \
  ./configure --prefix=/work/local && \
  make -j 4 && \
  make install

# libwebrtc
# ADD https://github.com/llamerada-jp/libwebrtc/releases/download/m96/libwebrtc-96.0.4664.45-linux-amd64.tar.gz /work/local
COPY libwebrtc-96.0.4664.45-linux-amd64.tar.gz /work/local/libwebrtc-96.0.4664.45-linux-amd64.tar.gz
RUN tar vzxf /work/local/libwebrtc-96.0.4664.45-linux-amd64.tar.gz -C /work/local

# google-glog
WORKDIR /work
RUN git clone https://github.com/google/glog.git && \
  cd glog && \
  git checkout "refs/tags/v0.4.0" && \
  ./autogen.sh && \
  ./configure && \
  make &&\
  make install

# libcolonio (tempolary)
RUN git clone --depth 1 https://github.com/colonio/colonio.git && \
  cp -R /work/colonio/src/colonio /work/local/include/
COPY libcolonio.a /work/local/lib/libcolonio.a

# simulation
ADD . /work/simulations
WORKDIR /work/simulations
RUN cmake -DLOCAL_ENV_PATH=/work/local . && \
  make

FROM ubuntu:20.04
RUN mkdir -p /usr/local/lib && \
  apt-get update && \
  apt-get install -y libssl1.1 libmongoc-1.0 valgrind
COPY --from=build-env /work/local/lib/libprotobuf.so.26.0.8 /usr/lib/libprotobuf.so.26
COPY --from=build-env /usr/local/lib/libglog.so.0.0.0 /usr/lib/libglog.so.0
COPY --from=build-env /work/simulations/simulations /simulations
ENTRYPOINT /simulations
