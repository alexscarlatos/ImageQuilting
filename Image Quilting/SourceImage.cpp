//
//  SourceImage.cpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 3/31/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#include "SourceImage.hpp"
#include <random>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <time.h>

// Create a source image object with a file source, block size, border size and randomness
SourceImage::SourceImage(const char *filename, GLsizei blockS, GLsizei borderS, GLint randomness) {
    // Create the underlying image
    image = new Image(filename);
    
    blockSize = blockS;
    borderSize = borderS;
    
    // must have at least one column
    // then add as many (blockSize - borderSize) pieces as will fit
    numCols = 1 + ((image->width - blockSize) / (blockSize - borderSize));
    
    // same idea for rows
    numRows = 1 + ((image->height - blockSize) / (blockSize - borderSize));
    
    std::cout << "Source Image: numCols:" << numCols << " numRows:" << numRows << "\n";
    
    blockChoosingRandomness = randomness;

    // Seed the randomness
    srand((unsigned)time(0));
}

SourceImage::~SourceImage() {
    delete image;
}

// position of block is [col * (blockSize - borderSize), row * (blockSize - borderSize)]
GLint SourceImage::posX(GLint col) {
    return col * (blockSize - borderSize);
}

GLint SourceImage::posY(GLint row) {
    return row * (blockSize - borderSize);
}

GLint SourceImage::getRandomBlock() {
    return rand()%((numCols-1) * (numRows-1));
}

