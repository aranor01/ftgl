#ifdef __APPLE_CC__
	#include <OpenGL/glut.h>
#else
	#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <stdio.h>

#include "tb.h"

#include "FTGLExtrdFont.h"
#include "FTGLOutlineFont.h"
#include "FTGLPolygonFont.h"
#include "FTGLTextureFont.h"
#include "FTGLPixmapFont.h"
#include "FTGLBitmapFont.h"

#ifdef __linux__
#define FONT_FILE "/usr/share/fonts/truetype/arial.ttf"
#else
#define FONT_FILE ":Stacey:Curlz MT"
//#define FONT_FILE ":Stacey:arial.ttf"
#endif

#define EDITING 1
#define INTERACTIVE 2

#define FTGL_BITMAP 0
#define FTGL_PIXMAP 1
#define FTGL_OUTLINE 2
#define FTGL_POLYGON 3
#define FTGL_EXTRUDE 4
#define FTGL_TEXTURE 5

int current_font = FTGL_EXTRUDE;

GLint w_win = 640, h_win = 480;
float posX, posY, posZ;
int mode = INTERACTIVE;
int carat = 0;
char myString[16];

static FTFont* fonts[6];

void SetCamera(void);

void my_lighting()
{
   // Set up lighting.
   float light1_ambient[4]  = { 1.0, 1.0, 1.0, 1.0 };
   float light1_diffuse[4]  = { 1.0, 0.9, 0.9, 1.0 };
   float light1_specular[4] = { 1.0, 0.7, 0.7, 1.0 };
   float light1_position[4] = { -1.0, 1.0, 1.0, 0.0 };
   glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
   glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
   glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
   glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
   glEnable(GL_LIGHT1);

   float light2_ambient[4]  = { 0.2, 0.2, 0.2, 1.0 };
   float light2_diffuse[4]  = { 0.9, 0.9, 0.9, 1.0 };
   float light2_specular[4] = { 0.7, 0.7, 0.7, 1.0 };
   float light2_position[4] = { 1.0, -1.0, -1.0, 0.0 };
   glLightfv(GL_LIGHT2, GL_AMBIENT,  light2_ambient);
   glLightfv(GL_LIGHT2, GL_DIFFUSE,  light2_diffuse);
   glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular);
   glLightfv(GL_LIGHT2, GL_POSITION, light2_position);
   glEnable(GL_LIGHT2);

   float front_emission[4] = { 0.3, 0.2, 0.1, 0.0 };
   float front_ambient[4]  = { 0.2, 0.2, 0.2, 0.0 };
   float front_diffuse[4]  = { 0.95, 0.95, 0.8, 0.0 };
   float front_specular[4] = { 0.6, 0.6, 0.6, 0.0 };
   glMaterialfv(GL_FRONT, GL_EMISSION, front_emission);
   glMaterialfv(GL_FRONT, GL_AMBIENT, front_ambient);
   glMaterialfv(GL_FRONT, GL_DIFFUSE, front_diffuse);
   glMaterialfv(GL_FRONT, GL_SPECULAR, front_specular);
   glMaterialf(GL_FRONT, GL_SHININESS, 16.0);
   glColor4fv(front_diffuse);

   glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
   glEnable(GL_CULL_FACE);
   glColorMaterial(GL_FRONT, GL_DIFFUSE);
   glEnable(GL_COLOR_MATERIAL);

   glEnable(GL_LIGHTING);
   glShadeModel(GL_SMOOTH);
}


void do_display (void)
{
	switch( current_font)
	{
		case FTGL_BITMAP:
			glDisable( GL_BLEND);
			break;
		case FTGL_PIXMAP:
			glDisable( GL_TEXTURE_2D);
			glEnable(GL_BLEND);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE
			break;
		case FTGL_OUTLINE:
			glDisable( GL_TEXTURE_2D);
			glEnable( GL_LINE_SMOOTH);
			glEnable(GL_BLEND);
			glBlendFunc( GL_SRC_ALPHA, GL_ONE); // GL_ONE_MINUS_SRC_ALPHA
			break;
		case FTGL_POLYGON:
			glDisable( GL_BLEND);
			my_lighting();
			break;
		case FTGL_EXTRUDE:
			glEnable( GL_DEPTH_TEST);
			glDisable( GL_BLEND);
			my_lighting();
			break;
		case FTGL_TEXTURE:
			glEnable( GL_TEXTURE_2D);
			glDisable( GL_DEPTH_TEST);
			my_lighting();
			glNormal3f( 0.0, 0.0, 1.0);
//			glDisable( GL_BLEND);
			break;

	}

	glColor3f( 1.0, 1.0, 1.0);
	
	fonts[current_font]->render( myString);

	int x1, y1, z1, x2, y2, z2;
	fonts[current_font]->BBox( myString, x1, y1, z1, x2, y2, z2);
	
	// Draw the bounding box
	glDisable( GL_LIGHTING);
	glColor3f( 0.0, 1.0, 0.0);
	// Draw the front face
	glBegin( GL_LINE_LOOP);
		glVertex3f( x1, y1, z1);
		glVertex3f( x1, y2, z1);
		glVertex3f( x2, y2, z1);
		glVertex3f( x2, y1, z1);
	glEnd();
	// Draw the back face
	if( current_font == FTGL_EXTRUDE && z1 != z2)
	{
		glBegin( GL_LINE_LOOP);
			glVertex3f( x1, y1, z2);
			glVertex3f( x1, y2, z2);
			glVertex3f( x2, y2, z2);
			glVertex3f( x2, y1, z2);
		glEnd();
	// Join the faces
		glBegin( GL_LINES);
			glVertex3f( x1, y1, z1);
			glVertex3f( x1, y1, z2);
			
			glVertex3f( x1, y2, z1);
			glVertex3f( x1, y2, z2);
			
			glVertex3f( x2, y2, z1);
			glVertex3f( x2, y2, z2);
			
			glVertex3f( x2, y1, z1);
			glVertex3f( x2, y1, z2);
		glEnd();
	}
		
		// Draw the baseline, Ascender and Descender
	glBegin( GL_LINES);
		glColor3f( 0.0, 0.0, 1.0);
		glVertex3f( 0.0, 0.0, 0.0);
		glVertex3f( fonts[current_font]->Advance( myString), 0.0, 0.0);
		
		glVertex3f( 0.0, fonts[current_font]->Ascender(), 0.0);
		glVertex3f( 0.0, fonts[current_font]->Descender(), 0.0);
		
	glEnd();
	
	// Draw the origin
	glColor3f( 1.0, 0.0, 0.0);
	glPointSize( 5.0);
	glBegin( GL_POINTS);
		glVertex3f( 0.0, 0.0, 0.0);
	glEnd();

	glColor3f( 1.0, 1.0, 1.0);
	glutSwapBuffers();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   	SetCamera();
	
	glPushMatrix();

	switch( current_font)
	{
		case FTGL_BITMAP:
		case FTGL_PIXMAP:
			glRasterPos2i( w_win / 2, h_win / 2);
			glTranslatef(  w_win / 2, h_win / 2, 0.0);
			break;
		case FTGL_OUTLINE:
		case FTGL_POLYGON:
		case FTGL_EXTRUDE:
		case FTGL_TEXTURE:
			tbMatrix();
			break;
	}
	
	do_display();

	glPopMatrix();

}

