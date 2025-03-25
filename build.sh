#!/usr/bin/env bash

echo "[🔨 BUILDING]"

# Set compiler FIRST, before any cache or cmake commands
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++

# 💣 Remove build folder if it exists
if [ -d build ]; then
  echo "[⚠️ Removing existing build dir]"
  rm -rf build
fi

# 🧱 Make new clean build directory
mkdir build
cd build

# 🛠️ Generate Makefiles with CMake
cmake -G "Unix Makefiles" ..

# ⚒️ Compile using all cores
make -j"$(nproc)"

cd ..

# ✅ Check output
if [ -f build/deepface_filter.so ]; then
    echo "[✅ BUILT] build/deepface_filter.so"
else
    echo "[❌ ERROR] Build failed: .so not found"
fi
