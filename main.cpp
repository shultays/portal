#include <GL/glfw.h> 
#include <stdlib.h>

#include "Wall.h"
#include "Portal.h"
#include <stdio.h>


Wall *walls[6]; //duvarlari tutan array
int wallN; //duvar sayisi (aslinda sabit 6, ilerde belki arttiririm diye)
 
float camN[3] = {0, 0, 0}; //cameranin kordinati
float camX[3] = {1, 0, 0}; //cameranin x vektoru
float camY[3] = {0, 1, 0}; // y vektoru
float camZ[3] = {0, 0, 1}; //kameranin baktigi yon
 
void mouse();
void timerf();
void click();
 
float points[4][3];
float disabled=0;
float ciz=0;
int bt=0;
Portal portalA, portalB; //iki tane portal

double r = 60;
 
GLUquadric *quadric = gluNewQuadric(); 

void drawScene(void){
  //sahnedeki cizen fonksiyon (silindirdir, dairedir)
  
  glColor3f(0.0,0.7, 0.0);
  glPushMatrix();
    glTranslated(0,0,-160);
    glRotatef(r, 1,0,0);
    glTranslated(0,0,-60);
    
    gluCylinder(quadric, 60.0f, 100.0f, 120.0f, 20, 20); 
    glRotatef(180, 1, 1, 0);
    gluDisk(quadric, 0, 60, 20, 60);
    glTranslated(0,0, -120);
    glRotatef(180, 1, 1, 0);
    gluDisk(quadric, 0, 100, 20, 60);
  glPopMatrix();
    
  glColor3f(0.7,0.7, 0.0);
  glPushMatrix();
    glTranslated(160,0,160);
    glRotated(r,1,1,0);
    gluDisk(quadric, 0, 120, 20, 60);
    glRotated(180,1,1,0);
    gluDisk(quadric, 0, 120, 20, 60);
   
  glPopMatrix();

  glColor3f(0.7,0.0,0.4);
  glPushMatrix();
    glTranslated(-300, 0, 0);
    glRotated(r,1,1,0);

    gluSphere( quadric, 40, 6, 4);
  glPopMatrix();    
  
  r += 0.05;
}

float speed = 4; //hareket hizi

float forward = 0; //hangi yonlere gidildigini tutan degerler
float sideward = 0;
float upward = 0;

Portal *portals[3]; //portal nesnelerinin addreslerini tutan array
					//0 ve 2. elemanlar ayni eleman

void controlCam(void){
  //basilan tuslara gore kamerayi haraket ettiren fonksiyon
  glfwPollEvents(); //basilmis tuslari refresh et

  if(glfwGetKey('W') == GLFW_PRESS){ //hangi tusun basildigina gore hizlari belirle
    forward = 1.0;
  }else if(glfwGetKey('S') == GLFW_PRESS){
    forward = -1.0;
  }else forward = 0;
  if(glfwGetKey('A') == GLFW_PRESS){
    sideward = -1.0;
  }else if(glfwGetKey('D') == GLFW_PRESS){
    sideward = 1.0;
  }else sideward = 0;
  if(glfwGetKey('R') == GLFW_PRESS){
    upward = 1.0;
  }else if(glfwGetKey('F') == GLFW_PRESS){
    upward = -1.0;
  }else upward = 0;

  if(ciz) click(); //eger fare tiklanmis ve portal hareket ettirme modunda ise portali hareket ettir
}

void render(int t){
  //tum nesneleri ve duvarlari cizecek olan fonksiyon
  drawScene();
  for(int i=0; i<wallN;i++){
    if(!walls[i]->disable || t == 1) walls[i]->draw(); //duruma gore portalin uzerinde oldugu duvarlar cizilmeyebilir (daha sonra deginilecek)
  }
}


float d = 0;

