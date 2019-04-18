# SVG Native Viewer

[![CircleCI](https://circleci.com/gh/adobe/svg-native-viewer.svg?style=svg)](https://circleci.com/gh/adobe/svg-native-viewer)

SVG Native viewer is a library that parses and renders SVG Native documents.

## SVG Native
SVG Native is an upcoming specification of the SVG WG based on [SVG OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec/svg). SVG Native will be a strict subset of SVG 1.1 and SVG 2.0.

### A collection of supported features in SVG Native (in comparison to SVG1.1/SVG2)
* No stylesheet support (CSS/XSL) with the exception of the `style` attribute (only CSS property/value pairs), its basic inheritance model and the following presentation attributes:
    * `clip-path`
    * `clip-rule`
    * `color`
    * `display`
    * `fill`
    * `fill-opacity`
    * `fill-rule`
    * `opacity`
    * `stroke`
    * `stroke-dasharray`
    * `stroke-dashoffset`
    * `stroke-linecap`
    * `stroke-linejoin`
    * `stroke-miterlimit`
    * `stroke-opacity`
    * `stroke-width`
    * `stop-color`
    * `stop-opacity`
    * `visibility`
* CSS properties do not support any default property values like `inherit`, `initial`, `unset`, or `revert`.
* No support for scripting, interactions, events, animations, filters, masks, patterns, texts.
* No support for inner `<svg>` or `<symbol>` elements.
* No support for XML namespaces with the exception of the SVG namespace and the Xlink namespace.
* No support of `objectBoundingBox` on `gradientUnits` or `clipPathUnits`.
* The `var()` CSS value function is limited to the CSS properties `fill`, `stroke`, `stop-color` and `color`. Only color values are allowed. `currentColor` is supported.

A valid SVG Native document is always a valid SVG1.1/2.0 document.

### Known limitations in SVG Native Viewer
* Referenced elements need to be declared first. Example: A `<linearGradient>` element must be defined in the SVG document before its first reference (`fill="url(#gradient)"`).
* `viewBox` on `<svg>` element does not take translation values into account yet.
* `preserveAspectRatio` is not supported on the `<svg>` element yet.
* Furthermore, there might be limitations on certain platforms. (E.g. missing spread-method support on CoreGraphics.)

## SVG Native Viewer Library

SVG Native Viewer is a C++14 based project and can either be included in the source code of a client directly or linked statically or dynamically.

For rendering, SVG Native Viewer requires a rendering port. Already existing ports include:
* **StringSVGRenderer** for testing purposes,
* **CGSVGRenderer** a rendering port using CoreGraphics (Quartz 2D).
* **SkiaSVGRenderer** a rendering port using Skia.

New ports need to inherit from **SVGRenderer** and implement the virtual functions.

Here an example how to use SVG Native Viewer with the Skia **SkiaSVGRenderer**:

```cpp
// Create the renderer object
auto renderer = std::make_shared<SVGNative::SkiaSVGRenderer>();

// Create SVGDocument object and parse the passed SVG string.
auto doc = std::unique_ptr<SVGNative::SVGDocument>(SVGNative::SVGDocument::CreateSVGDocument(svgInput.c_str(), renderer));

// Setup SkSurface for drawing
auto skRasterSurface = SkSurface::MakeRasterN32Premul(doc->getWidth(), doc->getHeight());
auto skRasterCanvas = skRasterSurface->getCanvas();

// Pass SkCanvas to renderer object
renderer->SetSkCanvas(skRasterCanvas);

// Pass drawing commands for SVG document to renderer.
doc->Render();
```

Refer to the examples in the `example/` directory for other port examples.

## Requirements
### Submodules

This repository uses submodules. To initiate and keep submodules up-to-date run the following command:

```
git submodule update --init
```

Submodules are located in the `third_party/` directory. Used submodules:
* [stylesheet](https://github.com/adobe/stylesheet/tree/modifications) **(required)**
* [cpp-base64](https://github.com/ReneNyffenegger/cpp-base64) **(optional)** Needed by some ports for decoding base64 encoded raster image support.

### Windows

Install:
* [CMake](https://cmake.org) Download and run the installer
* [Boost](https://www.boost.org) Download the ZIP-package and extract the package into `C:>\Platform Files\boost\`
* [MS Visual Studio 2017 or up](https://visualstudio.microsoft.com/vs/whatsnew/) Download and install with the installer. Make sure Visual C++ gets installed. (You maybe be able to use the "Community" version for free for non-comercial/enterprise use. See the website from MS for license details.)
### OSX
With Homebrew:
```
brew install cmake
brew install llvm
brew install boost
```

### LINUX

* Apt
```
sudo apt-get install build-essential libboost-system-dev
```

## Building

### Create project files

For Windows 64 bit:
```
cmake -Bbuild/win64 -H. -G "Visual Studio 15 2017 Win64"
```

For Windows 32 bit:
```
cmake -Bbuild/win32 -H. -G "Visual Studio 15 2017"
```

For macOS
```
cmake -Bbuild/mac -H. -G "Xcode"
```

For Linux
```
cmake -BBuild/linux -H.
```
On Linux you may choose to use GCC or Clang/LLVM. Add the following to the command above to choose between one and the other:
* `-DCMAKE_CXX_COMPILER=g++` for GCC
* `-DCMAKE_CXX_COMPILER=clang` for Clang/LLVM

The following arguments can be passed with the `-D` flag and the options `ON` or `OFF`:
* `TEXT` adds the _Text_ port to the library. Default `ON`.
* `CG` adds the _CoreGraphics/Quartz2D_ port to the library. Default `OFF`.
* `Skia` adds the _Skia_ port to the library. Default `OFF`.

Each port includes an example project using the port. To disable the example projects set the following option to `OFF`. Default `ON`.
* `LIBRARY_ONLY`

The following example creates project files for the library with the Text, CoreGraphics/Quartz2D and Skia port and the example applications.
**Example:**
```
cmake -Bbuild/mac -H. -G "Xcode" -DCG=ON -DSKIA=ON
```

**Note:** For testing, build with the `TEXT` option set to `ON` and `LIBRARY_ONLY` option set to `OFF`. (The default for both.)

### Build

Replace `win64` with your platform (`mac` for Xcode on macOS)
```
cmake --build build/win64 --config Release
```

## C++14/Boost requirements

The following C++14 features get used and would need to get replaced to compile SVG Native Viewer in a C++11 environment.
* Advanced features of initialization lists
* Changes to default CTOR/DTOR
* `std::tuple` to hold 3 different color-stop values.

Only the header version of Boost is required. The following Boost features are used:
* Boost RapidXML (could be replace by RapidXML standalone)
* `boost::variant` to handle different SVG paint types of `fill` and `stroke` as well as different color value types.
* Boost string functions like `boost::tokenizer`, `boost::trim`. _(Only used by deprecated CSS `<style>` element parsing.)_

## Tests

1. Make sure your system has Python installed.
2. By default, CMake creates the project files for **SVGNativeViewerLib** and **testSVGNative**. Follow the steps above to build the test app.
3. Run
  ```
  python script/runTest.py --tests=test/
  ```
  Here the argument list of **runTest.py**:
  * `--test` the folder with the test files.
  * `--program` the path to **testSVGNative**. If not provided uses the default, relative build path.
  * `--debug` Debug build or Release build of **testSVGNative**. Only relevant if `--program` was not set and defaults to `--debug`.

## Contributing

Contributions are welcomed! Read the [Contributing Guide](./.github/CONTRIBUTING.md) for more information.

## Licensing

This project is licensed under the Apache V2 License. See [LICENSE](LICENSE) for more information.
