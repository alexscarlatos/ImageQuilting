//
//  main.cpp
//  Image Quilting
//
//  Created by Alex Scarlatos on 3/1/16.
//  Copyright © 2016 Alex Scarlatos. All rights reserved.
//

#include <iostream>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "math.h"
#include "SourceImage.hpp"
#include "Texture.hpp"

int width, height;

GLubyte *Picture; // Array of pixels
int filetype;
enum{PGM, PPM};

unsigned char * data;

SourceImage *sourceImage;
Texture *texture;

void readPPM(const char *filename, GLubyte **pic) {
    FILE *file;
    char line[80];
    int i, j, size, rowSize;
    GLubyte *ptr;
    
    file = fopen(filename, "rb");
    fgets(line, 80, file);
    if (line[1] == '5')
        filetype = PGM;
    else if (line[1] == '6')
        filetype = PPM;
    else {
        printf("Invalid file type.");
        exit(1);
    }
    
    fgets(line, 80, file);
    sscanf(line, "%d %d", &width, &height);
    printf("Width: %d, Height: %d\n", width, height);
    if (filetype == PGM) {
        size = width * height; // 1 byte per pixel
        rowSize = width;
    }
    else {
        size = width * height * 3; // 3 bytes per pixel
        rowSize = width * 3;
    }
    *pic = (GLubyte *)malloc(size);
    
    fgets(line, 80, file);
    
    ptr = *pic + (height-1) * rowSize;
    for (i = height; i > 0; i--) {
        j = fread((void *)ptr, 1, rowSize, file);
        ptr -= rowSize;
    }
    fclose(file);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 1.0);	// Set line color to blue
    
    /*	Set viewing transformation	*/
    glLoadIdentity();	// ** Clear the matrix stack
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    
    /*	Scale cube and display as wire-frame parallelepiped.	*/
    glScalef(1.5, 2.0, 1.0);
    glutWireCube(1.0);
    
    /*	Scale, translate, and display wire-frame dodecahedron	*/
    glScalef(0.8, 0.5, 0.8);
    glTranslatef(-6.0, -5.0, 0.0);
    glutWireDodecahedron();
    
    /*	Translate and display wire-frame tetrahedron	*/
    glTranslatef(8.6, 8.6, 2.0);
    glutWireTetrahedron();
    
    /*	Translate and display wire-frame octahedron	*/
    glTranslatef(-3.0, -1.0, 0.0);
    glutWireOctahedron();
    
    /*	Scale, translate, and displaywire-frame icosahedron	*/
    glScalef(0.8, 0.8, 1.0);
    glTranslatef(4.3, -2.0, 0.5);
    glutWireIcosahedron();
    
    glFlush();
}

void winReshapeFcn(GLint newWidth, GLint newHeight)
{
    double viewRangeX = (double)newWidth / width,
    viewRangeY = (double)newHeight / height;
    glViewport(0, 0, newWidth, newHeight);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();	// ** Clear the matrix stack
    glFrustum(-viewRangeX, viewRangeX, -viewRangeY, viewRangeY, 2.0, 20.0);	// ** Adjust the view range
    glMatrixMode(GL_MODELVIEW);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
}

void mouseClick(int button, int state, int x, int y) {
    
}



//------ MAIN FUNCTIONS ------

void init(void) {
    sourceImage = new SourceImage("furTex.bmp", 10, 0);
    texture = new Texture(sourceImage, 500, 400);
    texture->generateTexture();
    
    glClearColor(1, 1, 1, 0);
}

int main(int argc, char * argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutCreateWindow("Wire-Frame Polyhedra");
    
    init();
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glutDisplayFunc(display);
    glutReshapeFunc(winReshapeFcn);
    
    glutMainLoop();
}