void myinit ( const char* fontfile)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor( 0.13, 0.17, 0.32, 0.0);
	glColor3f( 1.0, 1.0, 1.0);
	
	glEnable( GL_CULL_FACE);
	glFrontFace( GL_CCW);
	
	glEnable( GL_DEPTH_TEST);
	
	glEnable( GL_POLYGON_OFFSET_LINE);
	glPolygonOffset( 1.0, 1.0); // ????
	 	
	SetCamera();

	fonts[FTGL_BITMAP] = new FTGLBitmapFont;
	fonts[FTGL_PIXMAP] = new FTGLPixmapFont;
	fonts[FTGL_OUTLINE] = new FTGLOutlineFont;
	fonts[FTGL_POLYGON] = new FTGLPolygonFont;
	fonts[FTGL_EXTRUDE] = new FTGLExtrdFont;
	fonts[FTGL_TEXTURE] = new FTGLTextureFont;

	for( int x = 0; x < 6; ++x)
	{
		if( !fonts[x]->Open( fontfile, false))
		{
			fprintf( stderr, "Failed to open font %s", fontfile);
			exit(1);
		}
		
		if( !fonts[x]->FaceSize( 144))
		{
			fprintf( stderr, "Failed to set size");
			exit(1);
		}
	
		fonts[x]->Depth(20);
	}
	
	myString[0] = 65;
	myString[1] = 0;
	

	tbInit(GLUT_LEFT_BUTTON);
	tbAnimate( GL_FALSE);

}


void parsekey(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 27: exit(0); break;
		case 13:
			if( mode == EDITING)
			{
				mode = INTERACTIVE;
			}
			else
			{
				mode = EDITING;
				carat = 0;
			}
			break;
		case ' ':
			current_font++;
			if(current_font > 5)
				current_font = 0;
			break;
		default:
			if( mode == INTERACTIVE)
			{
				myString[0] = key;
				myString[1] = 0;
				break;
			}
			else
			{
				myString[carat] = key;
				myString[carat + 1] = 0;
				carat = carat > 14 ? 15 : ++carat;
			}
	}
	
	glutPostRedisplay();

}


void parsekey_special(int key, int x, int y)
{
	switch (key)
	{
		case GLUT_KEY_UP:
			posY += 10;
			break;
		case GLUT_KEY_DOWN:
			posY -= 10;
			break;
		case GLUT_KEY_RIGHT:
			posX += 10;
			break;
		case GLUT_KEY_LEFT:
			posX -= 10;
			break;
	}
}

void motion(int x, int y)
{
	tbMotion( x, y);
}

void mouse(int button, int state, int x, int y)
{
	tbMouse( button, state, x, y);
}

void myReshape(int w, int h)
{
	glMatrixMode (GL_MODELVIEW);
	glViewport (0, 0, w, h);
	glLoadIdentity();
		
	w_win = w;
	h_win = h;
	SetCamera();
	
	tbReshape(w_win, h_win);
}

void SetCamera(void)
{
	switch( current_font)
	{
		case FTGL_BITMAP:
		case FTGL_PIXMAP:
			glMatrixMode( GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(0, w_win, 0, h_win);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			break;
		case FTGL_OUTLINE:
		case FTGL_POLYGON:
		case FTGL_EXTRUDE:
		case FTGL_TEXTURE:
			glMatrixMode (GL_PROJECTION);
			glLoadIdentity ();
			gluPerspective( 90, w_win/ h_win, 1, 1000);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt( 0.0, 0.0, h_win / 2, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
			break;
	}	
}


int main(int argc, char *argv[])
{
	char* fontfile = FONT_FILE;

	if (argc == 2)
		fontfile = argv[1];

	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glutInitWindowPosition(50, 50);
	glutInitWindowSize( w_win, h_win);
	glutCreateWindow("FTGL TEST");
	glutDisplayFunc(display);
	glutKeyboardFunc(parsekey);
	glutMouseFunc(mouse);
    glutMotionFunc(motion);
	glutSpecialFunc(parsekey_special);
	glutReshapeFunc(myReshape);
	glutIdleFunc(display);

	myinit( fontfile);

	glutMainLoop();

	return 0;
}