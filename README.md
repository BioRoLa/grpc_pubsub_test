# Corgi gRPC Power & Config Node Test Tools

## Overview
This repository provides testing tools for the Corgi robot's gRPC communication architecture. It specifically targets the **Config Node** to verify Publish/Subscribe functionality.

---
## Build Instructions

Follow these steps to download and compile the test tools:

```bash
# 1. Clone and Checkout
git clone https://github.com/rumsey000/grpc_pubsub_test
cd grpc_pubsub_test
git checkout corgi_grpc_test

# 2. Build
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=$HOME/corgi_ws/install -DCMAKE_INSTALL_PREFIX=$HOME/corgi_ws/install -DOPENSSL_ROOT_DIR=$HOME/corgi_ws/install/ssl
make -j16
```

## Run ConfigNodeTest

```bash
# Terminal 1
grpccore
# Terminal 2
cd ~/corgi_ws/grpc_pubsub_test/build
./ConfigNodeTestPub
# Terminal 3
cd ~/corgi_ws/grpc_pubsub_test/build
./ConfigNodeTestSub
```

## Directory Structure

```text
corgi_sbrio_ws/
├── fpga_driver/        # FPGA driver and Server logic
├── install/            # Installed libcore (Headers & Libs)
└── grpc_pubsub_test/   # This repository
