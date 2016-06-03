/*****************************************************************************************
// MAE574 Virtual Reality Application and Research              Hw#1    by DARUI ZHANG 2/3 2012

Viewing
1 Rotate viewing by keep pressing left  mouse button and moving mouse.
2 Zoom in and out by keep pressing  middle mouse button and moving mouse.
3 Pan viwing need to right click select Pan in the menue,then keep pressing left  mouse button and moving mouse.

Menue
1.Rotate viewing mode
2.Zoom in/out mode
3.Pan viewing mode
4.Front View
5.Side View
6.Top View
7.Select Red Cubes
8.Select Pink Cubes
9.Select Green Balls
10.Select Blue Balls
11.Enable/Disable Ambient Light
12.Enable/Disable Positioned Light
13.Enable/Disable Directed Light

Keyboard function
"a"vewing point move to the right ;   "d" vewing point move to the left
"w"vewing point move to the forward;  "s" vewing point move to the backward
"q"vewing point move to the up     ;  "e" vewing point move to the down
/******************************************************************************************/


#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "glut.h"
#define PI 3.1428571428

using namespace std;

bool depth = false;

const float _GRND_SIZE = 30;
int flag=0;
struct vect{
double x;
double y;
double z;
};

int storemousX=360; int storemousY=0;//need rename
int MouseUpX=0;int MouseUpY=0;
int MouseDownX=0;int MouseDownY=0;
int accumPosX=0;
int accumPosY=0;

int RotatFlag=0;
int ViewFlag=0;

bool LitAmbFlag=true;
bool LitPosFlag=true;
bool LitDrctFlag=true;

float CamCurrentPosX;
float CamCurrentPosY;
float CamCurrentPosZ;
vect cameraPos;

double cameraDistance;
bool flagCameraInMotion = false;
// Sub-menu handle           
int menu;
//int red, green, blue;
// mouse position and button status
int mouseX, mouseY;
int mouseButton;

// callback when a keyboard key is pressed
void keySelect(unsigned char key, int x, int y);

// callback when the right mouse button is pressed to select a menu item
void menuSelect(int value);

// update position of virtual camera
void updateCameraPosition();

// mouse click callback
void mouseClick(int button, int state, int x, int y);

// mouse move callback
void mouseMove(int x, int y);
vect pos;
void updateCameraPosition()
{ //null
}

void mouseClick(int button, int state, int x, int y)
{
     // mouse button down
    if (state == GLUT_DOWN)
    {
        flagCameraInMotion = true;
        mouseX = x;
        mouseY = y;
        mouseButton = button;
		MouseDownX=x;
		MouseDownY=y;
	//	cout<<"x"<<" "<<MouseDownX<<"     "<<"x"<<" "<<MouseDownY<<endl;
		 
    }

    // mouse button up
    else if (state == GLUT_UP)
    {
		
        flagCameraInMotion = false;
		MouseUpX=x;
		MouseUpY=y;
		storemousX=accumPosX;
		storemousY=accumPosY;
		
    }
}

void mouseMove(int x, int y)
{  float mouseSensity=0.2;
	   
    if (flagCameraInMotion)
    {
        if (mouseButton == GLUT_RIGHT_BUTTON) 
        {
         //   cameraDistance = cameraDistance - 1.0 * (y - mouseY);
		
        }
        if (mouseButton == GLUT_MIDDLE_BUTTON)
        {
	
			RotatFlag=2;

            cameraDistance = cameraDistance - 1.0 * (y - mouseY);
			pos.x=cameraDistance*sin(mouseSensity*(storemousX)*PI/180);
			pos.y=cameraDistance*cos(mouseSensity*(storemousX)*PI/180);
			pos.z=cameraDistance*cos(mouseSensity*(storemousY)*PI/180);
			RotatFlag=0;
        }

        else if (mouseButton == GLUT_LEFT_BUTTON)
        {	

			if (RotatFlag==0){
			//rotate
			
			accumPosX=accumPosX+mouseSensity*(mouseX-MouseDownX);
			accumPosY=accumPosY+mouseSensity*(mouseY-MouseDownY);

			if((mouseSensity*accumPosX>360)||(mouseSensity*accumPosX<-360)){
			accumPosX=0;
			}
			if((mouseSensity*accumPosY>360)||(mouseSensity*accumPosY<-360)){
			accumPosY=0;
			}

			pos.x=cameraDistance*sin(mouseSensity*accumPosX*PI/180);
			pos.y=cameraDistance*cos(mouseSensity*accumPosX*PI/180);
			pos.z=cameraDistance*cos(mouseSensity*accumPosY*PI/180);


			}
			
			else if(RotatFlag==1){
			//pan


			pos.y=0+x/10;
			pos.z=1+y/10;
			pos.x=50;

			cameraPos.y=0+x/10;
			cameraPos.z=0+y/10; 
			}
			else{
			cameraDistance = cameraDistance - 1.0 * (y - mouseY);
			pos.x=cameraDistance*sin(mouseSensity*(storemousX)*PI/180);
			pos.y=cameraDistance*cos(mouseSensity*(storemousX)*PI/180);
			pos.z=cameraDistance*cos(mouseSensity*(storemousY)*PI/180);
			}
			
        }
    }

    updateCameraPosition();


  mouseX = x;
  mouseY = y;

}

