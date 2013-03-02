
#include "Tools.h"

void normalize (float *v){
    float len = (float)(sqrt((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2])));

    if (len == 0.0f)
        len = 1.0f;

    v[0] /= len;
    v[1] /= len;
    v[2] /= len;
}

float* getNormal (float **v){
  float *ret = new float[3];
  float a[3], A[3];
  a[0] = v[0][0] - v[1][0];
  a[1] = v[0][1] - v[1][1];
  a[2] = v[0][2] - v[1][2];

  A[0] = v[1][0] - v[2][0];
  A[1] = v[1][1] - v[2][1];
  A[2] = v[1][2] - v[2][2];

  ret[0] = (a[1] * A[2]) - (a[2] * A[1]);
  ret[1] = (a[2] * A[0]) - (a[0] * A[2]);
  ret[2] = (a[0] * A[1]) - (a[1] * A[0]);

  normalize(ret);
	return ret;
}

float getLength(float *v){
	return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

double angleBetween(float *v1, float *v2){
	return acos( (v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2])/(getLength(v1)*getLength(v2)) )*180/PI;
}

void rotateVector(double angle, float* v1, float* v2){
	float modelview[16];

	glPushMatrix();

	glLoadIdentity();

	glRotatef(angle, v2[0], v2[1], v2[2]);
	glTranslatef(v1[0], v1[1], v1[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX , modelview);

	glPopMatrix();

	v1[0] = modelview[12];
	v1[1] = modelview[13];
	v1[2] = modelview[14];

}

void crossProduct(float v1[3], float v2[3], float vr[3]){
	vr[0] = (v1[1]*v2[2] - v1[2]*v2[1]);
	vr[1] = (v1[2]*v2[0] - v1[0]*v2[2]);
	vr[2] = (v1[0]*v2[1] - v1[1]*v2[0]);
}


void inverseMatrix(float **A, float** X){

  int i,j;
  float x,n=0;

     
  for(i=0,j=0;j<3;j++) {     
    if(j==2) 
      n+=A[i][j]*A[i+1][0]*A[i+2][1];
    else if(j==1)
      n+=A[i][j]*A[i+1][j+1]*A[i+2][0];
    else 
      n+=A[i][j]*A[i+1][j+1]*A[i+2][j+2];
  }
  for(i=2,j=0;j<3;j++){     
    if(j==2) 
      n-=A[i][j]*A[i-1][0]*A[i-2][1];
	else if(j==1)
      n-=A[i][j]*A[i-1][j+1]*A[i-2][0];
    else
      n-=A[i][j]*A[i-1][j+1]*A[i-2][j+2];
  }

  x=1.0/n;
 

  X[0][0]=(+x)*(A[1][1]*A[2][2]-(A[1][2]*A[2][1]));
  X[0][1]=(-x)*(A[0][1]*A[2][2]-(A[0][2]*A[2][1]));
  X[0][2]=(+x)*(A[0][1]*A[1][2]-(A[0][2]*A[1][1]));
     
  X[1][0]=(-x)*(A[1][0]*A[2][2]-A[1][2]*A[2][0]);
  X[1][1]=(+x)*(A[0][0]*A[2][2]-A[0][2]*A[2][0]);
  X[1][2]=(-x)*(A[0][0]*A[1][2]-A[0][2]*A[1][0]);

  X[2][0]=(+x)*(A[1][0]*A[2][1]-A[1][1]*A[2][0]);
  X[2][1]=(-x)*(A[0][0]*A[2][1]-A[0][1]*A[2][0]);
  X[2][2]=(+x)*(A[0][0]*A[1][1]-A[0][1]*A[1][0]);
   
}