static void display(){
  int i;
  mouse(); //fare de bir degisiklik var mi bak
  click(); //fare tiklanmis mi kontrol et
  if( glfwGetTime() > 0.02){ //eger belirli bir sure gecti ise timer fonksiyonunu calistir (saniyede 50 kere)
  	glfwSetTime(0.0);
  	
  	timerf();
  }
  
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //butun buffer lari temizle
  
  glClearDepth(1.0); //depth i temizle
  
  glLoadIdentity();
  gluLookAt( camN[0] - camZ[0],  camN[1] - camZ[1],  camN[2] - camZ[2], 
             camN[0], camN[1], camN[2],
             camY[0], camY[1], camY[2]); //kameranin bakis acisini ayarla
   


  
  //bu kisim portallari ve sahneyi duzgun bir sekilde cizecek
  
  if(portals[0]->onThis == portals[1]->onThis){ // eger iki portalda ayni duvar uzerinde ise durum biraz farkli

    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 
    glDisable(GL_DEPTH_TEST);
    portals[0]->draw(); //iki portalida stencil buffer a ciz
    portals[1]->draw();
    glEnable(GL_DEPTH_TEST);  

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0, 1);

    portals[0]->onThis->draw(); //portallarin cizilmedigi yerlere duvari ciz
  }
  


  for(i=0; i<2; i++){
    
    glEnable(GL_STENCIL_TEST); //bu kisimda portallarin gosterdigi bolumler ciziliyor
    //her iki portal icin dongu
    
    glStencilFunc(GL_ALWAYS, 1, 0);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    portals[i]->draw(); //portali stencil buffer a ciz
    glEnable(GL_DEPTH_TEST);  

    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    //burada yaptigi islem tum sahneyi portallari birbirine cakisacak sekilde hareket ettirmek
    //h y basilirsa vektorlerin cakistigi goruluyor
    
    glPushMatrix();
      glTranslatef(portals[i]->coor[0], portals[i]->coor[1], portals[i]->coor[2]);
   
      glMultMatrixf(portals[i]->inverse);
      glMultMatrixf(portals[i+1]->matr);
   
      glTranslatef(-portals[i+1]->coor[0], -portals[i+1]->coor[1], -portals[i+1]->coor[2]);
   

      portals[i+1]->onThis->disable = 1;
      render(1); //sahne hareket ettirilmis durumda iken diger portalin uzerinde oldugu duvar haric tum sahneyi render et
      
      
      if(glfwGetKey('H')) portals[i+1]->drawLine();
    
      //su anki portali siyah olarak ciz
      glTranslatef(portals[i]->lookZ[0]*4,portals[i]->lookZ[1]*4,portals[i]->lookZ[2]*4);
      portals[i]->drawFrame(1);

      //kameranin bulundugu yere kure ciz
      glTranslatef(camN[0], camN[1], camN[2]);
      glColor3f(0.5, 0.5, 0.5);
      gluSphere( quadric, 20, 20, 20);
 
     
    glPopMatrix(); 
    //buradada eger iki portal ayni yerde degilse portalin uzerinde oldugu duvari ciziyoruz (bu gercek duvar, portalin icinde olan degil)
    glStencilFunc(GL_EQUAL, 0, 1);
    if(portals[i+1]->onThis != portals[i]->onThis) portals[i]->onThis->draw();


  }

  glDisable(GL_STENCIL_TEST); //stencil ile isimiz bitti

  portalA.onThis->disable = 1; //her iki portalin uzerinde oldugu duvarlari iptal et
  portalB.onThis->disable = 1; //gercek sahneyi ciz
  render(0);
  portalA.drawFrame(0); //portallarin kenarlarini yari saydam olacak sekilde ciz
  portalB.drawFrame(0);

  if(ciz){ //eger fare basili tutuluyor ise portallari cizgi olarak ciz
    glLineWidth(4);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    if(disabled) glColor3f(0.0, 0.0, 0.0);
    else glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINES);
    for(i=0; i<4; i++){
      glVertex3fv(points[i]);
      glVertex3fv(points[(i+1)%4]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
  }
  
  portalA.onThis->disable = 0;
  portalB.onThis->disable = 0;
  if(glfwGetKey('H')) {
  	 portalA.drawLine(); //H basili ise portal vektorlerini ciz
     portalB.drawLine();
  }
  
}


