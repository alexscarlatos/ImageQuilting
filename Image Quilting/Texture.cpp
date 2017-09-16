//
//  Texture.cpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 3/31/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#include "Texture.hpp"
#include <iostream>

// Constructor for texture for synthesis
Texture::Texture(SourceImage *sImage, int w, int h) {
    sourceImage = sImage;
    targetImage = NULL;
    
    // Select enough cols and rows to fill out width and height, and add one to each for the ends
    cols = 1 + w / (sourceImage->blockSize - sourceImage->borderSize);
    rows = 1 + h / (sourceImage->blockSize - sourceImage->borderSize);
    
    std::cout << "Texture: numCols:"<<cols<<" numRows:"<<rows<<"\n";
    
    width = w;
    height = h;
}

// Constructor for redrawing an image with a texture
// Texture image is sImage and image to redraw is tImage
Texture::Texture(SourceImage *sImage, Image *tImage) {
    sourceImage = sImage;
    targetImage = tImage;
    
    // Select enough cols and rows to fill out width and height, and add one to each for the ends
    cols = 1 + tImage->width / (sourceImage->blockSize - sourceImage->borderSize);
    rows = 1 + tImage->height / (sourceImage->blockSize - sourceImage->borderSize);
    
    std::cout << "Texture: numCols:"<<cols<<" numRows:"<<rows<<"\n";
    
    width = tImage->width;
    height = tImage->height;
}

Texture::~Texture() {
    for (int i = 0; i < blocks.size(); i++) {
        delete [] blocks[i]->borderPathLeft;
        delete [] blocks[i]->borderPathBottom;
        delete blocks[i];
    }
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}

// Function to generate the data for this texture
void Texture::generateTexture() {
    int lastPercentage = 0, newPercentage = 0;
    int blockIndex = 0; // The index in the source image of the next block to place in the texture
    // Find an appropriate block and border path for every index in the texture
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            // Create a new block for this row,col
            block *curBlock = new block;
            curBlock->borderPathLeft = new int[sourceImage->blockSize];
            curBlock->borderPathBottom = new int[sourceImage->blockSize];
            curBlock->x = c * (sourceImage->blockSize - sourceImage->borderSize);
            curBlock->y = r * (sourceImage->blockSize - sourceImage->borderSize);
            curBlock->size = sourceImage->blockSize;
            
            // Choose first block (lower left corner)
            if (c == 0 && r == 0) {
                if (targetImage != NULL)
                    blockIndex = sourceImage->findMinimumErrorBlock(0, 0, None, NULL, NULL, targetImage, 0, 0);
                else
                    blockIndex = sourceImage->getRandomBlock();
                // Zero out border paths
                for (int i = 0; i < sourceImage->blockSize; i++) {
                    curBlock->borderPathLeft[i] = 0;
                    curBlock->borderPathBottom[i] = 0;
                }
            }
            // For first row, only compare blocks horizontally
            else if (r == 0) {
                blockIndex = sourceImage->findMinimumErrorBlock(blockIndex, 0, BlockMatch::Right, curBlock->borderPathLeft, NULL, targetImage, curBlock->x, curBlock->y);
                for (int i = 0; i < sourceImage->blockSize; i++)
                    curBlock->borderPathBottom[i] = 0;
            }
            // For first col (along left edge) only compare blocks vertically
            else if (c == 0) {
                int blockBelow = blocks[(r-1)*cols + c]->sourceImageIndex;
                blockIndex = sourceImage->findMinimumErrorBlock(0, blockBelow, BlockMatch::Top, NULL, curBlock->borderPathBottom, targetImage, curBlock->x, curBlock->y);
                for (int i = 0; i < sourceImage->blockSize; i++)
                    curBlock->borderPathLeft[i] = 0;
            }
            // For every other block, compare block to left and block below of new block
            else {
                int blockBelow = blocks[(r-1)*cols + c]->sourceImageIndex;
                blockIndex = sourceImage->findMinimumErrorBlock(blockIndex, blockBelow, BlockMatch::Both, curBlock->borderPathLeft, curBlock->borderPathBottom, targetImage, curBlock->x, curBlock->y);
            }

            curBlock->sourceImageIndex = blockIndex;
            blocks.push_back(curBlock);
            
            newPercentage = (int)blocks.size() * 100 / (rows * cols);
            if (newPercentage > lastPercentage) {
                std::cout << newPercentage << "%\n";
                lastPercentage = newPercentage;
            }
        }
    }
}

void Texture::drawTexture() {
    // Draw each block in the source image to this texture
    for (int i = 0; i < blocks.size(); i++) {
        sourceImage->drawBlock(blocks[i]->sourceImageIndex, blocks[i]->x, blocks[i]->y, blocks[i]->borderPathLeft, blocks[i]->borderPathBottom);
    }
}