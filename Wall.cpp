#include <GL/glut.h>
#include "Tools.h"
#include "Wall.h"

#include <stdio.h>

Wall::Wall(int n, float** p, float r, float g, float b){

  points = new float*[n];
  for(int i=0; i<n; i++){
    points[i] = new float[3];
    for(int j=0; j<3; j++){
      points[i][j] = p[i][j];
    }
  }

  pointNum = n;
  normal = getNormal(points);

  disable = 0;

  color[0] = r;
  color[1] = g;
  color[2] = b;

  hasPortal = 0;

   A = points[0][1]*(points[1][2] - points[2][2]) + points[1][1]*(points[2][2] - points[0][2]) + points[2][1]*(points[0][2] - points[1][2]);
   B = points[0][2]*(points[1][0] - points[2][0]) + points[1][2]*(points[2][0] - points[0][0]) + points[2][2]*(points[0][0] - points[1][0]);
	 C = points[0][0]*(points[1][1] - points[2][1]) + points[1][0]*(points[2][1] - points[0][1]) + points[2][0]*(points[0][1] - points[1][1]);
	 D = -(
		 points[0][0]*(points[1][1]*points[2][2] - points[2][1]*points[1][2])
		+points[1][0]*(points[2][1]*points[0][2] - points[0][1]*points[2][2])
		+points[2][0]*(points[0][1]*points[1][2] - points[1][1]*points[0][2]));


}

void Wall::draw(void){
  glColor3fv(color);

  glNormal3fv(normal);
  glBegin(GL_QUADS);
  for(int i=0; i<pointNum; i++){
    glVertex3fv(points[i]);
  }
  glEnd();

  
}

 
int Wall::checkCollision(float* p0, float* p1){
		  
	  float eq1 = A*p0[0] + B*p0[1] + C*p0[2] + D;

	  if(eq1 < 0) return 0;

	  float eq2 = A*p1[0] + B*p1[1] + C*p1[2] + D;
	  
	  if(eq2 > 0) return 0;



	  float dx = p1[0] - p0[0];
	  float dy = p1[1] - p0[1];
	  float dz = p1[2] - p0[2];

	  float division = A*dx + B*dy + C*dz;
	  if(division == 0.0) division = 1.0;
	  float k = (A*p0[0] + B*p0[1] + C*p0[2] + D)/division;
	  k = -k;

	  intersection[0] = p0[0] + k*dx;
	  intersection[1] = p0[1] + k*dy;
	  intersection[2] = p0[2] + k*dz;


	  float v1[3];
	  float v2[3];
	  float vr[3];
	  
	  for(int i=0;i<4;i++){
		  v1[0] = points[(i+1)%4][0] - points[i][0];
		  v1[1] = points[(i+1)%4][1] - points[i][1];
		  v1[2] = points[(i+1)%4][2] - points[i][2];

		  v2[0] = intersection[0] - points[i][0];
		  v2[1] = intersection[1] - points[i][1];
		  v2[2] = intersection[2] - points[i][2];

		  crossProduct(v1, v2, vr);

		  if(vr[0]*normal[0] + vr[1]*normal[1] + vr[2]*normal[2] < 0)
			  return 0;
	  }

	  return 1;

  }

  void Wall::checkCamCollision(float* p0, float* p1){

    if(checkCollision(p0, p1)){
      
	  float division = A*normal[0] + B*normal[1] + C*normal[2];
	  if(division == 0.0) division = 1.0;
	  float k2 = (A*p1[0] + B*p1[1] + C*p1[2] + D)/division;
	  k2 = -(k2*1.5);

	  p1[0] = p1[0] + k2*normal[0];
	  p1[1] = p1[1] + k2*normal[1];
	  p1[2] = p1[2] + k2*normal[2];

    }
  }
    