void init(){
  //sadece baslangicta calisan duvarlari vs ayarlayan fonksiyyon
  float **p;
  
  p = new float*[4];
  p[0] = new float[3];
  p[1] = new float[3];
  p[2] = new float[3];
  p[3] = new float[3];

  //tum duvarlar icin kordinatlari ver
  p[0][0] = -400;  p[0][1] = +400;   p[0][2] = 400;
  p[1][0] = +400;  p[1][1] = +400;   p[1][2] = 400;
  p[2][0] = +400;  p[2][1] = -400;   p[2][2] = 400;
  p[3][0] = -400;  p[3][1] = -400;   p[3][2] = 400;
  walls[0] = new Wall(4, p, 1, 0, 0);
  

  p[0][0] = -400;  p[0][1] = -400;   p[0][2] = -400;
  p[1][0] = +400;  p[1][1] = -400;   p[1][2] = -400;
  p[2][0] = +400;  p[2][1] = +400;   p[2][2] = -400;
  p[3][0] = -400;  p[3][1] = +400;   p[3][2] = -400;
  walls[1] = new Wall(4, p, 0, 1, 0);
  
  p[0][0] = +400;  p[0][1] = -400;   p[0][2] = -400;
  p[1][0] = +400;  p[1][1] = -400;   p[1][2] = +400;
  p[2][0] = +400;  p[2][1] = +400;   p[2][2] = +400;
  p[3][0] = +400;  p[3][1] = +400;   p[3][2] = -400;
  walls[2] = new Wall(4, p, 0, 0, 1);

  p[0][0] = -400;  p[0][1] = +400;   p[0][2] = -400;
  p[1][0] = -400;  p[1][1] = +400;   p[1][2] = +400;
  p[2][0] = -400;  p[2][1] = -400;   p[2][2] = +400;
  p[3][0] = -400;  p[3][1] = -400;   p[3][2] = -400;
  walls[3] = new Wall(4, p, 1, 1, 0);

  p[0][0] = -400;  p[0][1] = +400;   p[0][2] = -400;
  p[1][0] = +400;  p[1][1] = +400;   p[1][2] = -400;
  p[2][0] = +400;  p[2][1] = +400;   p[2][2] = +400;
  p[3][0] = -400;  p[3][1] = +400;   p[3][2] = +400;
  walls[4] = new Wall(4, p, 1, 0, 1);
  
  p[0][0] = -400;  p[0][1] = -400;   p[0][2] = +400;
  p[1][0] = +400;  p[1][1] = -400;   p[1][2] = +400;
  p[2][0] = +400;  p[2][1] = -400;   p[2][2] = -400;
  p[3][0] = -400;  p[3][1] = -400;   p[3][2] = -400;
  walls[5] = new Wall(4, p, 0, 1, 1);

  wallN = 6; //duvar sayisi 6

  portalA.coor[0] = 0; //portalin baslangic degerleri
  portalA.coor[1] = 0;
  portalA.coor[2] = 400;

  portalA.lookZ[0] = 0; //portalin batigi yon
  portalA.lookZ[1] = 0;
  portalA.lookZ[2] = -1;

  portalA.lookX[0] = 0.6; //portalin x vektoru
  portalA.lookX[1] = 0.8;
  portalA.lookX[2] = 0;

  portalA.lookY[0] = 1;  //portalin y vektoru
  portalA.lookY[1] = 0;
  portalA.lookY[2] = 0;

  portalA.color[0] = 0.0;
  portalA.color[1] = 0.0;
  portalA.color[2] = 1.0;

  portalB.coor[0] = -400; //diger portal
  portalB.coor[1] = 0;
  portalB.coor[2] = 0;


  portalB.lookX[0] = 0;
  portalB.lookX[1] = 0;
  portalB.lookX[2] = 1;

  portalB.lookY[0] = 0;
  portalB.lookY[1] = 0;
  portalB.lookY[2] = 1;

  portalB.lookZ[0] = 1;
  portalB.lookZ[1] = 0;
  portalB.lookZ[2] = 0;


  crossProduct(portalB.lookX, portalB.lookZ, portalB.lookY); //portallarin vektorlerini cross product yaparak dik olalarindan emin oluyoruz
  crossProduct(portalA.lookX, portalA.lookZ, portalA.lookY); //cokta gerekli degilmis aslinda =)

  crossProduct(camX, camZ, camY); //kamerada ayni sekilde

  portalB.color[0] = 0.0;
  portalB.color[1] = 0.0;
  portalB.color[2] = 0.0;

  portalA.setMatr(); //bu fonksiyon portallarin iclerini cizmek icin gerekli olan matrix leri sagliyor
  portalB.setMatr();

  portalA.onThis = walls[0];
  portalB.onThis = walls[3];

  walls[0]->hasPortal = 1;
  walls[3]->hasPortal = 1;


  portals[0] = &portalA;
  portals[1] = &portalB;
  portals[2] = &portalA;
}

