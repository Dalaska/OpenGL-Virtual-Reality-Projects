//*****************************************************************************************
// Disclaimer, Authorship and License
/*
	This code has been written primarily by Ashirwad Chowriappa.  Code from the Texture demo as
	well as the OBJ loader code was used in this project.  In addition, the tga.h code was
	retrieved from the internet, at http://nehe.gamedev.net.  Textures and meshes were
	primarily extracted from the video game The Elder Scrolls IV: Oblivion.
*/
//******************************************************************************************

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include "glut.h"
#include <math.h>
#include <direct.h>
#include "tga.h"

#define GL_CLAMP_TO_EDGE 0x812F

using namespace std;

typedef struct
{
    int material;
    int linked;
    float rx, ry, rz;
    float x, y, z;
    float scale;
    string meshfile;
    string texfile;
} mesh3d;

typedef struct
{
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat emission[4];
    GLfloat specular[4];
    GLfloat shininess[1];
} material;

int c = 1, q = 0; // current object
int objectCount = 0, linkedCount = 1;

GLuint aShape[30];
TextureImage textures[20];
TextureImage ltextures[10];
mesh3d mesh[20];
mesh3d lmesh[10];
material mat[10];
int setLength[30];

// declare a file reader
ifstream reader;
ifstream reader1;

// indexSet is the only global array used by the OBJ reader function.  this allows
// the makeShape function to access it.
vector<int> indexSet;
//Set absolute path to the config file and material file and obj files

string currentPath = "C:\\Users\\zdr\\Documents\\Visual Studio 2010\\Projects\\Assignment 3\\assignment 3";
string configFile =  "C:\\Users\\zdr\\Documents\\Visual Studio 2010\\Projects\\Assignment 3\\assignment 3\\config.txt";
string materialFile= "C:\\Users\\zdr\\Documents\\Visual Studio 2010\\Projects\\Assignment 3\\assignment 3\\materials.txt";
// Create values for a number of lights

// Yellow directional light
GLfloat yellow_light[] = { 1.0, 0.7, 0.2, 1. };

GLfloat green_light[] = { 0.0, 1.0, 0.0, 1. };


//white ambient light
GLfloat white_light_ambient[] = { 0.3, 0.2, 0.1, 1. };

float cameraAngle, headAngle =-0.5;
//float cameraX = 50, cameraY = 30, cameraZ = 30;
float cameraX = 64.0, cameraY = 40.3, cameraZ = 30;
float lookX = 0, lookY = 0, lookZ = 0;
static const float cameraTurnSpeed = 0.035, cameraMoveSpeed = 1.0;
static const float M_PI = 3.1415926536, M_PI_2 = M_PI / 2;