void keys( unsigned char key, int x, int y )
{
 switch( key )
 {


 case 'w' :
  {

   pos.x = pos.x-1;
   
  } break;
 case 's' :
  {

    pos.x =  pos.x+1;

  } break;

 case 'a' :
  {

   pos.y =  pos.y-1;

  } break;

 case 'd' :
  {

   pos.y =  pos.y+1;

  } break;
   case 'q' :
  {

   pos.z =  pos.z-1;

  } break;

 case 'e' :
  {

   pos.z =  pos.z+1;

  } break;

 }
 glutPostRedisplay();
}


void reshape( int w, int h )
{
 glViewport( 0, 0, (GLsizei)w, (GLsizei)h );
 glMatrixMode( GL_PROJECTION );
 glLoadIdentity();
 glFrustum( -.4, .4, -.3, .3, 1., 200. );
 glMatrixMode( GL_MODELVIEW );
 glLoadIdentity();
 glutPostRedisplay();
}

void idle( void )
{
	
	glutPostRedisplay();
}


GLfloat white_light_diff[] = { 1., 1., 1., 1. };
GLfloat white_light_amb[] = { .2, .2, .2, 1. };
GLfloat light_posn[] = { 0., 0., 1., 0. };

// green ball (solid)
GLfloat mat_diffuse0[] = { 0., 0.6, 0.2, 1.0 };
GLfloat mat_ambient0[] = { 0., 0.6, .2, 1.0 };
GLfloat mat_specular0[]= {0., 0.0, .0, 0.0};
GLfloat mat_shininess0[]={0};

// Blue ball (solid)
GLfloat mat_diffuse2[] = { 0.0, 0.4, 0.8, 1.0 };
GLfloat mat_ambient2[] = { 0.0, 0.1, .3, 1.0 };
GLfloat mat_specular2[]= {0., 0.0, .0, 0.0};
GLfloat mat_shininess2[]={0};

// Purple cube
GLfloat mat_diffuse3[] = { 0.3, 0.2, 0.6, 0.5 };
GLfloat mat_ambient3[] = { 0.3, 0.2, 0.6, 0.5 };
GLfloat mat_specular3[]= {0., 0.0, .0, 0.0};
GLfloat mat_shininess3[]={0};

// transparent
GLfloat mat_diffuse1[] = { 0.6, 0.1, 0.1, 0.5 };
GLfloat mat_ambient1[] = { 0.1, 0., 0.0, 1.0 };
GLfloat mat_specular1[]= {0., 0.0, .0, 0.0};
GLfloat mat_shininess1[]={0};

//ground
GLfloat mat_diffuse4[] = { 0.3, 0.3, 0.6, 0.5 };
GLfloat mat_ambient4[] = { 0.3, 0.3, 0.6, 0.5 };

void drawAxes()
{
	glBegin( GL_LINES );
		glColor3f( 1., 0., 0. );
		glVertex3f( -350., 0., 0. );
		glVertex3f( 350., 0., 0. );
	glEnd();

	glBegin( GL_LINES );
		glColor3f( 0., 1., 0. );
		glVertex3f( 0., -100., 0. );
		glVertex3f( 0., 100., 0. );
	glEnd();

	glBegin( GL_LINES );
		glColor3f( 0., 0., 1. );
		glVertex3f( 0., 0., -350. );
		glVertex3f( 0., 0., 350. );
	glEnd();
}



void drawGround(){
	 glPushAttrib( GL_LIGHTING );
	 glEnable( GL_LIGHTING );
	 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient4);
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse4);

	glBegin(GL_QUADS); //Begin quadrilateral coordinates
	
	//Trapezoid   
	glVertex3f(-_GRND_SIZE /2, -_GRND_SIZE /2, 0);
	glVertex3f( _GRND_SIZE /2, -_GRND_SIZE /2, 0);
	glVertex3f(_GRND_SIZE /2, _GRND_SIZE /2,  0);
	glVertex3f(-_GRND_SIZE /2, _GRND_SIZE /2, 0);
	
	

	glEnd(); //End quadrilateral coordinates
	glPopAttrib();
}

