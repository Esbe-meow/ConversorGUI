# ConversorGUI

A video and audio converter with a graphical interface, built in C++ with Qt6.
It uses ffmpeg under the hood to perform conversions and shows real-time progress.

## Features

- Simple and intuitive graphical interface.
- File selection via native dialog.
- Convert to any format supported by ffmpeg (mp4, avi, mkv, webm, mp3, etc.).
- Real-time progress bar.
- Non-blocking UI: you can keep using the window during conversion.

## Dependencies

- **Qt6** (`qt6-base`, `qt6-tools`)
- **ffmpeg** (includes `ffprobe`)
- **CMake** >= 3.19
- A C++ compiler (GCC or Clang)

On Arch Linux:

    sudo pacman -S qt6-base qt6-tools ffmpeg cmake gcc

## Building

    git clone https://github.com/Esbe-meow/ConversorGUI.git
    cd ConversorGUI
    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel

The executable will be located at `build/ConversorGUI`.

## Usage

1. Open the application.
2. Click **Select** to choose the input file.
3. Type the output extension (for example: `mp4`, `avi`, `mp3`).
4. Click **Convert**.
5. The converted file is saved in the same folder as the original.

## Project status

Personal project under development. Functional and stable.

## License

GPLv3. See [LICENSE](LICENSE).