int readObjFile( string filename )
{
    vector<GLdouble> points;
    vector<GLdouble> normals;
	vector<GLdouble> texCoords;

    GLdouble*		vertArr;
    GLdouble*		normArr;
    GLdouble*       texArr;

    //vector<int> indexSet;
    vector<int> normalSet;
    vector<int> texSet;
    vector<GLdouble> centroids;

	ifstream reader2;

	string fullPath = currentPath + "\\" + filename;

	if( !filename.empty() )
		reader2.open( filename.c_str() );
	else
		return 1;

	// safety check to ensure that the file pointer opened is valid
	if (!reader2.is_open())
		return 2;

	// just blindly read in the entire file till eof in to a vector
	vector<string> Word;
	// File reading and breaking the file into white-space seperated words
	// then storing it in the container object Word
	string line;

	while(!reader2.eof())
	{
		reader2 >> line;
		Word.push_back(line);
	}
	reader2.close();

	cout << "Word.size = " << Word.size() << ", filename = " << filename.c_str() << "\n";
	// file order as follows,
	// "v" vertices of form x y z
	// "vt" texture coords of form u v (sometimes w exists, need to check for tht)
	// "vn" vertex normal, again of form x y z
	// "f" face, comprised of the following order
	//  v1[/vt1][/vn1] v2[/vt2][/vn2] v3[/vt3][/vn3] ...
	// again need to check if faces are of form triangles or quads
	for(unsigned int i = 0; i < Word.size(); i++)
	{
		if(Word[i] == "v")
		{
			points.push_back(atof(Word[i+1].c_str()));
			points.push_back(atof(Word[i+2].c_str()));
			points.push_back(atof(Word[i+3].c_str()));
		}

		if(Word[i] == "vt")
		{
			texCoords.push_back( atof(Word[i+1].c_str()) );
			texCoords.push_back( atof(Word[i+2].c_str()) );
		}

		if(Word[i] == "vn")
		{
			normals.push_back(atof(Word[i+1].c_str()));
			normals.push_back(atof(Word[i+2].c_str()));
			normals.push_back(atof(Word[i+3].c_str()));
		}

		if(Word[i] == "f")
		{
			for( int k=1; k<=3; k++)
			{
				string copy = Word[i+k];
				string store_vert;
				string store_tex;
				string store_norm;
				unsigned int j = 0;
				int count = 0;

				//  A max of three "/" can occur in one file
				while( j<copy.size() )
				{
					// get the current string of form .../.../...
					string temp;
					temp = copy[j];

					// if we encounter a /, increment the count to the
					// next thing in line
					if( temp == "/" )
						count++;
					else
					{
						// while the stuff is being done we can push the values through
						if( count == 0 )
							store_vert += copy[j];

						if( count == 1 )
							store_tex += copy[j];

						if( count == 2 )
							store_norm += copy[j];
					}
					j++;
				}

				indexSet.push_back( atof(store_vert.c_str()));
				texSet.push_back( atoi(store_tex.c_str()));
				normalSet.push_back( atoi(store_norm.c_str()));
			}
		}
	}

//	// enable the vertex arrays
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
//
	vertArr = new GLdouble[points.size()*3];
	normArr = new GLdouble[normals.size()*3];
	texArr = new GLdouble[texCoords.size()*2];

	// for obj files we have as many normals as points
	for( int i=0; i<points.size(); i++)
	{
			vertArr[i] = points[i];
			normArr[i] = normals[i];
	}

	for( int i=0; i<texCoords.size(); i++)
		texArr[i] = texCoords[i];

    // setLength is an array containing the last index value corresponding to object #c
    setLength[q] = (q>0 ? setLength[q-1] : 0 ) + normalSet.size();
	cout << "setLength[" << q << "]: " << points.size() << "\n";

    glTexCoordPointer( 2, GL_DOUBLE, 0, texArr );
	glVertexPointer( 3, GL_DOUBLE, 0, vertArr );
	glNormalPointer( GL_DOUBLE, 0, normArr );

	reader.close();
	Word.clear();

	return 0;
}