void drawRedCubes(){
	//transparent red cubes (notice how the toggling of the depth mask affects
 // final rendering)
    float x=0;
	float y=2;
	float z=0;
 //glPushAttrib( GL_LIGHTING );
 //glEnable( GL_LIGHTING );
 //glEnable( GL_BLEND );
 //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 //if( depth )
 //glDepthMask( GL_FALSE );
 //glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
 //glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);

  glPushAttrib( GL_LIGHTING );
	 glEnable( GL_LIGHTING );
	 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient1);
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse1);
	 glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular1);
	 glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess1);


 for( unsigned int i=0; i<3; i++)
 {
  glPushMatrix();
  glTranslatef( x+11-float(i*5) ,y+0+float(i*5),z+11-float(i*5) );
  glutSolidCube( 2.0 ); //cubes righthand side
  glPopMatrix();

  glPushMatrix();
  glTranslatef( x+11-float(i*5) , y+0-float(i*5), z+11-float(i*5) );
  glutSolidCube( 2.0 ); //cubes lefthand side
  glPopMatrix();

 }
 glPopAttrib(  );

 //glDisable( GL_LIGHTING );
glDisable( GL_BLEND );
	}

void drawGreenBalls(){
	float x=-4;
	float y=-8; //start point at the left end 
	float z=0;
		
	 glPushAttrib( GL_LIGHTING );
	 glEnable( GL_LIGHTING );
	 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient0);
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse0);
	 glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular0);
	 glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess0);
	 for( unsigned int m=0; m<4; m++){
			for( unsigned int i=0; i<4-m; i++)
		 {
		  glPushMatrix();
		  glTranslatef( x+0, y+-4.5+float(m*1.5)+float(i*3), z+1.2+float(m*2.5981) );
		  glutSolidSphere( 1.2, 20, 20 );
		  glPopMatrix();
		 }
 
	 }
	  glPopAttrib();

	}


// solid blue balls
void drawBlueBalls(){

	float x=0;
	float y=2;
	float z=0;

 glPushAttrib( GL_LIGHTING );
 glEnable( GL_LIGHTING );
 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient2);
 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse2);
 
 for( unsigned int i=0; i<2; i++)
 {
  glPushMatrix();
  glTranslatef( x+8.5-float(i*5), y+2.5+float(5*i), z+8.5-float(5*i) );
  glutSolidSphere( 1.2, 20, 20 );
  glPopMatrix();

  glPushMatrix();
  glTranslatef( x+8.5-float(i*5), y-2.5-float(5*i), z+8.5-float(5*i) );
  glutSolidSphere( 1.2, 20, 20 );
  glPopMatrix();

 }
 //glDisable( GL_LIGHTING );
 //glDisable( GL_BLEND );
  glPopAttrib();
 
}

void drawPinkCubes(){
	float x=-4;
	float y=4;
	float z=0;
	 glPushAttrib( GL_LIGHTING );
	 glEnable( GL_LIGHTING );
	 glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient3);
	 glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse3);


	 for( unsigned int m=0; m<4; m++){
			for( unsigned int i=0; i<4; i++)
		 {
		  glPushMatrix();
		  
		  if (((m==1)&&(i==1))||((m==1)&&(i==2))||((m==2)&&(i==1))||((m==2)&&(i==2))){
			  glTranslatef( x+2.5, y-3.75+float(i*2.5), z+1.0+float(m*2.5) );}
		  else{
			  glTranslatef( x+0, y-3.75+float(i*2.5), z+1.0+float(m*2.5) );
		  }

		  glutSolidCube( 2.0 );

		  glPopMatrix();
		 }
 
	 }

	 glPopAttrib();
  //glDisable( GL_LIGHTING );
  //glDisable( GL_BLEND );
	}


