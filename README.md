# Image Compression using DCT

This project implements a simplified image compression algorithm based on the Discrete Cosine Transform (DCT) and quantization. The program reads a grayscale image, compresses it using a configurable compression factor, reconstructs the image, and reports the Compression Rate and PSNR.

A detailed description of the project, implementation, and experimental results can be found in the project report:

- [Project Report](report/Image_Compression_Using_the_Discrete_Cosine_Transform.pdf)

## Requirements

* C++17 or newer
* OpenCV

## Building

Clone the repository and compile the project:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

Run the executable from the command line:

```bash
./compress <input_image> <output_image> <compression_factor>
```

### Parameters

* `input_image`: Path to the input image.
* `output_image`: Path where the reconstructed image will be saved.
* `compression_factor`: Quantization scaling factor. Larger values produce stronger compression and lower image quality.


## Output

After execution, the program:

* Saves the reconstructed image to the specified output path.
* Displays the estimated Compression Rate.
* Displays the MSE and the Peak Signal-to-Noise Ratio (PSNR) in decibels (dB).
* Displays a visual comparison between the original image and the reconstructed verison.

## Notes

* The program processes images in grayscale.
* Images are internally divided into 8×8 blocks.
* The reported compression rate is an estimate based on the number of non-zero quantized DCT coefficients rather than the size of a JPEG file.
