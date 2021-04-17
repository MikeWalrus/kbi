# How to compile?
## Linux
Make sure you have portaudio and gtkmm-4.0 installed and open this project in any IDE that supports CMake.

## Window
### 1. Install MSYS2: https://www.msys2.org/

### 2. Open msys2.exe
   
    Run theses commands to install some packages.
```
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-extra-cmake-modules
pacman -S mingw-w64-x86_64-make
pacman -S mingw-w64-x86_64-gdb
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-portaudio
pacman -S mingw-w64-x86_64-gtkmm4
```

### 3. Environment variable
Add `...\mingw64\bin` in your MSYS2 directory to the `path` environment variable.

### 4. Open CLion (I don't know the exact steps to configure other IDEs)

   Set the toolchain to mingw if it is not the default toolchain.

### 5. Build and run

   If it fails, comment out the line with something like `WIN32` in the header file where the error occurs.
   
# References
gtkmm图形界面的教程
https://developer.gnome.org/gtkmm-tutorial/unstable/gtkmm-tutorial.html#sec-gtkmm

产生声音和音乐的东西（有示例代码）
https://github.com/micknoise/Maximilian

电子合成器的介绍
https://m.youtube.com/playlist?list=PL6gRE6x7kQfpDQ3v0rnP872jdNGwpskez