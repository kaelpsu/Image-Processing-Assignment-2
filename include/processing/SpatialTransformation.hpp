#pragma once

#include <vector>
#include <cmath>

typedef std::vector<std::vector<double>> Block8x8;

class SpatialTransformation {
private:
    Block8x8 quantizationMatrix;

    // função matemática auxiliar da equação da DCT
    double calculateC(int val);

public:
    SpatialTransformation();
    Block8x8 applyDCT(const Block8x8& pixelBlock);
    Block8x8 applyQuantization(const Block8x8& dctBlock, float compressionFactor);
    Block8x8 applyInverseQuantization(const Block8x8& quantizedBlock);
    Block8x8 applyInverseDCT(const Block8x8& dctBlock);
};