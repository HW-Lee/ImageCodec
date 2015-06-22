# ImageCodec


## Quick Start

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
	```

2. Specific lower bound of PSNR

	```c++
	YUVImage* img = YUVImage::import({image_path})
							->withFormat({image_format})
							->withSize({image_width}, {image_height});
	PerformancePackage::load(img->getName());
	ImageCodec::encodeTo({target_path})->withImage()->withMinPSNR({min_PSNR})->run();
	```

	e.g.

	```c++
	YUVImage* img = YUVImage::import("./1_1536x1024.yuv")
							->withFormat(YUVImage::FORMAT_4_2_0)
							->withSize(1536, 1024);
	PerformancePackage::load(img->getName());
	ImageCodec::encodeTo("./bitstream.bin")->withImage(img)->withMinPSNR(28)->run();
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