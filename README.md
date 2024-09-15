
## pngquant-android

[pngquant](https://pngquant.org/) (pngquant_2.7.0 + libimagequant_2.8.2) with basic Java bindings for Android.


In your Android app:
**Optimizing the image with default settings:** 
```java
File inputPngFile = getYourPng();
File outputPngFile = getOutputFile();
LibPngQuant.pngQuantFile(inputFile, outputFile);
```
**Alternative commands with more options:**
```java
LibPngQuant.pngQuantFile(inputFile, outputFile, minQuality, maxQuality);
```
```java
LibPngQuant.pngQuantFile(inputFile, outputFile, minQuality, maxQuality, speed);
```
```java
LibPngQuant.pngQuantFile(inputFile, outputFile, minQuality, maxQuality, speed, floydDitherAmount);
```

### Building
- This project includes git submodules; please make sure to `git clone --recursive`. Alternatively, `git submodule init; git submodule update`.
- On OS X, please install wget: `brew install wget`.
- From the top-level directory, run `./gradlew installDebug`. This will download all dependencies, build the library, and install a test application to a connected Android device.