void readConfigFiles()
{
    // load configuration data
    reader.open( configFile.c_str() );

	// just blindly read in the entire file till eof in to a vector
	vector<string> Word;
	// File reading and breaking the file into white-space seperated words
	// then storing it in the container object Word
	string line;
	while(!reader.eof())
	{
		reader >> line;
		Word.push_back(line);
	}
	reader.close();

	for( int i=0; i<Word.size(); i++ )
	{
	    // load in the ordinary objects
	    if( Word[i] == "n" )
	    {
	        mesh[objectCount].linked = atoi(Word[i+1].c_str());
	        mesh[objectCount].material = atoi(Word[i+2].c_str());
	        mesh[objectCount].rx = atof(Word[i+3].c_str());
	        mesh[objectCount].ry = atof(Word[i+4].c_str());
	        mesh[objectCount].rz = atof(Word[i+5].c_str());
	        mesh[objectCount].x = atof(Word[i+6].c_str());
	        mesh[objectCount].y = atof(Word[i+7].c_str());
	        mesh[objectCount].z = atof(Word[i+8].c_str());
	        mesh[objectCount].scale = atof(Word[i+9].c_str());
	        mesh[objectCount].meshfile = Word[i+10].data();
	        mesh[objectCount].texfile = Word[i+11].data();
	        objectCount++;
	    }

	    // load in the linked objects.  these objects are physically connected to other
	    // objects but have different textures or material properties.
	    if( Word[i] == "l" )
	    {
	        lmesh[linkedCount].material = atoi(Word[i+1].c_str());
	        lmesh[linkedCount].meshfile = Word[i+2].data();
	        lmesh[linkedCount].texfile = Word[i+3].data();
	        linkedCount++;
	    }
	}

	// read in materials file
	reader1.open( materialFile.c_str() );

	// just blindly read in the entire file till eof in to a vector
	Word.clear();
	// File reading and breaking the file into white-space seperated words
	// then storing it in the container object Word
	line.clear();
	int currentMaterial = -1; // this way, this first time this is incremented, it becomes 0

	while(!reader1.eof())
	{
		reader1 >> line;
		Word.push_back(line);
	}
	reader1.close();

	for( int i=0; i<Word.size(); i++ )
	{
	    switch( Word[i].at(0) )
	    {
	        case 'a':
                currentMaterial++;
                mat[currentMaterial].ambient[0] = atof(Word[i+1].c_str());
                mat[currentMaterial].ambient[1] = atof(Word[i+2].c_str());
                mat[currentMaterial].ambient[2] = atof(Word[i+3].c_str());
                mat[currentMaterial].ambient[3] = atof(Word[i+4].c_str());
            break;

            case 'd':
                mat[currentMaterial].diffuse[0] = atof(Word[i+1].c_str());
                mat[currentMaterial].diffuse[1] = atof(Word[i+2].c_str());
                mat[currentMaterial].diffuse[2] = atof(Word[i+3].c_str());
                mat[currentMaterial].diffuse[3] = atof(Word[i+4].c_str());
            break;

            case 'e':
                mat[currentMaterial].emission[0] = atof(Word[i+1].c_str());
                mat[currentMaterial].emission[1] = atof(Word[i+2].c_str());
                mat[currentMaterial].emission[2] = atof(Word[i+3].c_str());
                mat[currentMaterial].emission[3] = atof(Word[i+4].c_str());
            break;

            case 's':
                mat[currentMaterial].specular[0] = atof(Word[i+1].c_str());
                mat[currentMaterial].specular[1] = atof(Word[i+2].c_str());
                mat[currentMaterial].specular[2] = atof(Word[i+3].c_str());
                mat[currentMaterial].specular[3] = atof(Word[i+4].c_str());
            break;

            case 'h':
                mat[currentMaterial].shininess[0] = atof(Word[i+1].c_str());
            break;
	    }
	}
}

void draw( )
{
    int a;
	// Prelims
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();

	gluLookAt( cameraX, cameraY, cameraZ, lookX, lookY, lookZ, 0., 0., 1. );

    // We keep our light within the display func so that we can influence its properties
	// Create the first white light
	//  Candle 1 Light (Big Candle)
	GLfloat light_posn[] = { mesh[5].x, mesh[5].y, mesh[5].z + 15, 1.0 };
	GLfloat light_attenuation[] = { 0.001 };
	glLightfv( GL_LIGHT0, GL_POSITION, light_posn );
	glLightfv( GL_LIGHT0, GL_AMBIENT, yellow_light );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, yellow_light );
	glLightfv( GL_LIGHT0, GL_SPECULAR, yellow_light );
    glLightfv( GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_attenuation );

	glLightfv( GL_LIGHT1, GL_AMBIENT, white_light_ambient );

	// candle2 light (small candle)
	light_posn[0] = mesh[6].x;
	light_posn[1] = mesh[6].y;
	light_posn[2] = mesh[6].z + 5;
	GLfloat light_attenuation2[] = { 0.004 };
	glLightfv( GL_LIGHT2, GL_POSITION, light_posn );
	glLightfv( GL_LIGHT2, GL_AMBIENT, yellow_light );
	glLightfv( GL_LIGHT2, GL_DIFFUSE, yellow_light );
	glLightfv( GL_LIGHT2, GL_SPECULAR, yellow_light );
    glLightfv( GL_LIGHT2, GL_QUADRATIC_ATTENUATION, light_attenuation2 );

	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 ); // large candle (large)
	glEnable( GL_LIGHT1 );  // ambient light
	glEnable( GL_LIGHT2 ); // candle 2 (small)
	
	

    for( int i=0; i<objectCount; i++ )
    {
        // set material to 0 if the object is currently selected, so
        // you can tell which object you have selected
        a = (i == c ? 0 : mesh[i].material);

        // set material
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat[a].ambient);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat[a].diffuse);
        glMaterialfv(GL_FRONT, GL_EMISSION, mat[a].emission);
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat[a].specular);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat[a].shininess);

        // do the drawing
        glPushMatrix();

        glTranslatef( mesh[i].x, mesh[i].y, mesh[i].z );

        glRotatef( mesh[i].rx, 1., 0., 0. );
        glRotatef( mesh[i].ry, 0., 1., 0. );
        glRotatef( mesh[i].rz, 0., 0., 1. );

        glScalef( mesh[i].scale, mesh[i].scale, mesh[i].scale );

        // Draw using display list
	    glCallList( aShape[i] );
	    //glCallList( aShape[objectCount] );
	    //cout << i << " is linked to " << mesh[i].linked << "\n";

	    // Draw any linked objects
	    if( mesh[i].linked != 0 )
	    {
	        // if the object is not selected, use the specified material for linked
	        // objects, otherwise use the indicator material
	        if( i != c )
	        {
                a = lmesh[mesh[i].linked].material;

                glMaterialfv(GL_FRONT, GL_AMBIENT, mat[a].ambient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, mat[a].diffuse);
                glMaterialfv(GL_FRONT, GL_EMISSION, mat[a].emission);
                glMaterialfv(GL_FRONT, GL_SPECULAR, mat[a].specular);
                glMaterialfv(GL_FRONT, GL_SHININESS, mat[a].shininess);
                if( mat[a].diffuse[3] > 0 )
                    glEnable( GL_BLEND );
	        }

            glCallList( aShape[objectCount + mesh[i].linked - 1] );
            //glutSolidCube( 10.0 );
            glDisable( GL_BLEND );
	    }
	    glPopMatrix();
    }

