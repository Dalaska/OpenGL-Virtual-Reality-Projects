//*****************************************************************************************
// Disclaimer, Authorship and License
/*
	This code has been written to serve as a learning tool for the course MAE 574, 
	Virtual Reality Applications and Research, Spring 2010.
	
	Large parts of the code has been adapted from the examples given in the OpenGL 
	Programming Guide by Woo et. al and also from Computer Graphics Using OpenGL by FS Hill
	Some parts of the code has also been adapted from various resources available through 
	out the internet.I thank all those whose code and resources I have used to write these 
	examples. You may use this code for any non-commerical purpose. Feel free to include 
	and use parts of this code for your own application, but remember that this code is not
	entirely bug free, use it at your own risk..If you plan on using the code for your any 
	academic purpose please drop me an email. I would like to attach a link to your course 
	from my home page.

	The Author of this code is Ashirwad J Chowriappa, Dept. of Mech and Aero Eng. ,
	University at Buffalo.
*/
//******************************************************************************************

// This incomplete code is supposed to generate an optimized mesh based on a height map.
// Clicking and dragging the mouse turns the camera.  Getting this to work properly was
// probably the greatest accomplishment of this program, which isn't saying much.  W moves
// forward, S moves back, A moves left, and D moves right.  1 toggles textures, 2 toggles
// wireframe mode, 3 toggles normals, and 4 toggles the mesh optimization algorithm.

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <string.h>
#include "glut.h"
#include "tga.h"


using namespace std;
#define M_PI 3.14857
bool keyL = false, keyR = false, keyF = false, keyB = false;
bool useTextures = true, useWireframe = false, useNormals = true, quickGrid = false;
float cameraX = -100, cameraY = -100, cameraZ = 300;
float lookX = 0, lookY = 0, lookZ = 0;
float lastX = 0, lastY = 0;
float phi = 1.0, theta = 2.5;
static const float hrate = 0.003, vrate = 0.003, cameraTurnSpeed = 0.05, cameraMoveSpeed = 4.0;
int polyCount;
unsigned char* GrayScaleData;
float* normals;
int* contour;
int* t1, t2, t3;
float relief = 0.1, tolerance = 0.1;

TextureImage tex;
TextureImage textureMap;


GLuint aShape;
GLuint texID;

// White directional light
GLfloat white_light_diff[] = { 1., 1., 1., 1. };
GLfloat white_light_amb[] = { 1., 1., 1., 1. };
GLfloat light_posn[] = { 1., 1., 1., 0. };

