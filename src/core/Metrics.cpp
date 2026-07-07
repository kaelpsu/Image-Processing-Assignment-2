#include "core/Metrics.hpp"
#include <cmath>

double Metrics::calculateMSE(const cv::Mat& original, const cv::Mat& reconstructed) {
    cv::Mat diff;
    cv::absdiff(original, reconstructed, diff);
    diff.convertTo(diff, CV_32F);
    diff = diff.mul(diff);
    cv::Scalar s = cv::sum(diff);
    return s[0] / (double)(original.total());
}

double Metrics::calculatePSNR(double mse, double max_val) {
    if (mse <= 1e-10) return INFINITY;
    return 20.0 * std::log10(max_val / std::sqrt(mse));
}

double Metrics::calculateCompressionRate(size_t uncompressedSize, size_t nonZeroCoefs) {
    if (nonZeroCoefs == 0) nonZeroCoefs = 1;
    return static_cast<double>(uncompressedSize) / static_cast<double>(nonZeroCoefs);
}