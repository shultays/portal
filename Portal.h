#include <stdio.h>
#include <math.h>
#include "Wall.h"

class Portal {
public:
  float matr[16];
  float inverse[16];
  float coor[3];
  float lookX[4];
  float lookY[4];
  float lookZ[4];

  float matrR[16];
  float d;

  float coor2[4];

  Wall *onThis;
  
  float points[8][3];
  float A,B,C,D;


  float color[3];


  void setMatr(){
  	int i;
    float *matr1[3];
    matr1[0] = lookX;
    matr1[1] = lookY;
    matr1[2] = lookZ;

  
    float **inverse1 = new float*[3];
	inverse1[0] = new float[4];
	inverse1[1] = new float[4];
	inverse1[2] = new float[4];

    inverseMatrix(matr1, inverse1);
	  

    for(i=0; i<3; i++){
	    for(int j=0; j<3; j++){
        inverse[i + j*4] = inverse1[i][j];
         if(i != 1) matr[i + j*4] = -matr1[i][j];
         else matr[i + j*4] = matr1[i][j];
         matrR[i + j*4] = matr1[i][j];
      }
    }

  
    matr[3] = 0;
    matr[7] = 0;
	matr[11] = 0;
	matr[15] = 1;
	matr[14] = 0;
	matr[13] = 0;
	matr[12] = 0;
  
	matrR[3] = 0;
	matrR[7] = 0;
	matrR[11] = 0;
	matrR[15] = 1;
	matrR[14] = 0;
	matrR[13] = 0;
	matrR[12] = 0;
  
	inverse[3] = 0;
	inverse[7] = 0;
	inverse[11] = 0;
	inverse[15] = 1;
	inverse[14] = 0;
	inverse[13] = 0;
	inverse[12] = 0;

    d = sqrt(coor[0]*coor[0] + coor[1]*coor[1] + coor[2]*coor[2]);
 
    coor2[0] = (coor[0]*lookX[0] + coor[1]*lookX[1] + coor[2]*lookX[2]);
    coor2[1] = (coor[0]*lookY[0] + coor[1]*lookY[1] + coor[2]*lookY[2]);
    coor2[2] = (coor[0]*lookZ[0] + coor[1]*lookZ[1] + coor[2]*lookZ[2]);

    points[0][0] = coor[0] + lookX[0]*90 + lookY[0]*150;
    points[0][1] = coor[1] + lookX[1]*90 + lookY[1]*150;
    points[0][2] = coor[2] + lookX[2]*90 + lookY[2]*150;

    points[1][0] = coor[0] + lookX[0]*90 - lookY[0]*150;
    points[1][1] = coor[1] + lookX[1]*90 - lookY[1]*150;
    points[1][2] = coor[2] + lookX[2]*90 - lookY[2]*150;

    points[2][0] = coor[0] - lookX[0]*90 - lookY[0]*150;
    points[2][1] = coor[1] - lookX[1]*90 - lookY[1]*150;
    points[2][2] = coor[2] - lookX[2]*90 - lookY[2]*150;

    points[3][0] = coor[0] - lookX[0]*90 + lookY[0]*150;
    points[3][1] = coor[1] - lookX[1]*90 + lookY[1]*150;
    points[3][2] = coor[2] - lookX[2]*90 + lookY[2]*150;

    points[4][0] = coor[0] + lookX[0]*70 + lookY[0]*130;
    points[4][1] = coor[1] + lookX[1]*70 + lookY[1]*130;
    points[4][2] = coor[2] + lookX[2]*70 + lookY[2]*130;

    points[5][0] = coor[0] + lookX[0]*70 - lookY[0]*130;
    points[5][1] = coor[1] + lookX[1]*70 - lookY[1]*130;
    points[5][2] = coor[2] + lookX[2]*70 - lookY[2]*130;

    points[6][0] = coor[0] - lookX[0]*70 - lookY[0]*130;
    points[6][1] = coor[1] - lookX[1]*70 - lookY[1]*130;
    points[6][2] = coor[2] - lookX[2]*70 - lookY[2]*130;

    points[7][0] = coor[0] - lookX[0]*70 + lookY[0]*130;
    points[7][1] = coor[1] - lookX[1]*70 + lookY[1]*130;
    points[7][2] = coor[2] - lookX[2]*70 + lookY[2]*130;

    for( i=0; i<4;i++){
      for(int j=0; j<3; j++){
        points[i][j] += lookZ[j]*0.1;
      }
    }
    A = points[0][1]*(points[1][2] - points[2][2]) + points[1][1]*(points[2][2] - points[0][2]) + points[2][1]*(points[0][2] - points[1][2]);
    B = points[0][2]*(points[1][0] - points[2][0]) + points[1][2]*(points[2][0] - points[0][0]) + points[2][2]*(points[0][0] - points[1][0]);
	C = points[0][0]*(points[1][1] - points[2][1]) + points[1][0]*(points[2][1] - points[0][1]) + points[2][0]*(points[0][1] - points[1][1]);
	D = -(
		 points[0][0]*(points[1][1]*points[2][2] - points[2][1]*points[1][2])
		+points[1][0]*(points[2][1]*points[0][2] - points[0][1]*points[2][2])
		+points[2][0]*(points[0][1]*points[1][2] - points[1][1]*points[0][2]));


  }

  void draw(void){

    glColor3f(0,0,0);
    glNormal3fv(lookZ);
    glBegin(GL_QUADS);
    for(int i=0; i<4;i++){
      glVertex3fv(points[i]);
      //printf("%f %f %f \n", points[i][0], points[i][1], points[i][2]);
    }
    glEnd();
  }

  void drawFrame(int k){


    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPushMatrix();

    glBegin(GL_QUADS);
    
    glNormal3fv(lookZ);
    int i,j;
    for(j=0; j<4; j++){
      glColor4f(onThis->color[0], onThis->color[1], onThis->color[2], 1.0);
      for(i=0; i<2;i++){
        glVertex3fv(points[(i+j)%4]);
      }

      if(k) glColor4f(0.0, 0.0, 0.0, 1.0);
      else glColor4f(0.0, 0.0, 0.0, 0.0);
      for(i=1; i>=0;i--){
        glVertex3fv(points[4+(i+j)%4]);
      }

      
    }
    if(k){
      for(j=0;j<4;j++){
        glVertex3fv(points[4+j]);
      }
    }
    glEnd();

    glDisable(GL_BLEND);

    glPopMatrix();
  }

  void drawLine(){
    
    glLineWidth(10);
    glColor3d(1,0,0);
    glPushMatrix();
    glTranslatef(lookZ[0]*10, lookZ[1]*10, lookZ[2]*10);
    glBegin(GL_LINES);
      glVertex3f(coor[0] + lookY[0]*90
        ,coor[1] + lookY[1]*90
        ,coor[2] + lookY[2]*90
        );

      glVertex3fv(coor);

    glColor3d(0,0,1);
      glVertex3f(coor[0] + lookX[0]*90
        ,coor[1] + lookX[1]*90
        ,coor[2] + lookX[2]*90
        );

      glVertex3fv(coor);
    glEnd();

    glPopMatrix();

  }

  
  int checkCollision(float* p0, float* p1){
		  
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
	  float intersection[3];
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

		  if(vr[0]*lookZ[0] + vr[1]*lookZ[1] + vr[2]*lookZ[2] < 0)
			  return 0;
	  }


	  return 1;

  }

};