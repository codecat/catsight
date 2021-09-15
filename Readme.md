# Catsight
Cross-platform process memory viewer inspired by x64dbg.

## Features
* Cross-platform (currently runs on Linux and Windows).
* Attach to any process that your system user has access to.
* Look at all the mapped pages of memory.
* Browse and scroll through the memory of any process as an address table.
* Disassemble code in memory regions and jump between references.
* Byte pattern searching and generation in code sections.

## Download & install
Pre-built binaries will be provided very soon on Itch.io.

## Screenshots
### Data tab
![](https://missdev.nl/catsight4.png)

### Code tab
![](https://missdev.nl/catsight5.png)

### Maps tab
![](https://missdev.nl/catsight6.png)

### String search
![](https://missdev.nl/catsight7.png)

## What this isn't
This is not a debugger. (At least, not yet.) It was not designed with debugging in mind, it is merely a process memory viewer.

# Building
Catsight uses [CMake](https://cmake.org/) to build.

## Preparing the build
You'll need to make sure the Git submodules are initialized and updated first. For example:

```
$ git submodule update --init --recursive
```

## Building on Linux
Building on Linux should work out of the box as long as the necessary dependencies are installed.

Using your favorite package manager, you will have to install `lua` and its development files.

Then, to actually run the CMake build:
```
$ mkdir build
$ cd build
$ cmake ..
$ make -j9
```

You can also use `ninja` by running CMake like this:

```
$ cmake .. -G Ninja
$ ninja
```

## Building on Windows
On Windows, you can optionally use [vcpkg](https://github.com/Microsoft/vcpkg) to install the `lua` dependency using `vcpkg install lua:x64-windows`. You can then run CMake with `-DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake` before building and it will automatically find the dependencies.

To actually run the CMake build, you can use the CMake GUI application to generate a Visual Studio project, or you can use the command line:
```
> mkdir build
> cd build
> cmake ..
```

## Using VSCode
VSCode is a great way to automatically configure and build the CMake project on both Linux and Windows. On Linux no additional configuration is necessary.

On Windows, you might need to add the following directive to `.vscode/settings.json` to make sure that the vcpkg toolchain is being used:

```json
"cmake.configureSettings": {
	"CMAKE_TOOLCHAIN_FILE": "C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake",
},
```
