# FT2Test
FreeType2 Test Env on Msys2

1. Install Msys2 and Startup MSYS2 MinGW 32-bit
Please confirm your MSYS2 install path, Eg: Install to D:/program/msys64，the path of Msys2 is:
```
/d/program/msys64
```
2. Install dependencies
```
pacman -Syu
pacman -S git gcc gdb ninja make cmake mingw-w64-i686-SDL2
```
3. Download
```
git clone https://github.com/lgl88911/FT2Test.git

```
Since the Msys2 you installed may be in a different path, please modify the cd ./FT2Test/CMakeLists.txt below to ensure that SDL2 can be used correctly.
```
include_directories(/d/program/msys64/mingw32/include/)
link_directories(/d/program/msys64/mingw32/bin)
```

4. build
```
cd ./FT2Test
mkdir ftbuild
cd ./ftbuild
cmake -D CMAKE_INSTALL_PREFIX=$PWD/../INSTALL \
    -D CMAKE_DISABLE_FIND_PACKAGE_ZLIB=TRUE \
    -D CMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE \
    -D CMAKE_DISABLE_FIND_PACKAGE_PNG=TRUE \
    -D CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE \
    -D CMAKE_DISABLE_FIND_PACKAGE_BrotliDec=TRUE \
    ../freetype-2.11.0/
ninja
ninja install
cd ../
mkdir build
cd ./build
cmake ../
ninja
```

5. Test
The executable file main.exe will be generated under build, you can execute ./main.exe


Add the code to test Freetype in main.c according to your needs and test it.
