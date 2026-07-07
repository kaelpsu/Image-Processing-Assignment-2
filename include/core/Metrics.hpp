#pragma once
#include <opencv2/opencv.hpp>

namespace Metrics {
    double calculateMSE(const cv::Mat& original, const cv::Mat& reconstructed);
    double calculatePSNR(double mse, double max_val = 255.0);
    double calculateCompressionRate(size_t uncompressedSize, size_t nonZeroCoefs);
}