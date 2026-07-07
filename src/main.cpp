#include <iostream>
#include <string>
#include <cmath>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include "processing/SpatialTransformation.hpp"
#include "core/Metrics.hpp"

// exibe a imagem original e a imagem reconstruída lado a lado para comparação visual
void showComparison(const cv::Mat& original, const cv::Mat& reconstructed) {
    cv::Mat visual_comparison;
    
    // converte ambas para 8 bits para fazer a concatenação (comparação visual)
    cv::Mat img1, img2;
    original.convertTo(img1, CV_8U);
    reconstructed.convertTo(img2, CV_8U);

    // margem para imagem de comparação
    int marginWidth = 20;
    cv::Mat margin = cv::Mat::zeros(img1.rows, marginWidth, CV_8U);

    // concatena imagem original, margem e imagem reconstruída
    std::vector<cv::Mat> matrix_vec = {img1, margin, img2};
    cv::hconcat(matrix_vec, visual_comparison);

    // cria a janela e exibe
    std::string windowName = "Comparison: Original (left) | Compressed (right)";
    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, 1200, 600);
    cv::imshow(windowName, visual_comparison);
    
    std::cout << ">>> Press any key in the image window to close the program...\n";
    cv::waitKey(0); 
    cv::destroyAllWindows();
}

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Correct usage: " << argv[0] << " <input_path> <output_path> <compression_factor>\n";
        std::cerr << "Example: " << argv[0] << " input.jpg output.jpg 10.5\n";
        return -1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];
    float compressionFactor = std::stof(argv[3]);

    cv::Mat originalImage = cv::imread(inputPath, cv::IMREAD_GRAYSCALE);

    if (originalImage.empty()) {
        std::cerr << "Error: Could not load the image from " << inputPath << "\n";
        return -1;
    }

    // faz o padding com zeros se a imagem não for múltiplo de 8
    int origRows = originalImage.rows;
    int origCols = originalImage.cols;
    int paddedRows = ((origRows + 7) / 8) * 8;
    int paddedCols = ((origCols + 7) / 8) * 8;

    cv::Mat imgPadded;
    cv::copyMakeBorder(originalImage, imgPadded, 0, paddedRows - origRows, 0, paddedCols - origCols, cv::BORDER_CONSTANT, cv::Scalar(0));

    SpatialTransformation processor;

    cv::Mat imgReconstructedPadded = cv::Mat::zeros(paddedRows, paddedCols, CV_8U);
    size_t totalNonZeroCoefs = 0;

    // para cada bloco de 8x8 pixels
    for (int r = 0; r < paddedRows; r += 8) {
        for (int c = 0; c < paddedCols; c += 8) {

            Block8x8 pixelsBlock(8, std::vector<double>(8, 0.0));

            // para cada pixel no bloco, copia valor original como double (para usar com o DCT)
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    pixelsBlock[i][j] = static_cast<double>(imgPadded.at<uchar>(r + i, c + j));
                }
            }

            Block8x8 dctBlock = processor.applyDCT(pixelsBlock);
            Block8x8 quantizedBlock = processor.applyQuantization(dctBlock, compressionFactor);

            // conta o número de coeficientes não nulos para calcular a taxa de compressão
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    if (quantizedBlock[i][j] != 0.0) totalNonZeroCoefs++;
                }
            }

            Block8x8 dequantizedBlock = processor.applyInverseQuantization(quantizedBlock);
            Block8x8 reconstructedBlock = processor.applyInverseDCT(dequantizedBlock);

            // copia o bloco reconstruído para bloco da imagem final, convertendo de double para uchar (0-255)
            for (int i = 0; i < 8; i++) {
                for (int j = 0; j < 8; j++) {
                    imgReconstructedPadded.at<uchar>(r + i, c + j) = static_cast<uchar>(std::round(reconstructedBlock[i][j]));
                }
            }
        }
    }

    cv::Mat imgFinalResult = imgReconstructedPadded(cv::Rect(0, 0, origCols, origRows));
    cv::imwrite(outputPath, imgFinalResult);

    double mse = Metrics::calculateMSE(originalImage, imgFinalResult);
    double psnr = Metrics::calculatePSNR(mse, 255.0);

    std::cout << "\n================ RESULTADOS ================\n";
    std::cout << "Processed image: " << inputPath << "\n";
    std::cout << "File saved to: " << outputPath << "\n";
    std::cout << "Applied compression factor: " << compressionFactor << "\n";
    std::cout << "Mean Squared Error (MSE): " << mse << "\n";
    std::cout << "PSNR: " << psnr << " dB\n";
    std::cout << "Compression Ratio: " << Metrics::calculateCompressionRate(origRows * origCols, totalNonZeroCoefs) << "x\n";
    std::cout << "==============================================\n";

    originalImage = cv::imread(inputPath, cv::IMREAD_GRAYSCALE);
    cv::Mat compressedImage = cv::imread(outputPath, cv::IMREAD_GRAYSCALE);

    if (!originalImage.empty() && !compressedImage.empty()) {
        showComparison(originalImage, compressedImage);
    } else {
        std::cerr << "Warning: Could not load the images for visualization.\n";
    }

    return 0;
}