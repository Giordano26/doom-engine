#include <math.h>
#include <stdio.h>
#include <GL/glut.h>

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

  }
}