// Find minimum error block bordering one or two source blocks
// Params: sourceBlockLeft - this is the block whose right border you are testing against
//         sourceBlockBottom - this is the block whose top border you are testing against
//         type - the type of matching (Right, Top, or Both)
// Returns: the index of the chosen block to place after the matching process
//          and fills borderPathLeft and borderPathBottom with best border paths
GLint SourceImage::findMinimumErrorBlock(int sourceBlockLeft, int sourceBlockBottom, BlockMatch type, int *borderPathLeft, int *borderPathBottom, Image *targetImage, int drawX, int drawY) {
    GLint borderArea = borderSize * blockSize * 3, blockArea = blockSize * blockSize * 3;
    GLint error = 0;
    GLint totalNumBlocks = numCols * numRows;
    GLubyte *sourceRightBorder = NULL, *sourceTopBorder = NULL, *targetBorder, *sourceBlock = NULL, *targetBlock = NULL;
    if (type == Right || type == Both)
        sourceRightBorder = new GLubyte[borderArea];
    if (type == Top || type == Both)
        sourceTopBorder = new GLubyte[borderArea];
    if (targetImage != NULL) {
        sourceBlock = new GLubyte[blockArea];
        targetBlock = new GLubyte[blockArea];
    }
    targetBorder = new GLubyte[borderArea];
    
    // Get pixels of sourceBlock's appropriate border and store in sourceBorder
    GLint col = sourceBlockLeft % numCols;
    GLint row = sourceBlockLeft / numCols;
    if (type == Right || type == Both) {
        // Right border of source block begins where block to right starts
        image->readPixels(posX(col + 1), posY(row), borderSize, blockSize, sourceRightBorder);
    }
    col = sourceBlockBottom % numCols;
    row = sourceBlockBottom / numCols;
    if (type == Top || type == Both) {
        // Top border of source block begins where block above starts
        image->readPixels(posX(col), posY(row+1), blockSize, borderSize, sourceTopBorder);
    }
    
    struct errorBlock {
        GLint index;
        GLint error;
    };
    
    std::vector<errorBlock> possibleBlocks;
    
    // Compare sourceBorder with the appropriate border of every block in the image
    for (int i = 0; i < totalNumBlocks; i++) {
        
        col = i % numCols;
        row = i / numCols;
        
        error = 0;
        
        // If a target image was given, consider proper target image block in error calculation
        if (targetImage != NULL) {
            // Put target block data in targetBorder and source block data in sourceBlock
            targetImage->readPixels(drawX, drawY, blockSize, blockSize, targetBlock);
            image->readPixels(posX(col), posY(row), blockSize, blockSize, sourceBlock);
            // Add luminance difference of each pixel to error
            for (int p = 0; p < blockArea; p += 3) {
                int targetLuminance = pixelLuminance(targetBlock[p], targetBlock[p+1], targetBlock[p+2]);
                int sourceLuminance = pixelLuminance(sourceBlock[p], sourceBlock[p+1], sourceBlock[p+2]);
                error += abs(targetLuminance - sourceLuminance);
            }
        }
        
        if (type == Right || type == Both) {
            // Get the area of the left border of the target block to compare and store in targetBorder
            image->readPixels(posX(col), posY(row), borderSize, blockSize, targetBorder);
            // Compare each pixel of sourceBorder and targetBorder
            for (int p = 0; p < borderArea; p+=3) {
                int rDif = sourceRightBorder[p] - targetBorder[p];
                int gDif = sourceRightBorder[p+1] - targetBorder[p+1];
                int bDif = sourceRightBorder[p+2] - targetBorder[p+2];
                int magnitude = sqrt(rDif * rDif + gDif * gDif + bDif * bDif);
                error += magnitude;
            }
        }
        
        if (type == Top || type == Both) {
            // Get the area of the bottom border of the target block to compare and store in targetBorder
            image->readPixels(posX(col), posY(row), blockSize, borderSize, targetBorder);
            // Compare each pixel of sourceBorder and targetBorder
            for (int p = 0; p < borderArea; p+=3) {
                int rDif = sourceTopBorder[p] - targetBorder[p];
                int gDif = sourceTopBorder[p+1] - targetBorder[p+1];
                int bDif = sourceTopBorder[p+2] - targetBorder[p+2];
                int magnitude = sqrt(rDif * rDif + gDif * gDif + bDif * bDif);
                error += magnitude;
            }
        }
    
        errorBlock b;
        b.error = error;
        b.index = i;
        possibleBlocks.push_back(b);
    }
    
    // Sort blocks so we can choose from the lowest errors
    auto sortErrorBlocks = [](errorBlock a, errorBlock b) { return a.error < b.error; };
    std::sort(possibleBlocks.begin(), possibleBlocks.end(), sortErrorBlocks);
    int chosenBlock = rand()%(blockChoosingRandomness);
    
    col = possibleBlocks[chosenBlock].index % numCols;
    row = possibleBlocks[chosenBlock].index / numCols;
    // Calculate minimum error border path for left border of chosen block
    if (type == Right || type == Both) {
        image->readPixels(posX(col), posY(row), borderSize, blockSize, targetBorder);
        if (targetImage != NULL) {
            targetImage->readPixels(drawX, drawY, borderSize, blockSize, targetBlock);
            getMinimumErrorPathWithTargetImage(targetBorder, sourceRightBorder, targetBlock, borderPathLeft, Right);
        }
        else
            getMinimumErrorPath(targetBorder, sourceRightBorder, borderPathLeft, Right);
    }
    // Calculate minimum error border path for bottom border of chosen block
    if (type == Top || type == Both) {
        image->readPixels(posX(col), posY(row), blockSize, borderSize, targetBorder);
        if (targetImage != NULL) {
            targetImage->readPixels(drawX, drawY, blockSize, borderSize, targetBlock);
            getMinimumErrorPathWithTargetImage(targetBorder, sourceTopBorder, targetBlock, borderPathBottom, Top);
        }
        else
            getMinimumErrorPath(targetBorder, sourceTopBorder, borderPathBottom, Top);
    }
    
    if (type == Right || type == Both)
        delete[] sourceRightBorder;
    if (type == Top || type == Both)
        delete[] sourceTopBorder;
    delete[] targetBorder;
    
    return possibleBlocks[chosenBlock].index;
}