void mouse(){ //faremizin hareketine gore amerayi donduren fonksiyon
	int x, y;
	glfwGetMousePos(&x, &y);
	if(x != 240 || y != 240){
      //eger fare haraket etmisse vektorleri cevir
      float a = (240 - x)*0.5;
    
      rotateVector(a, camX, camY); 
      rotateVector(a, camZ, camY);

      float b = (240 - y)*0.5;
    
      rotateVector(b, camY, camX);
      rotateVector(b, camZ, camX);
      glfwSetMousePos(240, 240);
	}
}

void timerf(){
  //kamera hareket yonune gore onu hareet ettiren fonksiyon
  int i;
  
  float camOldN[3];
  camOldN[0] = camN[0]; //eski koordinatlari kaydet
  camOldN[1] = camN[1];
  camOldN[2] = camN[2];

  controlCam(); //hangi tuslar basili kontrol et

  camN[0] += camZ[0]*forward*speed; //tuslarin basili olmasina gore atanan forward, sideward and upward degerlerine gore kamerayi hareket ettir
  camN[1] += camZ[1]*forward*speed;
  camN[2] += camZ[2]*forward*speed;

  camN[0] += camX[0]*sideward*speed;
  camN[1] += camX[1]*sideward*speed;
  camN[2] += camX[2]*sideward*speed;
  
  camN[0] += camY[0]*upward*speed;
  camN[1] += camY[1]*upward*speed;
  camN[2] += camY[2]*upward*speed;

  for(i=0; i<2; i++){ // bu kisim portalin icinden geciyormuyuz diye kontrol ediyor
    if(portals[i]->checkCollision(camOldN, camN)){ //eger bir onceki koordinatimiz portalin arkasinda, digeri onunde ise 
      float rc[3];									//ve bu iki kordinatin ortasi portalin uzerinde ise portalndan gecmisiz
    
      //bu kisim portaldan ciktigimizda hangi yonlere bakacagiz, hangi koordinata gidecegiz onu ayarliyor
      //en cok kasan olaylardan birisi buydu =)
      
      rc[0] = -((camOldN[0]-portals[i]->coor[0])*portals[i]->lookX[0] + (camOldN[1]-portals[i]->coor[1])*portals[i]->lookX[1] + (camOldN[2]-portals[i]->coor[2])*portals[i]->lookX[2]);
      rc[1] = ((camOldN[0]-portals[i]->coor[0])*portals[i]->lookY[0] + (camOldN[1]-portals[i]->coor[1])*portals[i]->lookY[1] + (camOldN[2]-portals[i]->coor[2])*portals[i]->lookY[2]);
      rc[2] = -((camOldN[0]-portals[i]->coor[0])*portals[i]->lookZ[0] + (camOldN[1]-portals[i]->coor[1])*portals[i]->lookZ[1] + (camOldN[2]-portals[i]->coor[2])*portals[i]->lookZ[2]);

      camN[0] = (portals[i+1]->lookY[0]*rc[1]+portals[i+1]->lookX[0]*rc[0] + portals[i+1]->lookZ[0]*rc[2]) + portals[i+1]->coor[0];
      camN[1] = (portals[i+1]->lookY[1]*rc[1]+portals[i+1]->lookX[1]*rc[0] + portals[i+1]->lookZ[1]*rc[2]) + portals[i+1]->coor[1];
      camN[2] = (portals[i+1]->lookY[2]*rc[1]+portals[i+1]->lookX[2]*rc[0] + portals[i+1]->lookZ[2]*rc[2]) + portals[i+1]->coor[2];     

      rc[0] = -(camX[0]*portals[i]->lookX[0] + camX[1]*portals[i]->lookX[1] + camX[2]*portals[i]->lookX[2]);
      rc[1] = (camX[0]*portals[i]->lookY[0] + camX[1]*portals[i]->lookY[1] + camX[2]*portals[i]->lookY[2]);
      rc[2] = -(camX[0]*portals[i]->lookZ[0] + camX[1]*portals[i]->lookZ[1] + camX[2]*portals[i]->lookZ[2]);

      camX[0] = (rc[0]*portals[i+1]->lookX[0] + rc[1]*portals[i+1]->lookY[0] + rc[2]*portals[i+1]->lookZ[0]);
      camX[1] = (rc[0]*portals[i+1]->lookX[1] + rc[1]*portals[i+1]->lookY[1] + rc[2]*portals[i+1]->lookZ[1]);
      camX[2] = (rc[0]*portals[i+1]->lookX[2] + rc[1]*portals[i+1]->lookY[2] + rc[2]*portals[i+1]->lookZ[2]);
    
      rc[0] = -(camY[0]*portals[i]->lookX[0] + camY[1]*portals[i]->lookX[1] + camY[2]*portals[i]->lookX[2]);
      rc[1] = (camY[0]*portals[i]->lookY[0] + camY[1]*portals[i]->lookY[1] + camY[2]*portals[i]->lookY[2]);
      rc[2] = -(camY[0]*portals[i]->lookZ[0] + camY[1]*portals[i]->lookZ[1] + camY[2]*portals[i]->lookZ[2]);

      camY[0] = (rc[0]*portals[i+1]->lookX[0] + rc[1]*portals[i+1]->lookY[0] + rc[2]*portals[i+1]->lookZ[0]);
      camY[1] = (rc[0]*portals[i+1]->lookX[1] + rc[1]*portals[i+1]->lookY[1] + rc[2]*portals[i+1]->lookZ[1]);
      camY[2] = (rc[0]*portals[i+1]->lookX[2] + rc[1]*portals[i+1]->lookY[2] + rc[2]*portals[i+1]->lookZ[2]);

      rc[0] = -(camZ[0]*portals[i]->lookX[0] + camZ[1]*portals[i]->lookX[1] + camZ[2]*portals[i]->lookX[2]);
      rc[1] = (camZ[0]*portals[i]->lookY[0] + camZ[1]*portals[i]->lookY[1] + camZ[2]*portals[i]->lookY[2]);
      rc[2] = -(camZ[0]*portals[i]->lookZ[0] + camZ[1]*portals[i]->lookZ[1] + camZ[2]*portals[i]->lookZ[2]);

      camZ[0] = (rc[0]*portals[i+1]->lookX[0] + rc[1]*portals[i+1]->lookY[0] + rc[2]*portals[i+1]->lookZ[0]);
      camZ[1] = (rc[0]*portals[i+1]->lookX[1] + rc[1]*portals[i+1]->lookY[1] + rc[2]*portals[i+1]->lookZ[1]);
      camZ[2] = (rc[0]*portals[i+1]->lookX[2] + rc[1]*portals[i+1]->lookY[2] + rc[2]*portals[i+1]->lookZ[2]);

      break;
    }
  }
  
  if( i==2 ){ //eger portal icinden gecmediyse
    for(i=0; i<6; i++){ //duvarlarla collision var mi kontrol ett
      walls[i]->checkCamCollision(camOldN, camN); //varsa zaten o otomatik olarak kamerayi geri cekiyor
    }
    for(i=0; i<3; i++){ //koselerden kacmamak icin
      if(camN[i] < -400) camN[i] = -400;
      else if(camN[i] > 400) camN[i] = 400;
    }
  }
  
}
int oldState1 = GLFW_RELEASE;
int oldState2 = GLFW_RELEASE;

