#define GL_SILENCE_DEPRECATION
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
#define numSect 4
#define numWall 16


typedef struct {

  int fr1, fr2; //frame 1 and frame 2 to create constant frame rate, prevents screen tear

} time; time T;

typedef struct {
  int w,s,a,d; 		// up , down , left , right
  int sl, sr;		//strafe left, strafe right
  int m; 		//camera up down
} keys; keys K;

typedef struct{

  float cos[360];
  float sin[360];

}math; math M;


typedef struct {

  int x1, y1; //bottom line point 1
  int x2, y2; //bottom line point 2
  int c; //wall color

} walls; walls W[30];


typedef struct {

  int ws, we; //wall number start and end
  int z1,z2; //height of bottom and top
  int x,y;  //center position for sector
  int d;   //add y distance to sort drawing order
  int c1,c2; //bottom and top color
  int surf[SW]; //hold points for surfaces
  int surface; //check if there is surfaces do draw


} sectors; sectors S[30];


typedef struct {
  
  int x,y,z;  //3d spacial loc
  int a;  // angle of roation left right
  int l;  //look up and down

} player; player P;

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
    
    //left
    if(K.a == 1 && K.m == 0){ 
      P.a -= 4;
      if(P.a < 0){
        P.a += 360;
      }
     }

    //right
    if(K.d == 1 && K.m == 0){ 
      P.a += 4;
      if(P.a > 359){
        P.a -=360;
       } 
      }

    int dx = M.sin[P.a] * 10.0;
    int dy = M.cos[P.a] * 10.0;

    //foward
    if(K.w == 1 && K.m == 0){
       P.x += dx;
       P.y += dy; 
       }

    //backwards   
    if(K.s == 1 && K.m == 0){
      P.x -= dx;
      P.y -= dy;
      }

    //strafe left
    if(K.sl == 1 ){ 
      P.x -= dy;
      P.y += dx; 
      }

    //strafe right
    if(K.sr == 1 ){ 
      P.x += dy;
      P.y -= dx;     
    }

    //look up
    if(K.a == 1 && K.m == 1){ P.l -= 1; }

    //look down
    if(K.d == 1 && K.m == 1){ P.l += 1; }

    //move up
    if(K.w == 1 && K.m == 1){ P.z -= 4; }

    //move down
    if(K.s == 1 && K.m == 1){ P.z += 4; }
    

  }


  void clearBg(){

    int x,y;

    for(y = 0; y < SH; y++){
      for(x = 0; x < SW; x++){
        pixel(x,y,8); //clear with bg color
      }
    }
  }


void clipBehindPlayer(int *x1, int *y1, int *z1, int x2, int y2, int z2){ //clip line


  float da = *y1;  //distance plane to point a
  float db = y2;  //distance plane to point b
  float d = da - db; 

  if( d == 0){d = 1;}

  float s = da / (da-db); //intersection factor 

  *x1 = *x1 + s* (x2 - (*x1)); 
  *y1 = *y1 + s* (y2 - (*y1));

  if(*y1 == 0){ *y1 = 1; } //trying to prevent 0 divisions

  *z1 = *z1 + s* (z2 - (*z1));



}


void drawWall(int x1, int x2, int b1, int b2, int t1, int t2, int c, int s){

  int x,y;

  //variable to put the difference in distance

  int dyb = b2 - b1; //y distance of bottom line
  int dyt = t2 - t1; //y distance of top line
  int dx = x2 - x1;
  
  // x distance
  if ( dx == 0) {dx = 1;}

  // initial x1 pos
  int xs = x1;

  //Clipping X

  if(x1 < 1){  // clip left
    x1 = 1;
  }

  if(x2 < 1) {
    x2 = 1;  //clip left
  }

  if(x1 > SW - 1){
    x1 = SW - 1; //clip right
  }

  if(x2 > SW - 1){
    x2 = SW - 1;  //clip right
  }

  //drawing the vertical line

  for( x = x1; x < x2; x++){

    int y1 = dyb * (x - xs + 0.5) / dx + b1; //bottom point for y | 0.5 prevents rounding issues
    int y2 = dyt * (x - xs + 0.5) / dx + t1; 

    if(y1 < 1){  // clip y
      y1 = 1;
      }

    if(y2 < 1) {
      y2 = 1;  //clip y
    }

    if(y1 > SH - 1){
      y1 = SH - 1; //clip y
    }

    if(y2 > SH - 1){
      y2 = SH - 1;  //clip y
    }

    //surface
    if(S[s].surface == 1){ S[s].surf[x] = y1; continue; } //save bottom points
    if(S[s].surface == 2 ){ S[s].surf[x] = y2; continue; } //save top points
    if(S[s].surface == -1){ //bottom
      for(y = S[s].surf[x]; y < y1; y++){
        pixel(x,y,S[s].c2);
      }
    }
    if(S[s].surface == - 2){ //top
      for(y = y1; y < S[s].surf[x]; y++){
        pixel(x,y,S[s].c2);
      }
    }
    for(y = y1; y < y2; y++){
      pixel(x,y,c); //normal wall
    }
  }
}