// Find the minimum error path between the given borders (orientation given by type) and put in path param
void SourceImage::getMinimumErrorPath(GLubyte *targetBorder, GLubyte *sourceBorder, GLint *path, BlockMatch type) {
    
    // Create matrices for dynamic programming algorithm
    int **errorMatrix = new int*[blockSize];     // error for each pixel
    int **pathErrorMatrix = new int*[blockSize]; // best cumulative error of path up to this pixel
    int **pathMatrix = new int*[blockSize];      // holds the best next column to move to from this pixel
    for (int i = 0; i < blockSize; i++) {
        errorMatrix[i] = new int[borderSize];
        pathErrorMatrix[i] = new int[borderSize];
        pathMatrix[i] = new int[borderSize];
    }
    
    // Get pixel differences and fill matrices with initial values
    for (int i = 0; i < borderSize * blockSize * 3; i += 3) {
        int rDif = targetBorder[i] - sourceBorder[i];
        int gDif = targetBorder[i+1] - sourceBorder[i+1];
        int bDif = targetBorder[i+2] - sourceBorder[i+2];
        int magnitude = sqrt(rDif * rDif + gDif * gDif + bDif * bDif);
        
        int row, col;
        // Borders are stored in raster order
        // For left/right border, fill in matrices in raster order
        if (type == Right) {
            row = (i/3) / borderSize;
            col = (i/3) % borderSize;
        }
        // For top/bottom border, we have to pretend matrix is rotated
        // So the top row of the border maps to the right column of the matrix
        else {
            row = (i/3) % blockSize;
            col = borderSize - ((i/3) / blockSize) - 1;
        }
        errorMatrix[row][col] = magnitude;
        
        // For the last row, path error is equal to the error of that pixel
        // For other rows, set to -1 to know when it has not been set
        if (row == blockSize - 1)
            pathErrorMatrix[row][col] = magnitude;
        else
            pathErrorMatrix[row][col] = -1;
    }
    
    // Calculate best path for each pixel in first row
    int bestStartingPixel = 0;
    int bestPathError = pixelError(0, 0, errorMatrix, pathErrorMatrix, pathMatrix);
    for (int i = 1; i < borderSize; i++) {
        int error = pixelError(0, i, errorMatrix, pathErrorMatrix, pathMatrix);
        if (error < bestPathError) {
            bestPathError = error;
            bestStartingPixel = i;
        }
    }
    
    // Reconstruct best path and fill path array
    int nextBestCol = bestStartingPixel;
    path[0] = nextBestCol;
    for (int i = 1; i < blockSize; i++) {
        nextBestCol = pathMatrix[i-1][nextBestCol];
        path[i] = nextBestCol;
    }
    
    // Clean up memory
    for (int i = 0; i < blockSize; i++) {
        delete [] errorMatrix[i];
        delete [] pathErrorMatrix[i];
        delete [] pathMatrix[i];
    }
    delete[] errorMatrix;
    delete[] pathErrorMatrix;
    delete[] pathMatrix;
}