glDisable(GL_TEXTURE_2D);
glDisable( GL_BLEND );


glEnable(GL_BLEND);
glEnable(GL_TEXTURE_2D);

glBindTexture(GL_TEXTURE_2D, ltextures[0].texID);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//billboard


        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
		

//candle 1 Billboarded to face camera
float dx1, dy1;
float R1, phi1;
dx1 = cameraX-mesh[5].x;
dy1 = cameraY-mesh[5].y;
R1 = sqrtf(powf(dx1, 2) + powf(dy1, 2));
//phi2 = atan(dy2/dx2);
phi1 = atan2(dy1, dx1);
float fw1 = 1.5;
glBegin(GL_QUADS);
glTexCoord2f(0.0f, 0.0f); glVertex3f(mesh[5].x+ fw1*sinf(phi1), mesh[5].y - fw1*cosf(phi1), mesh[5].z +10);
glTexCoord2f(1.0f, 0.0f); glVertex3f(mesh[5].x- fw1*sinf(phi1), mesh[5].y + fw1*cosf(phi1), mesh[5].z +10);
glTexCoord2f(1.0f, 1.0f); glVertex3f(mesh[5].x- fw1*sinf(phi1), mesh[5].y + fw1*cosf(phi1), mesh[5].z + 13);
glTexCoord2f(0.0f, 1.0f); glVertex3f(mesh[5].x+ fw1*sinf(phi1), mesh[5].y - fw1*cosf(phi1), mesh[5].z + 13);
glEnd();

glDisable(GL_BLEND);
//candle 2 Billboarded to face camera
float dx2, dy2;
float R2, phi2;
dx2 = cameraX-mesh[6].x;
dy2 = cameraY-mesh[6].y;
R2 = sqrtf(powf(dx2, 2) + powf(dy2, 2));
//phi2 = atan(dy2/dx2);
phi2 = atan2(dy2, dx2);
float fw2 = 1.2;

glEnable(GL_BLEND);
glEnable(GL_TEXTURE_2D);
glBindTexture(GL_TEXTURE_2D, ltextures[0].texID);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//billboard


        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_COLOR);