void draw( )	
{
 glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
 glLoadIdentity();



	  switch( ViewFlag )
	{
			 case 0: //free view
		 {
			 gluLookAt( pos.x, pos.y, pos.z, cameraPos.x, cameraPos.y, cameraPos.z, 0., 0., 1.0 );//free view

		 } break;

		   case 1: //Front view
		 {
			gluLookAt( 60.0, 0.0, 0.0, 0., 0., 0., 0., 0., 1.0 ); // y-z plane view
			//accumPosX=0;       //you can update tomorrow
			//accumPosY=0;
			//gluLookAt( pos.x, pos.y, pos.z, cameraPos.x, cameraPos.y, cameraPos.z, 0., 0., 1.0 );
			//ViewFlag=0;
		 } break;

		  case 2: //Side view
		 {
			gluLookAt( 0.0, 60.0, 0.0, 0., 0., 0., 0., 0., 1.0 ); // x-z plane view
		 } break;
		  case 3: //Top view
		 {
			gluLookAt( 0.0, 0.0, 60.0, 0., 0., 0., -1., 0., 0.0 ); // y-x plane view
		 } break;
	}
  
 // Draw the coordinate axes.
	glDisable( GL_LIGHTING ); // disable enable lighting,axes does not affect by light
	drawAxes();
	glEnable( GL_LIGHTING );

	drawGround();



 drawRedCubes();
 drawGreenBalls();
 drawBlueBalls();
 drawPinkCubes();

 // glutSolidCube( 2.0 ); //test light
	
// bool LitAmbFlag=true;
//bool LitPosFlag=true;
//bool LitDrctFlag=true;

	// //Add ambient light

	glEnable( GL_LIGHTING );
	GLfloat ambientColor[] = {0.3f, 0.0f, 0.0f, 1.0f}; 
	GLfloat ambientColor2[] = {0.0f, 0.0f, 0.0f, 1.0f};
	if(LitAmbFlag==true){
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	}
	else{
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor2);
	}

	//Add positioned light
	GLfloat lightColor0[] = {0.8f, 0.8f, 0.8f, 1.0f}; 
	GLfloat lightPos0[] = {0.0f, 0.0f, 15.0f, 1.0f}; 
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	if( LitPosFlag==true){
	glEnable( GL_LIGHT0 );
	}
	else{
	glDisable( GL_LIGHT0 );
	}

	//Add directed light
	GLfloat lightColor1[] = {0.9f, 0.9f, 0.9f, 1.0f}; 
	GLfloat lightPos1[] = {1.0f, 1.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	if( LitDrctFlag==true){
	glEnable( GL_LIGHT1 );
	}
	else{
	glDisable( GL_LIGHT1);
	}
	
 if( depth )
  glDepthMask( GL_TRUE );

 glPopAttrib();

 glutSwapBuffers();
}

void resetColor(){

//" Red Cubes"
		mat_ambient1[0]=0.6 ; mat_ambient1[1]=0.1 ; mat_ambient1[2]=0.1;
	    mat_diffuse1[0]=0.1 ; mat_diffuse1[1]=0.0 ; mat_diffuse1[2]=0.0;
		mat_specular1[0]=0.0; mat_specular1[1]=0.0; mat_specular1[2]=0.0;
		mat_shininess1[0]=0;
	
		//" Purple Cubes"
		mat_ambient3[0]=0.3 ; mat_ambient3[1]=0.2 ; mat_ambient3[2]=0.6;
		mat_diffuse3[0]=0.3 ; mat_diffuse3[1]=0.2 ; mat_diffuse3[2]=0.6;
		mat_specular3[0]=0.0; mat_specular3[1]=0.0; mat_specular3[2]=0.0;
		mat_shininess3[0]=0;
		
		//" Green Balls"
		mat_ambient0[0]=0.0 ; mat_ambient0[1]=0.6 ; mat_ambient0[2]=0.2;
		mat_diffuse0[0]=0.0 ; mat_diffuse0[1]=0.6 ; mat_diffuse0[2]=0.2 ;
		mat_specular0[0]=0.0; mat_specular0[1]=0.0; mat_specular0[2]=0.0;
		mat_shininess0[0]=0;

		//" Blue Balls"
		mat_ambient2[0]=0.0 ; mat_ambient2[1]=0.4 ; mat_ambient2[2]=0.8;
		mat_diffuse2[0]=0.0 ; mat_diffuse2[1]=0.1 ; mat_diffuse2[2]=0.3;
		mat_specular2[0]=0.0; mat_specular2[1]=0.0; mat_specular2[2]=0.0;
		mat_shininess2[0]=0;
}
void init( )
{
    cameraPos.x = 0;
	cameraPos.y = 0;
	cameraPos.z = 0;
    cameraDistance = 50;

 glClearColor( .46, .53, .6, 1. );

 // Enable depth testing
 glEnable( GL_DEPTH_TEST );


 glLightfv( GL_LIGHT0, GL_POSITION, light_posn );
 glLightfv( GL_LIGHT0, GL_DIFFUSE, white_light_diff );
 

}