// Get minimum error path between two borders, taking error with target image into consideration
void SourceImage::getMinimumErrorPathWithTargetImage(GLubyte *sourceBorder1, GLubyte *sourceBorder2, GLubyte *targetImageBorder, GLint *path, BlockMatch type) {
    
    // Create matrices for dynamic programming algorithm
    int **errorMatrix = new int*[blockSize];     // error for each pixel
    int **errorMatrixLeft = new int*[blockSize];    // error for each pixel between sourceBorder2 and targetImageBorder (left side of the border of the left block, right border of that block)
    int **errorMatrixRight = new int*[blockSize];    // error for each pixel between sourceBorder1 and targetImageBorder (right side of the border of the right block, left border of that block)
    int **pathErrorMatrix = new int*[blockSize]; // best cumulative error of path up to this pixel
    int **pathMatrix = new int*[blockSize];      // holds the best next column to move to from this pixel
    for (int i = 0; i < blockSize; i++) {
        errorMatrix[i] = new int[borderSize];
        errorMatrixLeft[i] = new int[borderSize];
        errorMatrixRight[i] = new int[borderSize];
        pathErrorMatrix[i] = new int[borderSize];
        pathMatrix[i] = new int[borderSize];
    }
    
    // Construct error matrix
    // Algo: the error of a pixel is (the sum of the errors with the left border up to that pixel from the left) plus (the sum of the errors with right border up to that pixel from the right)
    // So with the left border, pixel error is error sum up to that pixel from the left
    // And with the right border, pixel error is error sum up to and including that pixel from the right
    
    // First, get initial pixel errors with target image and store in errorMatrixLeft and errorMatrixRight
    for (int i = 0; i < borderSize * blockSize * 3; i += 3) {
        int row, col;
        // Borders are stored in raster order
        // For left/right border, fill in matrices in raster order
        if (type == Right) {
            row = (i/3) / borderSize;
            col = (i/3) % borderSize;
        }
        // For top/bottom border, we have to pretend matrix is rotated
        // So the top row of the border maps to the right column of the matrix
        else {
            row = (i/3) % blockSize;
            col = borderSize - ((i/3) / blockSize) - 1;
        }
        
        // We are comparing luminance values
        int targetLuminance = pixelLuminance(targetImageBorder[i], targetImageBorder[i+1], targetImageBorder[i+2]);
        int sourceLuminance = pixelLuminance(sourceBorder1[i], sourceBorder1[i+1], sourceBorder1[i+2]);
        
        errorMatrixRight[row][col] = abs(targetLuminance - sourceLuminance);
        
        sourceLuminance = pixelLuminance(sourceBorder2[i], sourceBorder2[i+1], sourceBorder2[i+2]);
        
        errorMatrixLeft[row][col] = abs(targetLuminance - sourceLuminance);
        
        // Also, initially fill errorMatrix with errors between two blocks being drawn
        int rDif = sourceBorder1[i] - sourceBorder2[i];
        int gDif = sourceBorder1[i+1] - sourceBorder2[i+1];
        int bDif = sourceBorder1[i+2] - sourceBorder2[i+2];
        int magnitude = sqrt(rDif * rDif + gDif * gDif + bDif * bDif);
        
        errorMatrix[row][col] = magnitude;
    }
    // Then find row errors and construct errorMatrix
    for (int r = 0; r < blockSize; r++) {
        int leftError = 0, rightError = 0;
        // Replace values of errorMatrixLeft with values summed up to c from left
        for (int c = 0; c < borderSize; c++) {
            leftError += errorMatrixLeft[r][c];
            errorMatrixLeft[r][c] = leftError;
        }
        // Replace values of errorMatrixRight with values summed up to c from right
        for (int c = borderSize - 1; c >= 0; c--) {
            rightError += errorMatrixRight[r][c];
            errorMatrixRight[r][c] = rightError;
        }
        // Fill errorMatrix
        for (int c = 0; c < borderSize; c++) {
            // The error of a pixel equals (the sum of the errors with left block border in that row up to that pixel) plus (the sum of the errors with the right block border in that row up to that pixel)
            // i.e. At c == 0, pixel error = sum of errors in that row between target border and right block border
            // and at c == borderSize-1, pixel error = sum of errors in that row between target border and left block border
            // Note: we are also adding in the pixel error between the two blocks being drawn
            if (c == 0)
                errorMatrix[r][c] += errorMatrixRight[r][c];
            else
                errorMatrix[r][c] += errorMatrixRight[r][c] + errorMatrixLeft[r][c-1];
            
            // For the last row, path error is equal to the error of that pixel
            // For other rows, set to -1 to know when it has not been set
            if (r == blockSize - 1)
                pathErrorMatrix[r][c] = errorMatrix[r][c];
            else
                pathErrorMatrix[r][c] = -1;
        }
    }
    
    // Calculate best path for each pixel in first row
    int bestStartingPixel = 0;
    int bestPathError = pixelError(0, 0, errorMatrix, pathErrorMatrix, pathMatrix);
    for (int i = 1; i < borderSize; i++) {
        int error = pixelError(0, i, errorMatrix, pathErrorMatrix, pathMatrix);
        if (error < bestPathError) {
            bestPathError = error;
            bestStartingPixel = i;
        }
    }
    
    // Reconstruct best path and fill path array
    int nextBestCol = bestStartingPixel;
    path[0] = nextBestCol;
    for (int i = 1; i < blockSize; i++) {
        nextBestCol = pathMatrix[i-1][nextBestCol];
        path[i] = nextBestCol;
    }
    
    // Clean up memory
    for (int i = 0; i < blockSize; i++) {
        delete [] errorMatrix[i];
        delete [] errorMatrixLeft[i];
        delete [] errorMatrixRight[i];
        delete [] pathErrorMatrix[i];
        delete [] pathMatrix[i];
    }
    delete[] errorMatrix;
    delete[] errorMatrixLeft;
    delete[] errorMatrixRight;
    delete[] pathErrorMatrix;
    delete[] pathMatrix;
}

