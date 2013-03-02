#ifndef TOOLS_H
#define TOOLS_H

#include <math.h>
#include <GL/glut.h>

#define PI 3.14159265



GLuint loadTexture(char *);
void normalize(float*);
float* getNormal(float**);
float getLength(float*);
double angleBetween(float*, float*);
void rotateVector(double, float*, float*);
void crossProduct(float[3], float[3], float[3]);

void inverseMatrix(float**, float**);



#endif