#include "processing/SpatialTransformation.hpp"
#include <cmath>

SpatialTransformation::SpatialTransformation() {
    // matriz de quantização do formato JPEG para luminância
    quantizationMatrix = {
        {16, 11, 10, 16, 24, 40, 51, 61},
        {12, 12, 14, 19, 26, 58, 60, 55},
        {14, 13, 16, 24, 40, 57, 69, 56},
        {14, 17, 22, 29, 51, 87, 80, 62},
        {18, 22, 37, 56, 68, 109, 103, 77},
        {24, 35, 55, 64, 81, 104, 113, 92},
        {49, 64, 78, 87, 103, 121, 120, 101},
        {72, 92, 95, 98, 112, 100, 103, 99}
    };
}

double SpatialTransformation::calculateC(int val) {
    if (val == 0) {
        return 1.0 / std::sqrt(2.0);
    }
    return 1.0;
}

Block8x8 SpatialTransformation::applyDCT(const Block8x8& pixelsBlock) {

    Block8x8 dctBlock(8, std::vector<double>(8, 0.0));

    // u e v representam as coordenadas no domínio da frequência
    for (int u = 0; u < 8; u++) {
        for (int v = 0; v < 8; v++) {
            
            double sum = 0.0;
            
            // x e y representam as coordenadas espaciais (os pixels originais)
            for (int x = 0; x < 8; x++) {
                for (int y = 0; y < 8; y++) {

                    double cosine1 = std::cos(((2.0 * x + 1.0) * u * M_PI) / 16.0);
                    double cosine2 = std::cos(((2.0 * y + 1.0) * v * M_PI) / 16.0);
                    
                    // para equilibrar a onda em torno do zero
                    double centerPixel = pixelsBlock[x][y] - 128.0;
                    
                    sum += centerPixel * cosine1 * cosine2;
                }
            }
            
            double coeficientC = calculateC(u) * calculateC(v);
            dctBlock[u][v] = 0.25 * coeficientC * sum;
        }
    }

    return dctBlock;
}

Block8x8 SpatialTransformation::applyQuantization(const Block8x8& dctBlock, float compressionFactor ) {
    Block8x8 quantizedBlock(8, std::vector<double>(8, 0.0));

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            double adjustedQuantizationValue = quantizationMatrix[i][j] * compressionFactor;
            
            if (adjustedQuantizationValue < 1.0) {
                adjustedQuantizationValue = 1.0; // Evita divisão por zero ou valores muito pequenos
            }
            quantizedBlock[i][j] = std::round(dctBlock[i][j] / adjustedQuantizationValue);
        }
    }

    return quantizedBlock;
}

Block8x8 SpatialTransformation::applyInverseQuantization(const Block8x8& quantizedBlock) {
    Block8x8 dequantizedBlock(8, std::vector<double>(8, 0.0));

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            dequantizedBlock[i][j] = quantizedBlock[i][j] * quantizationMatrix[i][j];
        }
    }

    return dequantizedBlock;
}

Block8x8 SpatialTransformation::applyInverseDCT(const Block8x8& dctBlock) {
    Block8x8 pixelBlock(8, std::vector<double>(8, 0.0));

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            double sum = 0.0;

            for (int u = 0; u < 8; u++) {
                for (int v = 0; v < 8; v++) {
                    double cosine1 = std::cos(((2.0 * x + 1.0) * u * M_PI) / 16.0);
                    double cosine2 = std::cos(((2.0 * y + 1.0) * v * M_PI) / 16.0);

                    double coeficientC = calculateC(u) * calculateC(v);

                    sum += coeficientC * dctBlock[u][v] * cosine1 * cosine2;
                }
            }

            double spatialValue = 0.25 * sum;

            // soma 128 para trazer o pixel de volta ao intervalo original
            spatialValue += 128.0;

            if (spatialValue > 255.0) spatialValue = 255.0;
            if (spatialValue < 0.0) spatialValue = 0.0;

            pixelBlock[x][y] = std::round(spatialValue);
        }
    }

    return pixelBlock;
}