//simple distance formula
int dist(int x1, int y1, int x2, int y2){

  int distance = sqrt((x2-x1) * (x2 - x1) + (y2-y1)*(y2-y1));
  return distance;

}

  void draw3D(){
  int s,w,loop, wall_x[4], wall_y[4], wall_z[4];
  float cs = M.cos[P.a], sn = M.sin[P.a]; //cos and sin from player rotation


  for( s = 0; s < numSect - 1; s++){
    for( w = 0; w < numSect - 1; w++){
      if(S[w].d < S[w+1].d){
        sectors st = S[w];
        S[w] = S[w+1];
        S[w+1] = st;
      }
    }
  }


  //draw sectors
  for(s = 0; s < numSect; s++){

    S[s].d = 0; //clear distance 

    if(P.z < S[s].z1){
      S[s].surface = 1; //bottom surface
    } else if (P.z > S[s].z2){
      S[s].surface = 2; //top surface
    } else{
      S[s].surface = 0; //no surface
    }


    for( loop = 0; loop < 2; loop++){

      for(w = S[s].ws; w < S[s].we; w++){

        //offset from 2 bottom points from the wall
        int x1 = W[w].x1 - P.x;
        int y1 = W[w].y1 - P.y;
        int x2 = W[w].x2 - P.x;
        int y2 = W[w].y2 - P.y;

        //swap for surface
        if(loop == 0){
          int swp = x1;
          x1 = x2;
          x2 = swp;
          swp = y1;
          y1 = y2;
          y2 = swp;
          
        }

        //World x position
        wall_x[0] = x1 * cs - y1 * sn;
        wall_x[1] = x2 * cs - y2 * sn;

        wall_x[2] = wall_x[0];
        wall_x[3] = wall_x[1]; //top line has the same x

        //World y position
        wall_y[0] = y1 * cs + x1 * sn;
        wall_y[1] = y2 * cs + x2 * sn;

        wall_y[2] = wall_y[0];
        wall_y[3] = wall_y[1]; //top line has the same y


        S[s].d += dist(0,0,(wall_x[0] + wall_x[1])/2, (wall_y[0] + wall_y[1])/2); //store wall distance

        //world z height
        //rescale the center 32 -> to keep in scale
        wall_z[0] = S[s].z1 - P.z + ((P.l * wall_y[0]) / 32.0 );
        wall_z[1] = S[s].z1 - P.z + ((P.l * wall_y[1]) / 32.0 );

        wall_z[2] = wall_z[0] + S[s].z2;
        wall_z[3] = wall_z[1] + S[s].z2; //top line has new z

        //don't draw behind player
        if(wall_y[0] < 1 && wall_y[1] < 1) { continue; }

        //point 1 behind player, needs to be clipped
        if(wall_y[0] < 1){

          clipBehindPlayer(&wall_x[0], &wall_y[0],&wall_z[0], wall_x[1],wall_y[1],wall_z[1]); //bottom line
          clipBehindPlayer(&wall_x[2], &wall_y[2],&wall_z[2], wall_x[3],wall_y[3],wall_z[3]); //top line

        }

        //point 2 behind player, needs to be clipped
        if(wall_y[1] < 1){

          //same logic for point 1 but reversed
          clipBehindPlayer(&wall_x[1], &wall_y[1],&wall_z[1], wall_x[0],wall_y[0],wall_z[0]); //bottom line
          clipBehindPlayer(&wall_x[3], &wall_y[3],&wall_z[3], wall_x[2],wall_y[2],wall_z[2]); //top line

        }

        //screen x and y, the further it is more at the center it should be
        wall_x[0] = wall_x[0] * 200 / wall_y[0] + SW2;
        wall_y[0] = wall_z[0] * 200 / wall_y[0] + SH2;

        wall_x[1] = wall_x[1] * 200 / wall_y[1] + SW2;
        wall_y[1] = wall_z[1] * 200 / wall_y[1] + SH2;

        wall_x[2] = wall_x[2] * 200 / wall_y[2] + SW2;
        wall_y[2] = wall_z[2] * 200 / wall_y[2] + SH2;

        wall_x[3] = wall_x[3] * 200 / wall_y[3] + SW2;
        wall_y[3] = wall_z[3] * 200 / wall_y[3] + SH2;

        drawWall(wall_x[0],wall_x[1],wall_y[0],wall_y[1], wall_y[2], wall_y[3], W[w].c, s);

      }

      S[s].d /= (S[s].we - S[s].ws); //finds the avg for sector distance
      S[s].surface *= -1; //flip to negative to draw surface
    }
  }
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

  if(key == 'w' ){ K.w = 1;}
  if(key == 'a' ){ K.a = 1;}
  if(key == 's' ){ K.s = 1;}
  if(key == 'd' ){ K.d = 1;}
  if(key == 'm' ){ K.m = 1;}
  if(key == ',' ){ K.sl = 1;}
  if(key == '.' ){ K.sr = 1;}

}