glBegin(GL_QUADS);
glTexCoord2f(0.0f, 0.0f); glVertex3f(mesh[6].x+ fw2*sinf(phi2), mesh[6].y - fw2*cosf(phi2), mesh[6].z +2.5);
glTexCoord2f(1.0f, 0.0f); glVertex3f(mesh[6].x- fw2*sinf(phi2), mesh[6].y + fw2*cosf(phi2), mesh[6].z +2.5);
glTexCoord2f(1.0f, 1.0f); glVertex3f(mesh[6].x- fw2*sinf(phi2), mesh[6].y + fw2*cosf(phi2), mesh[6].z + 5);
glTexCoord2f(0.0f, 1.0f); glVertex3f(mesh[6].x+ fw2*sinf(phi2), mesh[6].y - fw2*cosf(phi2), mesh[6].z +5);
glEnd();
glDisable(GL_BLEND);
	glutSwapBuffers();
	glDisable(GL_BLEND);

	//cout << "phi " << phi2 << " Camera Angle " << cameraAngle <<"\n";
}

void keys( unsigned char key, int x, int y )
{

	switch( key )
	{
	case '1':
		mesh[c].rx += 1.;
		break;

	case '2':
		mesh[c].ry += 1.;
		break;

	case '3':
		mesh[c].rz += 1.;
		break;

	case '4':
		mesh[c].rx -= 1.;
		break;

	case '5':
		mesh[c].ry -= 1.;
		break;

	case '6':
		mesh[c].rz -= 1.;
		break;

    case 'a':
        mesh[c].x += 0.1;
        break;

    case 'd':
        mesh[c].x -= 0.1;
        break;

    case 'w':
        mesh[c].y -= 0.1;
        break;

    case 's':
        mesh[c].y += 0.1;
        break;

    case 'r':
        mesh[c].z += 0.1;
        break;

    case 'f':
        mesh[c].z -= 0.1;
        break;

    case 't':
        mesh[c].scale += 0.1;
        break;

    case 'g':
        mesh[c].scale -= 0.1;
        break;

    case 'n':
       // c++;
		c = c + 1;
		cout << " c " << c <<"\n";
        if( c > objectCount )
            c = 0;
        break;
	} 
cout << " Rx " << mesh[c].rx <<  " Ry " << mesh[c].ry  <<  " Rz " << mesh[c].rz << " x " << mesh[c].x << " y " << mesh[c].y  << " z " << mesh[c].z  << " scale " << mesh[c].scale  << " object " << mesh[c].meshfile << " texture " << mesh[c].texfile <<"\n";
cout<<"cameraX  "<<cameraX<<" cameraY "<<cameraY<<" cameraZ " <<cameraZ<<endl;
	
	glutPostRedisplay(); 
}

// manages the arrow keys used for moving the viewpoint
void keys2( int key, int x, int y )
{
    if( key == GLUT_KEY_LEFT )
        cameraAngle += cameraTurnSpeed;

    if( key == GLUT_KEY_RIGHT )
        cameraAngle -= cameraTurnSpeed;

    if( key == GLUT_KEY_UP )
    {
        cameraX += cameraMoveSpeed * cosf( cameraAngle );
        cameraY += cameraMoveSpeed * sinf( cameraAngle );
    }

    if( key == GLUT_KEY_DOWN )
    {
        cameraX -= cameraMoveSpeed * cosf( cameraAngle );
        cameraY -= cameraMoveSpeed * sinf( cameraAngle );
    }

    if( key == GLUT_KEY_HOME )
        cameraZ += cameraMoveSpeed;

    if( key == GLUT_KEY_END )
        cameraZ -= cameraMoveSpeed;

    if( key == GLUT_KEY_PAGE_UP )
    {
        headAngle += cameraTurnSpeed;
        if( headAngle >= M_PI_2 )
            headAngle = M_PI_2 - 0.01;
    }

    if( key == GLUT_KEY_PAGE_DOWN )
    {
        headAngle -= cameraTurnSpeed;
        if( headAngle <= -M_PI_2 )
            headAngle = -M_PI_2 + 0.01;
    }

    lookX = cameraX + (cosf( cameraAngle ) * cosf( headAngle ));
    lookY = cameraY + (sinf( cameraAngle ) * cosf( headAngle ));
    lookZ = cameraZ + sinf( headAngle );
    //cout << headAngle << "\n";
	//cout << "look x" << lookX << " look y " << lookY << " look z " << lookZ <<"\n";
	//cout << "cameraX " << cameraX << " Camera y " << cameraY << " camera z " << cameraZ <<"\n";
	//cout << "head angle " << headAngle << " Camera Angle " << cameraAngle <<"\n";

//begin keys revised

    draw();
}