// material for the object
GLfloat mat_ambient[] = { 0.05, 0.05, 0.05, 1.0 };
GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
GLfloat mat_specular[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat low_shininess[] = { 15.0 };

void createList();

void keys( unsigned char key, int x, int y )
{
	switch( key )
	{
    case 'w':
    case 'W':
        keyF = true;
    break;

	case 'a':
    case 'A':
        keyL = true;
    break;

    case 's':
    case 'S':
        keyB = true;
    break;

    case 'd':
    case 'D':
        keyR = true;
    break;

    case 'f':
        relief -= 0.01;
        createList();
    break;

    case 'r':
        relief += 0.01;
        createList();
    break;

    case '1':
        if( useTextures )
        {
        	useTextures = false;
        	glDisable( GL_TEXTURE_2D );
        }
        else
        {
        	useTextures = true;
        	glEnable( GL_TEXTURE_2D );
        }
    break;

    case '2':
        if( useWireframe )
        {
        	useWireframe = false;
        	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        else
        {
        	useWireframe = true;
        	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        }
    break;

    case '3':
        if( useNormals )
        {
        	useNormals = false;
        	createList();
        }
        else
        {
        	useNormals = true;
        	createList();
        }
    break;

    case '4':
        if( quickGrid )
        {
        	quickGrid = false;
        	createList();
        }
        else
        {
        	quickGrid = true;
        	createList();
        }
    break;
	}
}

void up( unsigned char key, int x, int y )
{
	switch( key )
	{
    case 'w':
    case 'W':
        keyF = false;
    break;

	case 'a':
    case 'A':
        keyL = false;
    break;

    case 's':
    case 'S':
        keyB = false;
    break;

    case 'd':
    case 'D':
        keyR = false;
    break;
	}
}

void mouseclick( int button, int state, int x, int y )
{
    lastX = x;
    lastY = y;
}

void mousedrag( int x, int y)
{
    phi -= (lastX - x) * hrate;
    theta += (lastY - y) * vrate;
    if( theta > M_PI - 0.01 )
        theta = M_PI - 0.01;
    if( theta < 0.01 )
        theta = 0.01;
    lastX = x;
    lastY = y;
}

void idle( )
{
	//glutPostRedisplay();
}

void drawAxes()
{
	glBegin( GL_LINES );
		glColor3f( 1., 0., 0. );
		glVertex3f( -350., 0., 0. );
		glVertex3f( 350., 0., 0. );
	glEnd();

	glBegin( GL_LINES );
		glColor3f( 0., 1., 0. );
		glVertex3f( 0., -350., 0. );
		glVertex3f( 0., 350., 0. );
	glEnd();

	glBegin( GL_LINES );
		glColor3f( 0., 0., 1. );
		glVertex3f( 0., 0., -350. );
		glVertex3f( 0., 0., 350. );
	glEnd();
}

void reshape( int w, int h )
{
	glViewport( 0, 0, (GLsizei)w, (GLsizei)h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -.4, .4, -.3, .3, 1., 2000. );
	glMatrixMode( GL_MODELVIEW );
	glutPostRedisplay();
}

void draw( )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	//glColor3f( 1., 0., 0. );
	glLoadIdentity();

	//The first viewing command
	gluLookAt( cameraX, cameraY, cameraZ, lookX, lookY, lookZ, 0., 0., 1. );

    glDisable( GL_LIGHTING );
	drawAxes();
    glEnable( GL_LIGHTING );

    glLightfv( GL_LIGHT0, GL_POSITION, light_posn );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, white_light_diff );
	glLightfv( GL_LIGHT0, GL_AMBIENT, white_light_amb );

    //glTranslatef( moveX, moveY, moveZ );

	
	

	GLfloat lightColor0[] = {0.8f, 0.0f, 0.0f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {20.0f, 20.0f, 15.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	glEnable( GL_LIGHT0 );
    /*glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);*/

    glCallList( aShape );



	glutSwapBuffers();
}

void tgaGrayscale()
{

	int mode,i,j;
// compute the number of actual components
	mode = tex.bpp / 8;

// allocate an array for the new image data
	int total = 0;
	total = tex.height * tex.width;
	GrayScaleData = new unsigned char[ (total*sizeof(unsigned char)) ];

// convert pixels: greyscale = 0.30 * R + 0.59 * G + 0.11 * B
	for (i = 0,j = 0; j < tex.width * tex.height; i +=mode, j++)
		GrayScaleData[j] = (unsigned char)(0.30 * tex.imageData[i]   +
												0.59 * tex.imageData[i+1] +
												0.11 * tex.imageData[i+2]);
}

// draws a sqare using GL_TRIANGLES.  the bool determines which way the
// diagonal goes.
void drawSquare(int x1, int y1, int x2, int y2, bool diag)
{
    float z1 = GrayScaleData[y1*tex.width+x1]*relief;
    float z3 = GrayScaleData[y2*tex.width+x1]*relief;
    float z2 = GrayScaleData[y1*tex.width+x2]*relief;
    float z4 = GrayScaleData[y2*tex.width+x2]*relief;
    int q;

    glBegin( GL_TRIANGLES );

    q = (y1 * tex.width + x1) * 3;
    if( useNormals )
        glNormal3f(normals[q], normals[q+1], normals[q+2]);
    glTexCoord2f( 1.*x1/tex.width, 1.*y1/tex.height );
    glVertex3f( y1, x1, z1 );

    q = (y1 * tex.width + x2) * 3;
    if( useNormals )
        glNormal3f(normals[q], normals[q+1], normals[q+2]);
    glTexCoord2f( 1.*x2/tex.width, 1.*y1/tex.height );
    glVertex3f( y1, x2, z2 );

    // draws the triangles differently depending on which way the diagonal goes
    if( diag )
    {
        q = (y2 * tex.width + x1) * 3;
        if( useNormals )
            glNormal3f(normals[q], normals[q+1], normals[q+2]);
        glTexCoord2f( 1.*x1/tex.width, 1.*y2/tex.height );
        glVertex3f( y2, x1, z3 );

        q = (y2 * tex.width + x1) * 3;
        if( useNormals )
            glNormal3f(normals[q], normals[q+1], normals[q+2]);
        glTexCoord2f( 1.*x1/tex.width, 1.*y2/tex.height );
        glVertex3f( y2, x1, z3 );

        q = (y1 * tex.width + x2) * 3;
        if( useNormals )
            glNormal3f(normals[q], normals[q+1], normals[q+2]);
        glTexCoord2f( 1.*x2/tex.width, 1.*y1/tex.height );
        glVertex3f( y1, x2, z2 );
    }
    else
    {
        q = (y2 * tex.width + x2) * 3;
        if( useNormals )
            glNormal3f(normals[q], normals[q+1], normals[q+2]);
        glTexCoord2f( 1.*x2/tex.width, 1.*y2/tex.height );
        glVertex3f( y2, x2, z4 );

        q = (y2 * tex.width + x1) * 3;
        if( useNormals )
            glNormal3f(normals[q], normals[q+1], normals[q+2]);
        glTexCoord2f( 1.*x1/tex.width, 1.*y2/tex.height );
        glVertex3f( y2, x1, z3 );


        q = (y1 * tex.width + x1) * 3;
        if( useNormals )
            glNormal3f(normals[q], normals[q+1], normals[q+2]);
        glTexCoord2f( 1.*x1/tex.width, 1.*y1/tex.height );
        glVertex3f( y1, x1, z1 );
    }

    q = (y2 * tex.width + x2) * 3;
    if( useNormals )
        glNormal3f(normals[q], normals[q+1], normals[q+2]);
    glTexCoord2f( 1.*x2/tex.width, 1.*y2/tex.height );
    glVertex3f( y2, x2, z4 );

    glEnd();
}

void drawVertex( float x, float y, float z, float u, float v, int q )
{
    if( useNormals )
        glNormal3f(normals[q], normals[q+1], normals[q+2]);
    glTexCoord2f( u, v );
    glVertex3f( x, y, z );
}

// recursively tests an area to see if it can be represented as a flat area.  as
// you can see, this method still has dozens of bugs to be worked out yet, not the
// least of which is the gaps between squares.  the next step would be altering the
// drawSquare algorithm to check for these discontinuities and fix them.
int testSquare( int x1, int y1, int x2, int y2 )
{
    float z0, zt, dx, dy;
    float z1 = GrayScaleData[y1*tex.width+x1]*relief;
    float z2 = GrayScaleData[y2*tex.width+x1]*relief;
    float z3 = GrayScaleData[y1*tex.width+x2]*relief;
    float z4 = GrayScaleData[y2*tex.width+x2]*relief;

    // if the square is 1x1, it can't get any smaller, so draw it.
    if( x2-x1 <= 1 && y2-y1 <= 1 )
    {
        // the bool value at the end tells the drawSquare function to draw the
        // diagonal such that it goes whichever way gives it a smaller slope
        drawSquare( x1, y1, x2, y2, abs(z1-z4) > abs(z2-z3) );
        return 0;
    }

    for( int i=0; i <= y2-y1; i++ )
    {
        for( int j=0; j<= x2-x1; j++ )
        {
            // this determines how close each point is to the simplified polygon
            // and refines the drawing in that area if the point is outside the
            // allowable tolerance.  The math is for the simplified case of an
            // axis-aligned (when projected onto the x-y plane) rectangle.
            dx = 1.*j/(x2-x1);
            dy = 1.*i/(y2-y1);
            zt = z1 + dy*(z3-z1) + dx*(z2-z1);
            z0 = GrayScaleData[(y1+i)*tex.width+x1+j]*relief;
            if( abs(zt-z0) > tolerance )
                goto outOfTolerance;
        }
    }

    drawSquare( x1, y1, x2, y2, abs(z1-z4) > abs(z2-z3) );
    return 0;

    outOfTolerance:
    testSquare( x1, y1, (x1+x2)/2, (y1+y2)/2 );
    testSquare( x1, (y1+y2)/2, (x1+x2)/2, y2 );
    testSquare( (x1+x2)/2, y1, x2, (y1+y2)/2 );
    testSquare( (x1+x2)/2, (y1+y2)/2, x2, y2 );
}

void createTriStrip()
{
	int row, column, q;
	float z0, z1, z2, z3, z4, dz1, dz2, dz3, dz4;

	tgaGrayscale();

	// allocate space for the normals array
	if( useNormals )
	{
        normals = (float *)malloc( tex.height * tex.width * sizeof(float) * 3);

        for( int i=0; i<tex.height*tex.width; i++ )
        {
            row = i / tex.width;
            column = i % tex.width;

            // calculates z-values for each pixel. If a pixel is on the edge, the
            // slope between the pixel and the edge is assumed to be zero.
            z0 = GrayScaleData[i] * relief;
            z1 = row == 0 ? z0 : GrayScaleData[i-tex.width] * relief;
            z2 = column+1 == tex.width ? z0 : GrayScaleData[i+1] * relief;
            z3 = row+1 == tex.height ? z0 : GrayScaleData[i+tex.width] * relief;
            z4 = column == 0 ? z0 : GrayScaleData[i-1] * relief;

            // I calculated the cross products and simplified the math to get the
            // i, j, and k components of the unnormalized vector normal.  I set the
            // program to normalize automatically, so this works.  Even though this
            // seems extremely simple, this is because the x or y component of the
            // distance between adjacent points always has a magnitude of 1.  As a
            // result, i and j simplify to subtraction problems, and k simplifies
            // to a constant.
            normals[3*i] = z4 - z2;
            normals[3*i+1] = z1 - z3;
            normals[3*i+2] = 2.0;
        }
	}

	// as we have n grid points, we can generate only n-1 trianglestrips
    for(int i=0; i<tex.height-1; i++)
    {
        glBegin( GL_TRIANGLES );

        // This optimizes triangles so the slope of the diagonal edges is
        // minimized, for the most part.  This produces a significantly less
        // jagged mesh.
        for(int j=0; j<tex.width; j++)
        {
            dz1 = GrayScaleData[(i*tex.width)+j]*relief;
            dz2 = GrayScaleData[(i+1)*tex.width+j]*relief;
            dz3 = GrayScaleData[(i*tex.width)+j+1]*relief;
            dz4 = GrayScaleData[(i+1)*tex.width+j+1]*relief;

            q = (i * tex.width + j) * 3;

            // this draws each two-triangle square.
            if( abs(dz1-dz4) > abs(dz2-dz3) )
            {
                drawVertex( i, j, dz1, 1.*j/tex.width, 1.*i/tex.height, q );
                drawVertex( i+1, j, dz2, 1.*j/tex.width, 1.*(i+1)/tex.height, q+3*tex.width );
                drawVertex( i, j+1, dz3, 1.*(j+1)/tex.width, 1.*i/tex.height, q+3 );
                drawVertex( i, j+1, dz3, 1.*(j+1)/tex.width, 1.*i/tex.height, q+3 );
                drawVertex( i+1, j, dz2, 1.*j/tex.width, 1.*(i+1)/tex.height, q+3*tex.width );
                drawVertex( i+1, j+1, dz4, 1.*(j+1)/tex.width, 1.*(i+1)/tex.height, q+3*tex.width+3 );
            }
            else
            {
                drawVertex( i, j, dz1, 1.*j/tex.width, 1.*i/tex.height, q );
                drawVertex( i+1, j, dz2, 1.*j/tex.width, 1.*(i+1)/tex.height, q+3*tex.width );
                drawVertex( i+1, j+1, dz4, 1.*(j+1)/tex.width, 1.*(i+1)/tex.height, q+3*tex.width+3 );
                drawVertex( i, j, dz1, 1.*j/tex.width, 1.*i/tex.height, q );
                drawVertex( i+1, j+1, dz4, 1.*(j+1)/tex.width, 1.*(i+1)/tex.height, q+3*tex.width+3 );
                drawVertex( i, j+1, dz3, 1.*(j+1)/tex.width, 1.*i/tex.height, q+3 );
            }
        }
        glEnd();
    }
}

void timer( int t )
{
    if( keyR )
    {
        cameraX += cameraMoveSpeed * sinf( phi );
        cameraY -= cameraMoveSpeed * cosf( phi );
    }
    if( keyL )
    {
        cameraX -= cameraMoveSpeed * sinf( phi );
        cameraY += cameraMoveSpeed * cosf( phi );
    }
    if( keyF )
    {
        cameraX += cameraMoveSpeed * cosf( phi ) * sinf( theta );
        cameraY += cameraMoveSpeed * sinf( phi ) * sinf( theta );
        cameraZ += cameraMoveSpeed * cosf( theta );
    }
    if( keyB )
    {
        cameraX -= cameraMoveSpeed * cosf( phi ) * sinf( theta );
        cameraY -= cameraMoveSpeed * sinf( phi ) * sinf( theta );
        cameraZ -= cameraMoveSpeed * cosf( theta );
    }

    lookX = cameraX + cosf( phi ) * sinf( theta );
    lookY = cameraY + sinf( phi ) * sinf( theta );
    lookZ = cameraZ + cosf( theta );

    glutPostRedisplay();

    glutTimerFunc( 50, timer, 0 );
}

void createList()
{
	glNewList( aShape, GL_COMPILE );
        glBindTexture( GL_TEXTURE_2D, textureMap.texID );
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureMap.width, textureMap.height,
                0, GL_RGBA, GL_UNSIGNED_BYTE , textureMap.imageData );
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        if( quickGrid )
            testSquare( 0, 0, tex.width, tex.height-1 );
        else
            createTriStrip();

	glEndList();
}

void init( )
{
	glClearColor( 0., 0., 0., 0. );
	//glEnable( GL_CULL_FACE );

    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );

	glEnable( GL_DEPTH_TEST );
	glShadeModel( GL_SMOOTH );
    glEnable( GL_NORMALIZE );
    glEnable( GL_AUTO_NORMAL );

    glDepthFunc( GL_NICEST );
    glEnable( GL_TEXTURE_2D );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    LoadTGA(&tex, "terrain.tga");//height1.tga");
	
	cout << "Height map is " << tex.width << " x " <<  tex.height << "\n";
	cout << "Texture map is " << textureMap.width << " x " <<  textureMap.height << "\n";

    // maximum number of triangles in the embossed surface
    //t1 = (int *)malloc( sizeof(int) * ( tex.height * tex.width * 2 ) );
    //t2 = (int *)malloc( sizeof(int) * ( tex.height * tex.width * 2 ) );
    //t3 = (int *)malloc( sizeof(int) * ( tex.height * tex.width * 2 ) );

    // Using lists could speed up rendering time
    aShape = glGenLists(1);

    createList();

	glutTimerFunc( 100, timer, 0 );
}

int main(int argc, char* argv[])
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowPosition( 50, 50 );
	glutInitWindowSize( 800, 600 );
	glutCreateWindow( "Height maps" );

	glutDisplayFunc( draw );
	glutReshapeFunc( reshape );
	glutIdleFunc( idle );
	glutKeyboardFunc( keys );
	glutKeyboardUpFunc( up );
	//glutSpecialFunc( keys2 );
	//glutSpecialUpFunc( up2 );
	glutMouseFunc( mouseclick );
	glutMotionFunc( mousedrag );

	init( );

	glutMainLoop();

	return 0;
}

