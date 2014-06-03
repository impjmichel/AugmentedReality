#include <Windows.h>
#include <gl/glut.h>
#include <gl/GL.h>
#include <gl/glu.h>
#include <math.h>
#include "ObjModel.h"


float rotation = 0;
vector<pair<int, ObjModel*> > models;
int currentModel = 0;

float eyeAngle, rollAngle = 0;
float cameraX = 10 * -cos(eyeAngle);
float cameraY = 0.8f;
float cameraZ = 10 * sin(eyeAngle);

void drawAxis(void);
void drawCube(void);

void display(void)
{
	glClearColor(0.6f, 0.6f, 0.9f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*
	gluLookAt(0, models[currentModel].first*1.1, models[currentModel].first * 2,  // eye
		0, 0, 0,  // center
		0, 1, 0); // up
	*/	
	
	gluLookAt(
		cameraX, cameraY, cameraZ,	// eye
		0, 0, 0,	// center
		0, 1, 0);	// up
		
	drawAxis();
	glRotatef(rotation, 0, 1, 0);
	drawCube();
	if (models.size() > 0)
		models[currentModel].second->draw();

	glutSwapBuffers();
}

void reshape(GLint width, GLint height)
{
	glViewport(0,0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, width/(float)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void InitGraphics(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// geen progressive JPGs! ;)
	//models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/globe/globe3.obj")));
	models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/vampire/3D_vampire.obj")));
	//models.push_back(pair<int, ObjModel*>(1, new ObjModel("models/sk_cube/sk_cube.obj")));
	//models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/car/honda_jazz.obj")));
	//models.push_back(pair<int, ObjModel*>(1, new ObjModel("models/bloemetje/PrimRoseP.obj")));
	//models.push_back(pair<int, ObjModel*>(1, new ObjModel("models/cube/cube-textures.obj")));
	//models.push_back(pair<int, ObjModel*>(35, new ObjModel("models/ship/shipA_OBJ.obj")));
}

void MouseButton(int button, int state, int x, int y)
{

}

void MouseMotion(int x, int y)
{

}

void IdleFunc(void)
{
	rotation += 0.25f;
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	float fraction = 0.1f;
	switch (key)
	{
	case 27:             // ESCAPE key
		exit (0);
		break;
	case '[':
		currentModel = (currentModel + models.size() - 1) % models.size();
		break;
	case ']':
		currentModel = (currentModel + 1) % models.size();
		break;
	// eye
	case 'd':
		eyeAngle -= 0.1f;
		cameraX = 10 * -cos(eyeAngle);
		cameraZ = 10 * sin(eyeAngle);
		break;
	case 'a':
		eyeAngle += 0.1f;
		cameraZ = 10 * sin(eyeAngle);
		cameraX = 10 * -cos(eyeAngle);
		break;
	case 'w':
		rollAngle += 0.1f;
		eyeAngle -= 0.1f;
		cameraY = 10 * sin(rollAngle);
		cameraX = 10 * -cos(eyeAngle);
		break;
	case 's':
		rollAngle -= 0.1f;
		eyeAngle += 0.1f;
		cameraY = 10 * sin(rollAngle);
		cameraX = 10 * -cos(eyeAngle);
		break;
	// reset view
	case 'r':
		eyeAngle = rollAngle = 0;
		float cameraX = 10 * -cos(eyeAngle);
		float cameraY = 0.8f;
		float cameraZ = 10 * sin(eyeAngle);
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit (&argc, argv);
	glutInitWindowSize (1024, 768);
	glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow ("GLUT example");
	// Initialize OpenGL graphics state
	InitGraphics();
	// Register callbacks:
	glutDisplayFunc (display);
	glutReshapeFunc (reshape);
	glutKeyboardFunc (Keyboard);
	glutMouseFunc (MouseButton);
	glutMotionFunc (MouseMotion);
	glutIdleFunc (IdleFunc);
	// Turn the flow of control over to GLUT
	glutMainLoop ();
	return 0;
}

void drawAxis(void) {
	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// why can I not draw colors?
	// X-axis
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glPointSize(3.0);
	glVertex3f(0, 0, 0);
	glVertex3f(10, 0, 0);
	glEnd();

	// Y-axis
	glBegin(GL_LINES);
	glColor3f(0.0f, 1.0f, 0.0f);
	glPointSize(3.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 10, 0);
	glEnd();

	// Z-axis
	glBegin(GL_LINES);
	glColor3f(0.0f, 0.0f, 1.0f);
	glLineWidth(5.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 10);
	glEnd();

	glPopMatrix();
}

void drawCube(void)
{
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);
	glEnd();


	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, -1, 1);
	glVertex3f(-1, 1, 1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1, 1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(-1, 1, -1);
	glEnd();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);
	glEnd();
	glPopMatrix();
}