void reshape( int w, int h )
{
	glViewport( 0, 0, (GLsizei)w, (GLsizei)h );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glFrustum( -.4, .4, -.3, .3, 1., 1300. );

	glMatrixMode( GL_MODELVIEW );

	glutPostRedisplay();
}

static void makeShape( int q )
{
    //glDrawArrays( GL_TRIANGLES, 0, indexSet.size() );
	glBegin( GL_TRIANGLES );

	// since all the connection data is stored in one array,
	for( int i = (q>0 ? setLength[q-1] : 0); i<setLength[q]; i++ )
	{
	    glArrayElement( indexSet[i]-1 );
	}
	cout << q << ": " << indexSet.size() << "\n";
	glEnd();
}

void init( )
{
	glClearColor( 0.08, 0.03, 0.01, 0. );
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable depth testing
	glEnable( GL_DEPTH_TEST );
	glClearDepth( 1.0 );
    glDepthFunc( GL_NICEST );
    glEnable( GL_TEXTURE_2D );

	// Set up a shading model
	glShadeModel( GL_SMOOTH );

	glEnable( GL_NORMALIZE );
	glEnable( GL_CULL_FACE );

    // Enable alpha blending
	glDisable( GL_BLEND );
	//glDepthMask( GL_FALSE );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// initializes the camera angles.  headAngle is the up/down angle
	cameraAngle = -2.5;//atan2f( lookY-cameraY, lookX-cameraX );
	headAngle = -0.5;//atan2f( lookZ-cameraZ, sqrtf( powf(lookX-cameraX, 2) + powf(lookY-cameraY, 2) ) );

    // read in the configuration and material data files
    readConfigFiles();

    //objectCount = 1;

	cout << objectCount << " objects loaded.\n";

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    for( int i=0; i < objectCount; i++ )
    {
        c = i;

        // read in a mesh file
		q = c;
        readObjFile( mesh[i].meshfile );

        // Read in info from the TGA image now
        LoadTGA(&textures[i], mesh[i].texfile.c_str());
        aShape[i] = glGenLists(1);

        glNewList( aShape[i], GL_COMPILE );
            glBindTexture(GL_TEXTURE_2D, textures[i].texID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[i].width, textures[i].height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE , textures[i].imageData );
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            makeShape( c );
        glEndList();
    }

    // loading the flame texture into memory, for use with the candles
    LoadTGA(&ltextures[0], "flame.tga");

    // this creates all the linked objects
    for( int j=1; j < linkedCount; j++ )
    {
        //cout << lmesh[j].meshfile.c_str() << "\n";
        //cout << "i=" << i << " j=" << j << " linkedCount=" << linkedCount << "\n";
		q = c + j;
        readObjFile( lmesh[j].meshfile );

        // Read in info from the TGA image now
        LoadTGA(&ltextures[j], lmesh[j].texfile.c_str());
        aShape[c+j] = glGenLists(1);

        glNewList( aShape[c+j], GL_COMPILE );
            glBindTexture(GL_TEXTURE_2D, ltextures[j].texID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ltextures[j].width, ltextures[j].height,
                        0, GL_RGBA, GL_UNSIGNED_BYTE , ltextures[j].imageData );
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            makeShape( c+j );
        glEndList();
    }
}

int main(int argc, char* argv[])
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE );
	glutInitWindowPosition( 50, 50 );
	glutInitWindowSize( 800, 600 );
	glutCreateWindow( "Still Rendering" );

	glutDisplayFunc( draw );
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keys );
	glutSpecialFunc( keys2 );

    // set the path to the folder of the executable, so it has an easier time finding data files
    currentPath = "C:\\Users\\zdr\\Documents\\Visual Studio 2010\\Projects\\Assignment 3\\assignment 3";
	// currentPath = currentPath.substr(0, currentPath.rfind("\\"));
	//configFile = currentPath + "\\" + "config.txt";
	//materialFile = currentPath + "\\" + "materials.txt";

    _chdir( currentPath.c_str() );
	cout << currentPath.c_str() << "\n";

	init();

	glutMainLoop();

	return 0;
}
