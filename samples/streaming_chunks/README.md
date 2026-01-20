# Streaming chunks sample

This sample demonstrates streaming encode and decode with chunk callbacks for
both `tstr` and `bstr` fields using zcbor-generated code.

## To build

```sh
cmake -S . -B build -DREGENERATE_ZCBOR=Y
cmake --build build
```

## To run

```sh
build/app
```

## Expected output

```
Decoded name: Chunky Name
Decoded payload: de ad be ef ca fe ba be
```