void click(){ //fare tiklamalarini kontrol eden fonksiyon
  int n, m;
  int state = -1, button = -1;
  
  //bu kisim programin glut surumunden glfw ye gecerken bir parca karisti
  //normalde state ve button degerlerini glut ta direk aliyorum
  //bu sefer  buttonun once ki halini ve yeni halini kontrol etmek durumunda kaliyorum
  
  if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) != oldState1){
  	oldState1 = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT); //eger sol fare tilandi veya birakildi ise
  	if(oldState1 == GLFW_RELEASE) state = 1;  //birakildi ise state = 1
  	else state = 0; //basildi ise 0
  	button = 0; //sol tus icin 0
  }
  
  if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) != oldState2){
  	oldState2 = glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
    if(oldState2 == GLFW_RELEASE) state = 1;
  	else state = 0;
  	button = 2;
  }
  if(!ciz && button == -1)return; //eger farelerden biri tiklanmamissa ve bir fare tusu basili tutulmuyorsa cik
  
  if(state == 0){ //fare tusu basildi ise
    ciz = 1; //portallari cizgi olarak ciz
    bt = button;
  }else if(state == 1){ 
    ciz = 0;
    bt = button;
  }
 
  disabled = 0;
  //baktigimiz yon bir duvar uzerinde mi ona bakiyoruz
  for(int i=0; i<6; i++){
  	//laser dedigimiz baktigimiz yonde 1000 koordinat lerdeki nokta
    float laser[3] = {camN[0] + camZ[0]*1000, camN[1] + camZ[1]*1000, camN[2] + camZ[2]*1000 };
    if(walls[i]->checkCollision(camN, laser)){ //eger laser ile su anki koordinat arasindaki dogru duvar ile cakisiyors


      float lookN1[3]; 
      lookN1[0] = walls[i]->intersection[0];  //portalin orta noktasi bu isinin kesisim noktasi
      lookN1[1] = walls[i]->intersection[1];
      lookN1[2] = walls[i]->intersection[2];


      //bu kisimda portalin y vektoru hesaplanacak
      //aslinda grafikle anlatmak daha iyi olacakti ama o kadar ugrasasim yok
      //temelde kameranin y vektoru ile ayni yonde olacak portalin y vektoru, ama tabi ayni zamanda duvarin normaline de dik olmali
	  float dx = walls[i]->normal[0];
	  float dy = walls[i]->normal[1];
	  float dz = walls[i]->normal[2];

	  float division = walls[i]->A*dx + walls[i]->B*dy + walls[i]->C*dz;
	  if(division == 0.0) division = 1.0;
	  float k = (walls[i]->A*camN[0] + walls[i]->B*camN[1] + walls[i]->C*camN[2] + walls[i]->D)/division;
	  k = -k;

      
      float n1[3];
	  n1[0] = camN[0] + k*dx;
	  n1[1] = camN[1] + k*dy;
	  n1[2] = camN[2] + k*dz;


	  k = (walls[i]->A*(camN[0]+camY[0]*16) + walls[i]->B*(camN[1]+camY[1]*16) + walls[i]->C*(camN[2]+camY[2]*16) + walls[i]->D)/division;
	  k = -k;

      float n2[3];
	  n2[0] = (camN[0]+camY[0]*16) + k*dx;
	  n2[1] = (camN[1]+camY[1]*16) + k*dy;
	  n2[2] = (camN[2]+camY[2]*16) + k*dz;
        

      float lookY1[3] = {n2[0] - n1[0], n2[1] - n1[1], n2[2] - n1[2] };

      normalize(lookY1);

      float lookX1[3]; //x vektoru de y vektorunun ve duvarin normalinin cross product i
      crossProduct(walls[i]->normal, lookY1, lookX1);
      
      points[0][0] = lookN1[0] + lookX1[0]*90 + lookY1[0]*150;
      points[0][1] = lookN1[1] + lookX1[1]*90 + lookY1[1]*150;
      points[0][2] = lookN1[2] + lookX1[2]*90 + lookY1[2]*150;

      points[1][0] = lookN1[0] + lookX1[0]*90 - lookY1[0]*150;
      points[1][1] = lookN1[1] + lookX1[1]*90 - lookY1[1]*150;
      points[1][2] = lookN1[2] + lookX1[2]*90 - lookY1[2]*150;

      points[2][0] = lookN1[0] - lookX1[0]*90 - lookY1[0]*150;
      points[2][1] = lookN1[1] - lookX1[1]*90 - lookY1[1]*150;
      points[2][2] = lookN1[2] - lookX1[2]*90 - lookY1[2]*150;

      points[3][0] = lookN1[0] - lookX1[0]*90 + lookY1[0]*150;
      points[3][1] = lookN1[1] - lookX1[1]*90 + lookY1[1]*150;
      points[3][2] = lookN1[2] - lookX1[2]*90 + lookY1[2]*150;

      float v1[3], v2[3], vr[3];


      for(n=0; n<4; n++){ //son bir test daha var
        for(m=0; m<4; m++){ //eger portalin 4 koseside duvar uzeirnde mi diye kontrol ediyoruz
		  v1[0] = walls[i]->points[(m+1)%4][0] - walls[i]->points[m][0];
		  v1[1] = walls[i]->points[(m+1)%4][1] - walls[i]->points[m][1];
		  v1[2] = walls[i]->points[(m+1)%4][2] - walls[i]->points[m][2];

		  v2[0] = points[n][0] - walls[i]->points[m][0];
		  v2[1] = points[n][1] - walls[i]->points[m][1];
		  v2[2] = points[n][2] - walls[i]->points[m][2];

		  crossProduct(v1, v2, vr);

            
		  if(vr[0]*walls[i]->normal[0] + vr[1]*walls[i]->normal[1] + vr[2]*walls[i]->normal[2] < 0) disabled = 1;

        }
      }


      Portal *p, *p2;
      if(bt == 0){
        p = portals[0];
        p2 = portals[1];
      }else{
        p = portals[1];
        p2 = portals[0];
      }
      if(walls[i] == p2->onThis){ //bir baska testimizde iki portal da ayni duvarda ise
        for(n=0; n<4; n++){ //iki portal ust uste biniyor mu diye bakiyoruz
          int t = 0;
          for(int m=0; m<4; m++){
            v1[0] = p2->points[(m+1)%4][0] - p2->points[m][0];
            v1[1] = p2->points[(m+1)%4][1] - p2->points[m][1];
		    v1[2] = p2->points[(m+1)%4][2] - p2->points[m][2];

		    v2[0] = points[n][0] - p2->points[m][0];
		    v2[1] = points[n][1] - p2->points[m][1];
		    v2[2] = points[n][2] - p2->points[m][2];

		    crossProduct(v1, v2, vr);

            
		    if(vr[0]*walls[i]->normal[0] + vr[1]*walls[i]->normal[1] + vr[2]*walls[i]->normal[2] >= 0) t++;

          }
          if(t == 4) disabled = 1;
        }

        for(n=0; n<4; n++){
          int t = 0;
          for(m=0; m<4; m++){
		    v1[0] = points[(m+1)%4][0] - points[m][0];
		    v1[1] = points[(m+1)%4][1] - points[m][1];
		    v1[2] = points[(m+1)%4][2] - points[m][2];

		    v2[0] = p2->points[n][0] - points[m][0];
		    v2[1] = p2->points[n][1] - points[m][1];
		    v2[2] = p2->points[n][2] - points[m][2];

		    crossProduct(v1, v2, vr);
		        
		    if(vr[0]*walls[i]->normal[0] + vr[1]*walls[i]->normal[1] + vr[2]*walls[i]->normal[2] >= 0) t++;

          }
          if(t == 4) disabled = 1;
        }
      }

      if(disabled == 1 || state != 1) return;
      
      for(int j=0; j<3; j++){ //eger her sey duzgun ise portalin yerini degistir
        p->lookX[j] = lookX1[j];
        p->lookY[j] = lookY1[j];
        p->lookZ[j] = walls[i]->normal[j];
        p->coor[j] = lookN1[j];
      }
        
      p->setMatr(); 
      p->onThis->hasPortal = 0;
      p->onThis = walls[i];
      p->onThis->hasPortal = 1;
      break;
    }
  }
}