void processMenuEvents(int option) {
	
	switch (option) {
		case 1 : //"Rotate"
			RotatFlag=0;
			ViewFlag=0;
			cameraPos.x = 0;cameraPos.y = 0;cameraPos.z = 0;
			break;
		case 2 : //"zoom in out"
			 RotatFlag=2;
			break;
		case 3 : //"Pan"
			RotatFlag=1; CamCurrentPosX=pos.x; CamCurrentPosY=pos.y; CamCurrentPosZ=pos.z;
			break;
		case 4 : //Front View
			
			ViewFlag=1;
			break;
		case 5 : //Side View
			ViewFlag=2;
			break;
		case 6 : //Top View
			ViewFlag=3;
			break;
		case 7 ://"Select Red Cubes"
		resetColor();
		mat_ambient1[0]=0.24725 ; mat_ambient1[1]=0.1995 ; mat_ambient1[2]=0.0745;
	    mat_diffuse1[0]=0.75164 ; mat_diffuse1[1]=0.60648 ; mat_diffuse1[2]=0.22648;
		mat_specular1[0]=0.628281; mat_specular1[1]=0.555802; mat_specular1[2]=0.366065;
		mat_shininess1[0]=51.2;
		break;
		case 8 ://"Select Pink Cubes"
		resetColor();
		mat_ambient3[0]=0.24725 ; mat_ambient3[1]=0.1995 ; mat_ambient3[2]=0.0745;
		mat_diffuse3[0]=0.75164 ; mat_diffuse3[1]=0.60648 ; mat_diffuse3[2]=0.22648;
		mat_specular3[0]=0.628281; mat_specular3[1]=0.555802; mat_specular3[2]=0.366065;
		mat_shininess3[0]=51.2;
		break;
		case 9 ://"Select Green Balls"
		resetColor();
		mat_ambient0[0]=0.24725 ; mat_ambient0[1]=0.1995 ; mat_ambient0[2]=0.0745;
		mat_diffuse0[0]=0.75164 ; mat_diffuse0[1]=0.60648 ; mat_diffuse0[2]=0.22648;
		mat_specular0[0]=0.628281; mat_specular0[1]=0.555802; mat_specular0[2]=0.366065;
		mat_shininess0[0]=51.2;
		break;
		case 10 ://"Select Blue Balls"
		resetColor();
		mat_ambient2[0]=0.24725 ; mat_ambient2[1]=0.1995 ; mat_ambient2[2]=0.0745;
		mat_diffuse2[0]=0.75164 ; mat_diffuse2[1]=0.60648 ; mat_diffuse2[2]=0.22648;
		mat_specular2[0]=0.628281; mat_specular2[1]=0.555802; mat_specular2[2]=0.366065;
		mat_shininess2[0]=51.2;
		break;
		case 11 ://"Enable/Disable Ambient Light"
		LitAmbFlag=!LitAmbFlag;
		break;
		case 12://"Enable/Disable Positioned Light"
		LitPosFlag=!LitPosFlag;
		break;
		case 13 ://"Enable/Disable Directed Light"
		LitDrctFlag=!LitDrctFlag;
		break;
	
	}
}
int main(int argc, char* argv[])
{
 glutInit( &argc, argv );
 glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
 glutInitWindowPosition( 50, 50 );
 glutInitWindowSize( 640, 480 );
 glutCreateWindow( "VR is Awesom" );
 glutReshapeFunc( reshape );
 init();
 glutDisplayFunc( draw );
 glutKeyboardFunc( keys );
 
glutIdleFunc( idle );
glutMouseFunc( mouseClick );
glutMotionFunc( mouseMove );

// creates a menu  
    menu = glutCreateMenu(processMenuEvents);
	glutAddMenuEntry("Rotate",1);
	glutAddMenuEntry("Zoom in/out",2);
	glutAddMenuEntry("Pan",3); 
	glutAddMenuEntry("Front View",4);
	glutAddMenuEntry("Side View",5);
	glutAddMenuEntry("Top View",6);
	glutAddMenuEntry("Select Red Cubes",7);
	glutAddMenuEntry("Select Purple Cubes",8);
	glutAddMenuEntry("Select Green Balls",9);
	glutAddMenuEntry("Select Blue Balls",10);
	glutAddMenuEntry("Enable/Disable Ambient Light",11);
	glutAddMenuEntry("Enable/Disable Positioned Light",12);
	glutAddMenuEntry("Enable/Disable Directed Light",13);
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);

 glutMainLoop();
 glutMainLoop();

 return 0;
}

 