void KeysUp(unsigned char key, int x, int y){

  if(key == 'w' ){ K.w = 0;}
  if(key == 'a' ){ K.a = 0;}
  if(key == 's' ){ K.s = 0;}
  if(key == 'd' ){ K.d = 0;}
  if(key == 'm' ){ K.m = 0;}
  if(key == ',' ){ K.sl = 0;}
  if(key == '.' ){ K.sr = 0;}

}


int loadSectors [ ] = {
  //wall start, wall end, z1 height, z2 height
  0,4,0,40,2,3,
  4,8,0,40,4,5,
  8,12,0,40,6,7,
  12,16,0,40,0,1
};

int loadWalls [ ] = {
  0,0,32,0,0,
  32,0,32,32,1,
  32,32,0,32,0,
  0,32,0,0,1,

  64,0,96,0,2,
  96,0,96,32,3,
  96,32,64,32,2,
  64,32,64,0,3,

  64,64,96,64,4,
  96,64,96,96,5,
  96,96,64,96,4,
  64,96,64,64,5,

  0,64,32,64,6,
  32,64,32,96,7,
  32,96,0,96,6,
  0,96,0,64,7,

};

void init(){

  int x;
  //store sin/cos in dg

  for(x = 0; x < 360; x++){           
    M.cos[x] = cos(x/180.0*M_PI);
    M.sin[x] = sin(x/180.0*M_PI);

  }

  //init player
  P.x = 70;
  P.y = -110;
  P.z = 20;
  P.a = 0;
  P.l = 0;

  //load sectors

  int s,w, v1 = 0, v2 = 0;

  for (s = 0; s < numSect; s++){

    S[s].ws = loadSectors[v1+0];  // wall start number
    S[s].we = loadSectors[v1+1];  // wakk end number
    S[s].z1 = loadSectors[v1+2]; // sector bottom height
    S[s].z2 = loadSectors[v1+3]; // sector top height
    S[s].c1 = loadSectors[v1+4]; //sector top color
    S[s].c2 = loadSectors[v1+5]; //sector bottom color

    v1 += 6;

    //load walls

    for(w = S[s].ws; w < S[s].we; w++){

      W[w].x1 = loadWalls[v2+0]; //bottom x1
      W[w].y1 = loadWalls[v2+1]; //bottom y1
      W[w].x2 = loadWalls[v2+2]; //top x2
      W[w].y2 = loadWalls[v2+3]; //top y2
      W[w].c  = loadWalls[v2+4]; //wall color
      v2 += 5;


    }

  }

  
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
  glutKeyboardUpFunc(KeysUp);
  glutMainLoop();

  return 0;


}

