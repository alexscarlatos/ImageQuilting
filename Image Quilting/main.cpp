//
//  main.cpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 3/1/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <math.h>

#include <iostream>
#include <cstdlib>
#include "SourceImage.hpp"
#include "Texture.hpp"
#include "Image.hpp"


bool debugging = false;

SourceImage *sourceImage;
Image *targetImage = NULL;
Texture *texture = NULL;

// This is for creating the images in debug mode
void createDebugImages()
{
    bool useTargetImg = false;
    int width = 100, height = 100;
    int blockSize = 10, borderSize = 3, randomness = 2;
    
    sourceImage = new SourceImage("/Users/alexscarlatos/Documents/GitHub/ImageQuilting/Executable/fakeGrass.bmp", blockSize, borderSize, randomness);
    if (useTargetImg) {
        targetImage = new Image("/Users/alexscarlatos/Documents/GitHub/ImageQuilting/Executable/potato.ppm");
        texture = new Texture(sourceImage, targetImage);
    } else {
        texture = new Texture(sourceImage, width, height);
    }
}

// OpenGL function for displaying image
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    if (texture != NULL)
        texture->drawTexture();
    
    glFlush();
}

// OpenGL function for window resizing
void reshape(GLint newWidth, GLint newHeight)
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, newWidth, 0.0, newHeight);
    
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
#ifdef DEBUG
    debugging = true;
    std::cout << "Debug Mode\n";
#endif
    if (debugging)
        createDebugImages();
    else {
        // Parse arguments and create classes or exit if necessary
        if (argc == 2 && strcmp(argv[1], "-h") == 0) {
            std::cout << "Texture synthesis: source_image_path block_size border_size randomness width height\n";
            std::cout << "Texture transfer: source_image_path block_size border_size randomness target_image_path\n";
            exit(0);
        }
        // args for synthesis: executable sourceImage blockSize borderSize randomness width height
        else if (argc == 7) {
            sourceImage = new SourceImage(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
            texture = new Texture(sourceImage, atoi(argv[5]), atoi(argv[6]));
        }
        // args for transfer: executable sourceImage blockSize borderSize randomness targetImage
        else if (argc == 6) {
            sourceImage = new SourceImage(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
            targetImage = new Image(argv[5]);
            texture = new Texture(sourceImage, targetImage);
        }
        else {
            std::cout << "Invalid number of arguments.\n";
            std::cout << "Texture synthesis: source_image_path block_size border_size randomness width height\n";
            std::cout << "Texture transfer: source_image_path block_size border_size randomness target_image_path\n";
            exit(0);
        }
    }
    
    // Generate the texture
    texture->generateTexture();
    
    // Set up openGL, which will render the texture
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(texture->getWidth(), texture->getHeight());
    glutCreateWindow("Image Quilting");
    glClearColor(1.0, 1.0, 1.0, 0.0);   // White display window
    glClear(GL_COLOR_BUFFER_BIT);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutMainLoop();
}
