#ifndef WALL_H
#define WALL_H

#include <GL/glut.h>
#include "Tools.h"

class Wall {
  
public:
  float color[3];
  int pointNum;


  float A, B, C, D;
  
	float intersection[3];
  float *normal;
  float **points;

  Wall(int n, float **p, float r, float g, float b);

  char disable;
  char hasPortal;
  void draw(void);

  
    
  int checkCollision(float* p0, float* p1);
  void checkCamCollision(float* p0, float* p1);
};

#endif