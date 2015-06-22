# ImageCodec

## Requirements
- Mac OSX [**recommended**] or UNIX (terminal and shell script used)
- Xcode-build-tools [**recommended**] or g++ compiler
- Matlab [**recommended**] or YUV viewer


	Note that Windows has not been tested yet, so it might take lots of time encoding an image. Please wait for it, thanks.

## Project Structure
- Root: `/ImageCodec/`
- Code: `/ImageCodec/code/`, `/ImageCodec/libs/`
- Data: `/ImageCodec/data/`
- Generated: `/ImageCodec/results/`
- Executable (Mac OSX supported only): `/ImageCodec/executable/`
- Report: `/ImageCodec/report/`

	Note that if user does not arrange files followed the structure mentioned above, it might cause some unknown errors.

## Quick Start

### Directly running the sample code

- run `cd ~/{YOUR_DIR}/ImageCodec`

- Configuration in the sample code
	1. check the file `/ImageCodec/code/ImageCodec_Demo.cpp`

		```c++
		#define CONSTRAINT_BY_PSNR 0
		#define CONSTRAINT_BY_BITRATE 1
		#define SKIP_ENCODING -1

		using namespace std;
		int main(int argc, const char * argv[]) {

			int constWay = {Config}; // 
								<-- can be
								CONSTRAINT_BY_PSNR    : Use PSNR as constraint
								CONSTRAINT_BY_BITRATE : Use bitrate as constraint
								SKIP_ENCODING         : Skip encoding

			int opt = {Config}; // <-- can be
											0: 1_1536x1024.yuv
											1: 2_1024x768.yuv
											2: 3_1000x728.yuv
											3: 4_1000x1504.yuv
		```
		
		and
		
		```c++
			if (constWay == CONSTRAINT_BY_PSNR) {
				ss << PATHS[opt+4]; ss << "bitstream1.bin";
				ImageCodec::encodeTo(ss.str())->withImage(img)->withMinPSNR({Config})->run();
				ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream2.bin";
				ImageCodec::encodeTo(ss.str())->withImage(img)->withMinPSNR({Config})->run();
				ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream3.bin";
				ImageCodec::encodeTo(ss.str())->withImage(img)->withMinPSNR({Config})->run();
			} else if (constWay == CONSTRAINT_BY_BITRATE) {
				ss << PATHS[opt+4]; ss << "bitstream1.bin";
				ImageCodec::encodeTo(ss.str())->withImage(img)->withMaxBitrate({Config})->run();
				ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream2.bin";
				ImageCodec::encodeTo(ss.str())->withImage(img)->withMaxBitrate({Config})->run();
				ss.str(""); ss.clear(); ss << PATHS[opt+4]; ss << "bitstream3.bin";
				ImageCodec::encodeTo(ss.str())->withImage(img)->withMaxBitrate({Config})->run();
			}
		```
			
	2. Replace the annotation `{Config}` with your specific configurations.

	3. run `./run.sh` under `/ImageCodec/`

- Do the following commands on the terminal
	1. run `chmod +x ./run.sh`
	2. key in your pwd
	3. run `./run.sh`

- Check the output files in `/ImageCodec/results/`

### Encoding an image

- This API gives 2 ways to encode an image.

1. Specific upper bound of bitrate

	```c++
	YUVImage* img = YUVImage::import({image_path})
							->withFormat({image_format})
							->withSize({image_width}, {image_height});
	PerformancePackage::load(img->getName());
	ImageCodec::encodeTo({target_path})->withImage()->withMaxBitrate({max_bitrate})->run();
	```

	e.g.

	```c++
	YUVImage* img = YUVImage::import("./1_1536x1024.yuv")
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(1536, 1024);
	PerformancePackage::load(img->getName());
	ImageCodec::encodeTo("./bitstream.bin")->withImage(img)->withMaxBitrate(0.75)->run();
	PerformancePackage::save();
	```

2. Specific lower bound of PSNR

	```c++
	YUVImage* img = YUVImage::import({image_path})
							->withFormat({image_format})
							->withSize({image_width}, {image_height});
	PerformancePackage::load(img->getName());
	ImageCodec::encodeTo({target_path})->withImage()->withMinPSNR({min_PSNR})->run();
	PerformancePackage::save();
	```

	e.g.

	```c++
	YUVImage* img = YUVImage::import("./1_1536x1024.yuv")
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(1536, 1024);
	PerformancePackage::load(img->getName());
	ImageCodec::encodeTo("./bitstream.bin")->withImage(img)->withMinPSNR(28)->run();
	PerformancePackage::save();
	```
	
### Decoding an image

- Decoding a compressed file is easy.

	```c++
	ImageCodec::decode({bitstream_path})->saveTo({target_path})->run();
	```
	e.g.
	
	```c++
	ImageCodec::decode("./bitstream.bin")->saveTo("./decompressed.yuv")->run();
	```

### Obtaining PSNR

- Importing 2 images and then call the following API.

	```c++
	double PSNR = img1->calPSNR(img2);
	```
	
	e.g.
	
	```c++
	YUVImage* img1 = YUVImage::import("./1_1536x1024.yuv")
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(1536, 1024);
	YUVImage* img2 = YUVImage::import("./decompressed.yuv")
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(1536, 1024);
	double PSNR = img1->calPSNR(img2);
	```

### Obtaining Bitrate

- Importing the original image, and then give the path of compressed file.

	```c++
	double bitrate = img->calBitrate({bitstream_path});
	```	
	
	e.g.
	
	```c++
	YUVImage* img = YUVImage::import("./1_1536x1024.yuv")
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(1536, 1024);
	double bitrate = img->calBitrate("./bitstream.bin");
	```


## APIs List (only description so far)

### ImageCodec.h

### YUVImage.h

<!--
| type | return | name | parameters | description |
|:-----|:--------|:--------|:------|:-------|
| static | YUVImage* | emptyImage | string path, int width, <br/> int height, int format |
| dynamic | YUVImage* | transformToFormat | YUVImage::FORMAT\_4\_2\_0 \| <br/> YUVImage::FORMAT\_4\_2\_2 \| <br/> YUVImage::FORMAT\_4\_4\_4 |
| dynamic | YUVImage* | add | YUVImage* image | circular addition |
| dynamic | YUVImage* | diff | YUVImage* image | circular subtraction |
| dynamic | int | getWidth | DataLayer::Y \| <br/> DataLayer::Cb \| <br/> DataLayer::Cr | width of Y/U/V layer |
| dynamic | int | getHeight | DataLayer::Y \| <br/> DataLayer::Cb \| <br/> DataLayer::Cr | height of Y/U/V layer |
| dynamic | int | getDataSize | DataLayer::Y \| <br/> DataLayer::Cb \| <br/> DataLayer::Cr | area of Y/U/V layer |
| dynamic | T | getYDataAt\<T\> | int x, int y | the value at the position (x, y) in Y layer |
| dynamic | T | getCbDataAt\<T\> | int x, int y | the value at the position (x, y) in U layer |
| dynamic | T | getCrDataAt\<T\> | int x, int y | the value at the position (x, y) in V layer |
-->

### YUVImageFactory.h

### ImagePredictor.h

### Transform.h

### KmeansFactory.h

### HuffmanTable.h

### GolombRiceTable.h

### PerformancePackage.h

### BitReader.h

### BitWriter.h

### Symbol.h