# LWS Minimal Client

A concise, optimized C implementation of a secure WebSocket client using `libwebsockets`. Designed for extreme performance and binary self-sufficiency in financial data workflows.

## Features
- **SSL/TLS Support**: Robust handshake handling with `LCCSCF_USE_SSL`.
- **Optimized Build**: Uses `-O2`, `-march=native`, and `-flto` for minimal latency.
- **Clean Architecture**: Separates connection logic from event callbacks.
- **Zero Bloat**: No external JSON dependencies in this base template.

## Prerequisites
```bash
sudo apt-get install build-essential libwebsockets-dev libssl-dev
```

## Build & Run
```bash
make clean && make run
```

## Integration Notes
- **Binance/Finance**: Replace `i.address` and `i.path` with target exchange endpoints.
- **JSON Parsing**: Integrate `yyjson` in `LWS_CALLBACK_CLIENT_RECEIVE` for market data.
- **Static Linking**: Uncomment static flags in `Makefile` for portable binaries.
```
