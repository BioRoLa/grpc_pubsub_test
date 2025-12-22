# Corgi gRPC Power & Config Node Test Tools

## Overview
This repository (`corgi_grpc_test` branch) provides testing tools for the Corgi robot's gRPC communication architecture. It specifically targets the **Power Node** (FSM mode/power control) and **Config Node** (Motor parameter configuration) to verify Publish/Subscribe functionality and handshake mechanisms.

---
## Build Instructions

Follow these steps to download and compile the test tools:

```bash
# 1. Clone and Checkout
git clone [https://github.com/rumsey000/grpc_pubsub_test](https://github.com/rumsey000/grpc_pubsub_test)
cd grpc_pubsub_test
git checkout corgi_grpc_test

# 2. Build
mkdir build 
cd build 
cmake .. -DCMAKE_PREFIX_PATH=$HOME/corgi_sbrio_ws/install
make -j16
```
## PowerNodeTest Usage

```bash
# Terminal 1
grpccore
# Terminal 2 (inside build/)
./PowerNodeTestPub
# Terminal 3 (inside build/)
./PowerNodeTestSub
```

## ConfigNodeTest Usage

```bash
# Terminal 1
grpccore
# Terminal 2 (inside build/)
./ConfigNodeTestPub
# Terminal 3 (inside build/)
./ConfigNodeTestSub
```

## Directory Structure
Ensure your workspace is structured as follows to resolve `libcore` dependencies correctly:

```text
corgi_sbrio_ws/
├── fpga_driver/        # FPGA driver and Server logic
├── install/            # Installed libcore (Headers & Libs)
└── grpc_pubsub_test/   # This repository
