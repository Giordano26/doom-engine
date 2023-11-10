#define GL_SILENCE_DEPRECATION
#include <math.h>
#include <stdio.h>
#include <GLUT/glut.h>

#define res		1  // 0 -> 160x120, 1 -> 360x240, 4 -> 640 x 480
#define SW 		160 * res //width
#define SH 		120 * res //height
#define SW2		(SW/2) // half screen width
#define SH2 		(SH/2) //half screen height
#define pixelScale 	4/res //openGL scaling -> renders at res but sizes at 4x
#define GLSW 		(SW*pixelScale) //openGL screen width
#define GLSH 		(SH*pixelScale) //openGl screen height


typedef struct {

  int fr1, fr2; //frame 1 and frame 2 to create constant frame rate, prevents screen tear

} time; time T;

typedef struct {
  int w,s,a,d; 		// up , down , left , right
  int sl, sr;		//strafe left, strafe right
  int m; 		//camera up down
} keys; keys K;

//function to draw pixels at coords x and y with c rgb color pick (c)

void pixel (int x , int y , int c){
 int rgb[3];

 switch(c){
    case 0:
	    rgb[0] = 255; rgb[1] = 255; rgb[2] = 0; // Yellow
 	    break;

    case 1:
	    rgb[0] = 160; rgb[1] = 160; rgb[2] = 0; // Darker Yellow
  	  break;
  
    case 2:
      rgb[0] = 0; rgb[1] = 255; rgb[2] = 0; // Green
  	  break;

    case 3:
      rgb[0] = 0; rgb[1] = 160; rgb[2] = 0; // Darker Green
  	  break;
      
    case 4:
      rgb[0] = 0; rgb[1] = 255; rgb[2] = 255; // Cyan
  	  break;

    case 5:
      rgb[0] = 0; rgb[1] = 160; rgb[2] = 160; // Darker Cyan
  	  break;

    case 6:
      rgb[0] = 160; rgb[1] = 100; rgb[2] = 0; // Brown
  	  break;

    case 7:
      rgb[0] = 110; rgb[1] = 50; rgb[2] = 0; // Brown darker
  	  break;

    case 8:
      rgb[0] = 0; rgb[1] = 60; rgb[2] = 130; // background
  	  break;
    } 

  glColor3ub(rgb[0], rgb[1], rgb[2]);

  glBegin(GL_QUADS);

  glVertex2i(x * pixelScale, y * pixelScale);
  glVertex2i((x + 1) * pixelScale, y * pixelScale);
  glVertex2i((x + 1) * pixelScale, (y + 1) * pixelScale);
  glVertex2i(x * pixelScale, (y + 1) * pixelScale);

  glEnd();
}



  void movePlayer(){

    if(K.a == 1 && K.m == 0){ printf("left\n"); }
    if(K.d == 1 && K.m == 0){ printf("right\n"); }
    if(K.w == 1 && K.m == 0){ printf("up\n"); }
    if(K.s == 1 && K.m == 0){ printf("down\n"); }

    if(K.sl == 1 ){ printf("strafe left\n"); }
    if(K.sr == 1 ){ printf("srafe right\n"); }

    if(K.a == 1 && K.m == 1){ printf("look up\n"); }
    if(K.d == 1 && K.m == 1){ printf("look down\n"); }
    if(K.w == 1 && K.m == 1){ printf("move up\n"); }
    if(K.s == 1 && K.m == 1){ printf("move down\n"); }
    

  }


  void clearBg(){

    int x,y;

    for(y = 0; y < SH; y++){
      for(x = 0; x < SW; x++){
        pixel(x,y,8); //clear with bg color
      }
    }
  }

  int tick;

  void draw3D(){
    int x,y, c = 0;

    for(y = 0 ; y < SH2; y++){
      for( x = 0 ; x < SW2; x++){
        pixel(x,y,c);
        c += 1;
        if(c > 8) { c = 0; }
      }
    }


    tick += 1; 
    if(tick > 20){
      tick = 0;
    }

    pixel(SW2, SH2+tick,0);  //create up movement for testing refresh rate

  }


void display(){
  int x,y;


  if(T.fr1 - T.fr2 >= 50){ //limits to 20fps

    clearBg();
    movePlayer();
    draw3D();

    T.fr2 = T.fr1;

    glutSwapBuffers();
    glutReshapeWindow(GLSW,GLSH); //prevent window scaling

  }

T.fr1 = glutGet(GLUT_ELAPSED_TIME);  //1000 miliseconds per second -> grants the 20 fps 

glutPostRedisplay();

}


void KeysDown(unsigned char key, int x, int y){

  if(key == 'w' == 1 ){ K.w = 1;}
  if(key == 'a' == 1 ){ K.a = 1;}
  if(key == 's' == 1 ){ K.s = 1;}
  if(key == 'd' == 1 ){ K.d = 1;}
  if(key == ',' == 1 ){ K.sl = 1;}
  if(key == '.' == 1 ){ K.sr = 1;}

}

void KeysUp(unsigned char key, int x, int y){

  if(key == 'w' == 0 ){ K.w = 0;}
  if(key == 'a' == 0 ){ K.a = 0;}
  if(key == 's' == 0 ){ K.s = 0;}
  if(key == 'd' == 0 ){ K.d = 0;}
  if(key == ',' == 0 ){ K.sl = 0;}
  if(key == '.' == 0 ){ K.sr = 0;}

}

void init(){
  
}

int main(int argc, char* argv[]){

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowPosition(GLSW/2, GLSH/2);
  glutInitWindowSize(GLSW,GLSH);
  glutCreateWindow("graphics_test");
  gluOrtho2D(0,GLSW,0,GLSH);
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(KeysDown);
  glutKeyboardFunc(KeysUp);
  glutMainLoop();

  return 0;


}

