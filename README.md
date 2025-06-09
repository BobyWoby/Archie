# Archie
A voice assistant that I wrote for my Arch setup in C++ using SDL3 and whisper.cpp

## Installation
First, clone the repo and cd into it:
```bash
git clone https://github.com/BobyWoby/Archie.git
cd Archie
```

Next, install the dependencies (SDL3 and whisper.cpp):
```bash
git clone https://github.com/ggml-org/whisper.cpp.git ./vendored/whisper
git clone https://github.com/ggml-org/whisper.cpp.git ./vendored/SDL
```

Install the default whisper model to be run:
```bash
sh ./vendored/whisper/models/download-ggml-model.sh base.en
```

Build the project with CMake:
```bash
mkdir build && cd build # create a build directory and cd into it
cmake ../
make
```
Run the executable:
```bash
./Release/archie
```