// Helper method for dynamic programming path generation
int SourceImage::pixelError(int r, int c, int **errorMatrix, int **pathErrorMatrix, int **pathMatrix) {
    // Derived from paper... E[r,c] = e[r,c] + min(E[r+1,c-1], E[r+1,c], E[r+1,c+1])
    
    // Calculate value of path at this pixel if it hasn't been calculated yet
    if (pathErrorMatrix[r][c] < 0) {
        int min, cL = -1, cC = -1, cR = -1;
        
        // Find best path with dynamic programming, don't go beyond bounds of border
        if (c == 0) {
            cC = pixelError(r+1, c, errorMatrix, pathErrorMatrix, pathMatrix);
            cR = pixelError(r+1, c+1, errorMatrix, pathErrorMatrix, pathMatrix);
            min = min2(cC, cR);
        }
        else if (c == borderSize - 1) {
            cL = pixelError(r+1, c-1, errorMatrix, pathErrorMatrix, pathMatrix);
            cC = pixelError(r+1, c, errorMatrix, pathErrorMatrix, pathMatrix);
            min = min2(cL, cC);
        }
        else {
            cL = pixelError(r+1, c-1, errorMatrix, pathErrorMatrix, pathMatrix);
            cC = pixelError(r+1, c, errorMatrix, pathErrorMatrix, pathMatrix);
            cR = pixelError(r+1, c+1, errorMatrix, pathErrorMatrix, pathMatrix);
            min = min3(cL, cC, cR);
        }
        
        // Set pathMatrix value; go left, center or right depending on best path
        if (min == cL)
            pathMatrix[r][c] = c - 1;
        else if (min == cC)
            pathMatrix[r][c] = c;
        else if (min == cR)
            pathMatrix[r][c] = c + 1;
        
        // Set pathErrorMatrix value, sum of error at this pixel and best path error up to this pixel
        pathErrorMatrix[r][c] = errorMatrix[r][c] + min;
    }
    return pathErrorMatrix[r][c];
}

// Get min of two ints
int SourceImage::min2(int a, int b) {
    if (a < b)
        return a;
    return b;
}

// Get min of three ints
int SourceImage::min3(int a, int b, int c) {
    if (a < b)
        b = a;
    if (b < c)
        return b;
    return c;
}

// Get the luminance value from RGB set
int SourceImage::pixelLuminance(int r, int g, int b) {
    // R G and B are weighed differently because of human eye color sensitivity
    return 0.299 * r + 0.587 * g + 0.114 * b;
}

void SourceImage::drawFullImage() {
    image->drawFullImage();
}

// Draw the block at index at x,y on the screen
void SourceImage::drawBlock(int index, int drawX, int drawY, int *borderPathLeft, int *borderPathBottom) {
    // Get x and y of block on the source image
    int srcBlockX = posX(index % numCols);
    int srcBlockY = posY(index / numCols);
    
    // Array that will store row of pixels to be drawn at a time
    GLubyte *blockRow = new GLubyte[blockSize * 3];
    
    glDrawBuffer(GL_FRONT);
    // Draw each row of pixels for this block individually
    // For each block, we account for the left and bottom borders, and draw the right and top flat
    for (int y = 0; y < blockSize; y++) {
        // Read where the border path indicates for this pixel row
        int xRead = srcBlockX + borderPathLeft[y];
        int yRead = srcBlockY + y;
        
        // For bottom border section, draw pixels one at a time
        if (y < borderSize) {
            for (int x = borderPathLeft[y]; x < blockSize; x++) {
                // Draw this pixel if the border path is at or below this row at this column
                if (borderPathBottom[x] <= y) {
                    image->readPixels(srcBlockX + x, yRead, 1, 1, blockRow);
                    glRasterPos2i(drawX + x, drawY + y);
                    glDrawPixels(1, 1, GL_RGB, GL_UNSIGNED_BYTE, blockRow);
                }
            }
        }
        // For rest of block, just draw full row starting at left border
        else {
            image->readPixels(xRead, yRead, blockSize - borderPathLeft[y], 1, blockRow);
            glRasterPos2i(drawX + borderPathLeft[y], drawY + y);
            glDrawPixels(blockSize - borderPathLeft[y], 1, GL_RGB, GL_UNSIGNED_BYTE, blockRow);
        }
    }
    
    delete[] blockRow;
}