int main(int argc, char *argv[] ){
  int    ok;             
  int    running;
  glfwInit();

  
  ok = glfwOpenWindow(
  480, 480,          // Width and height of window
        8, 8, 8,           // Number of red, green, and blue bits for color buffer
        8,                 // Number of bits for alpha buffer
        24,                // Number of bits for depth buffer (Z-buffer)
        8,                 // Number of bits for stencil buffer
        GLFW_WINDOW        // We want a desktop window (could be GLFW_FULLSCREEN)
  );
  glViewport( 0, 0, 480, 480);
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();
  glFrustum( -0.1, 0.1, -0.1, 0.1, 0.1, 4000.0 );
  glMatrixMode( GL_MODELVIEW );
  if( !ok ){
    glfwTerminate();
    return 0;
  }

  glfwSetWindowTitle("Now you are thinking with Portals");

  glfwEnable( GLFW_STICKY_KEYS );
    

  glClearColor(1,1,1,1);
    
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);


  glEnable(GL_LIGHT0);
  glEnable(GL_NORMALIZE);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);

  
  const GLfloat light_ambient[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
  const GLfloat light_diffuse[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
  const GLfloat light_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
  const GLfloat light_position[] = { .0f, 300.0f, 300.0f, 1.0f };

  glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);

  glEnable(GL_CULL_FACE);
  glCullFace( GL_BACK );

  glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );
  glEnable ( GL_COLOR_MATERIAL ) ;
	
  init();
  glfwDisable( GLFW_MOUSE_CURSOR );
  glfwSetMousePos(240, 240);

  do{
    display();
    glfwSwapBuffers();

    running = !glfwGetKey( GLFW_KEY_ESC ) &&
                  glfwGetWindowParam( GLFW_OPENED );
  }while( running );

  glfwTerminate();

  return 0;
}

