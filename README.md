# WebGPU cross-platform app with CMake/Emscripten

Minimalist WebGpu native and web app skeleton with Imgui support.

## Live demo
https://pierricgimmig.github.io/web_gpu_app/

## Setup

```sh
# Clone repository and initialize submodules.
git clone https://github.com/pierricgimmig/web_gpu_app.git
cd web_gpu_app/
git submodule update --init --recursive --progress
```

## Requirements
- CMake: `pip install cmake`
- Emscripten: https://emscripten.org/docs/getting_started/downloads.html

## Specific platform build

```sh
# Build the app with CMake.
cmake -DCMAKE_BUILD_TYPE=Debug . -B build && cmake --build build -j4

# Run the app.
./build/app
```

## Web build

```sh
# Build the app with Emscripten.
emcmake cmake -DCMAKE_BUILD_TYPE=Debug -B build-web && cmake --build build-web -j4

# Run a server.
npx http-server
```

```sh
# Open the web app.
open http://127.0.0.1:8080/build-web/bin/triangle_app.html
```

## Ubuntu specific steps for web app
- Install unstable Google Chrome through sudo apt install google-chrome-unstable for WebGpu support
- Build this project with `emcmake cmake -B build-web && cmake --build build-web -j4`
- In a terminal, at the root of this project, run npx http-server (you might have to install npm)
- Launch WebGpu-enabled unstable Google Chrome: `google-chrome-unstable --enable-unsafe-webgpu --use-vulkan=true --test-type --enable-features=Vulkan`
- Go to http://127.0.0.1:8080/build-web/bin/triangle_app.html in the unstable browser

### Debugging WebAssembly

When building the app, compile it with DWARF debug information included thanks to `emcmake cmake -DCMAKE_BUILD_TYPE=Debug -B build-web`. And make sure to install the [C/C++ DevTools Support (DWARF) Chrome extension](https://goo.gle/wasm-debugging-extension) to enable WebAssembly debugging in DevTools.

