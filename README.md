# SVG Native Viewer

[![CircleCI](https://circleci.com/gh/adobe/svg-native-viewer.svg?style=svg)](https://circleci.com/gh/adobe/svg-native-viewer)

SVG Native viewer is a library that parses and renders SVG Native
documents.

## SVG Native
SVG Native is an upcoming specification of the SVG WG based on [SVG
OpenType](https://docs.microsoft.com/en-us/typography/opentype/spec/svg)
. SVG Native will be a strict subset of SVG 1.1 and SVG 2.0.

### Supported features in SVG Native (in comparison to SVG1.1/SVG2)
* No stylesheet support (CSS/XSL) with the exception of the basic
inheritance model and the following presentation attributes:
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
* CSS properties do not support any default property values like
  `inherit`, `initial`, `unset`, or `revert`.
* No support for scripting, interactions, events, animations, filters,
  masks, patterns, texts.
* No support for inner `<svg>` or `<symbol>` elements.
* No support for XML namespaces with the exception of the SVG namespace
  and the Xlink namespace.
* No support of `objectBoundingBox` on `gradientUnits` or
  `clipPathUnits`.
* The `var()` CSS value function is limited to the CSS properties
  `fill`, `stroke`, `stop-color` and `color`. Only color values are
  allowed. `currentColor` is supported.

A valid SVG Native document is always a valid SVG1.1/2.0 document.

## SVG Native Viewer Library

SVG Native Viewer is a C++11 based project and can either be included
in the source code of a client directly or linked statically or
dynamically.

For rendering, SVG Native Viewer requires a rendering port. Already
existing ports include:
* **StringSVGRenderer** for testing purposes,
* **CGSVGRenderer** a rendering port using CoreGraphics (Quartz 2D).
* **SkiaSVGRenderer** a rendering port using Skia. (**Skia requires a C++14 compatible compiler!**)
* **CairoSVGRenderer** a rendering port using Cairo Graphics.
* **GDIPlusSVGRenderer** a rendering port using GDI+.
* **D2DSVGRenderer** a rendering port using Direct2D.

New ports need to inherit from **SVGRenderer** and implement the
virtual functions.

Here an example how to use SVG Native Viewer with Skia
**SkiaSVGRenderer**:

```cpp
// Create the renderer object
auto renderer = std::make_shared<SVGNative::SkiaSVGRenderer>();

// Create SVGDocument object and parse the passed SVG string.
auto doc = std::unique_ptr<SVGNative::SVGDocument>
(SVGNative::SVGDocument::CreateSVGDocument(svgInput.c_str(), renderer));

// Setup SkSurface for drawing
auto skRasterSurface = SkSurface::MakeRasterN32Premul(doc->Width(),
                                                      doc->Height());
auto skRasterCanvas = skRasterSurface->getCanvas();

// Pass SkCanvas to renderer object
renderer->SetSkCanvas(skRasterCanvas);

// Pass drawing commands for SVG document to renderer.
doc->Render();

// Pass drawing commands for SVG document to renderer the element (and
// its descendants)
// with the XML ID "ref1".
std::string id1{"ref1"}
doc->Render(id1);

// The Render() function may get called multiple times. This can be
// used to render a combination of glyphs specified in the same SVG
// document.
std::string id2{"ref2"}
doc->Render(id2);
```

Refer to the examples in the `example/` directory for other port
examples.

## Requirements
### Submodules

This repository uses submodules. To initiate and keep submodules
up-to-date run the following command:

```
git submodule update --init
```

Submodules are located in the `third_party/` directory. Used submodules:
* [stylesheet](https://github.com/adobe/stylesheet/tree/modifications)
  **(optional)** Needed if compiled with limited CSS style support
(deprecated).
* [cpp-base64](https://github.com/ReneNyffenegger/cpp-base64)
  **(optional)** Needed by some ports for decoding base64 encoded raster
image support.
* [boost_variant_property_tree](https://github.com/dirkschulze/boost_variant_property_tree)
  **(optional)** Minimal version of Boost stripped down to the
  requirements of `variant` and `property_tree`. Used if Boost was not
  installed on the system.


### Windows

Install:
* [CMake](https://cmake.org) Download and run the installer
* [Boost](https://www.boost.org) Download the ZIP-package and extract
  the package into `C:>\Program Files\boost_x.y.z\` (See below how to
  specify a different Boost installation directory by a CMake option.)
* [MS Visual Studio 2017 or
  up](https://visualstudio.microsoft.com/vs/whatsnew/) Download and
  install with the installer. Make sure Visual C++ gets installed. (You
  maybe be able to use the "Community" version for free for
  non-commercial/enterprise use. See the website from MS for license
  details.)

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
sudo apt-get install build-essential libboost-system-dev cmake
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
_Omit `-H` when running in PowerShell._

For macOS
```
cmake -Bbuild/mac -H. -G "Xcode"
```

For Linux
```
cmake -Bbuild/linux -H.
```
On Linux you may choose to use GCC or Clang/LLVM. Add the following to
the command above to choose between one and the other:
* `-DCMAKE_CXX_COMPILER=g++` for GCC
* `-DCMAKE_CXX_COMPILER=clang` for Clang/LLVM

On Windows you may choose to use Clang/LLVM as compiler. For help,
follow the instructions of the linked MS Visual Studio [LLVM
Extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.llvm-toolchain). 
Add ` -T "LLVM"` to the project generation command above.

To specify a different Boost installation directory on Windows use the
following command:
* `-DBOOST_ROOT=X:\path\to\boost`

The following arguments can be passed with the `-D` flag and the
options `ON` or `OFF`:
* `LIB_ONLY` Only compile the library without examples. Default `OFF`.
* `SHARED` If `ON`, builds a dynamic library. Static otherwise. Default
  `OFF`.
* `PLATFORM_XML` If `ON`, uses libxml2 or Epat if provided by the
  system. Otherwise RapidXML via boost. Default `OFF`.
* `TEXT` adds the _Text_ port to the library. Default `ON`.
* `CG` adds the _CoreGraphics/Quartz2D_ port to the library. Default
  `OFF`.
* `SKIA` adds the _Skia_ port to the library. Default `OFF`.
* `GDIPLUS` adds the _GDI+_ port to the library. Default `OFF`.
* `D2D` adds the _Direct2D_ port to the library. Default `OFF`.
* `CAIRO` adds the _Cairo Graphics_ port to the library. Default `OFF`.

To enable the deprecated CSS styling support:
* `STYLE` adds limited, deprecated support for `<style>` element and
  `style` attribute. Default `OFF`. _(This option will get removed
  eventually.)_

The following example creates project files for the library with the
Text, CoreGraphics/Quartz2D and Skia port and the example applications.
**Example:**
```
cmake -Bbuild/mac -H. -G "Xcode" -DCG=ON -DSKIA=ON
```

**Note:** For testing, build with the `TEXT` option set to `ON` and
 `LIB_ONLY` option set to `OFF`. (The default for both.)

### Build

Replace `win64` with your platform (`mac` for Xcode on macOS)
```
cmake --build build/win64 --config Release
```

## Boost requirements

Only the header version of Boost is required. The following Boost
features are used:
* Boost RapidXML _(Can be replaced by libxml2.)_
* `boost::variant` to handle different SVG paint types of `fill` and
 `stroke` as well as different color value types.
* Boost string functions like `boost::tokenizer`, `boost::trim`. _(Only
used by deprecated CSS `<style>` element parsing.)_

## Tests
SVG Native Viewer has two testing mechanisms. A python script that performs the renderings on the *Text* port and compares the renderings with the existing ones and automated software testing using Google Tests framework. 

### *Text* port testing
To use the python script:
1. Make sure your system has Python installed.
2. By default, CMake creates the project files for
**SVGNativeViewerLib** and **testSVGNative**. Follow the steps above to
build the test app.
3. Run
  ```
  python script/runTest.py --tests=test/
  ```
  Here the argument list of **runTest.py**:
  * `--test` the folder with the test files.
  * `--program` the path to **testSVGNative**. If not provided uses the
    default, relative build path.
  * `--debug` Debug build or Release build of **testSVGNative**. Only
    relevant if `--program` was not set and defaults to `--debug`.
### Google Test based testing
In order to build and run the tests, pass the argument `-DTESTING=ON` when running Cmake. Cmake will automatically download and build Google Tests and compile the tests. In order to run the tests you can run `make test` or just use `ctest` in the build folder.

Ultimately, we aim to improve software unit testing as well as add rendering tests to ensure that SVG Native Viewer's renderings are accurate.

## Known limitations in SVG Native Viewer
* `preserveAspectRatio` is not supported on the `<svg>` element yet.
* Furthermore, there might be limitations on certain platforms. (E.g.
  missing gradient spread-method support on CoreGraphics.)

## Contributing

Contributions are welcomed! Read the [Contributing
Guide](./.github/CONTRIBUTING.md) for more information.

## Licensing

This project is licensed under the Apache V2 License. See
[LICENSE](LICENSE) for more information.
