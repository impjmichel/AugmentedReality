#include <GL/freeglut.h>
#include <GL/GL.h>
#include <iostream>
#include "Texture.h"
#include "ObjModel.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

float angle, eyeAngle = 0;
int eyeX, eyeY = 0;
float cameraX, cameraZ;
int windowWidth = 1000;
int windowHeight = 800;
GLuint textureId = 0;
Texture webcamTexture;

int currentModel = 0;
float rotation = 0;
vector<pair<int, ObjModel*> > models;

cv::VideoCapture cap;
cv::Mat dataImage;

void drawAxis(void);

void InitGraphics(void)
{
	// Lighting
	//glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/vampire/3D_vampire.obj")));	
	models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/planets/earth.obj")));
	models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/planets/mars.obj")));
	models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/planets/jupiter.obj")));
	models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/planets/saturn.obj")));
}

// OPENGL stuff, needs to be seperated in its own class

void drawAxis(void) {
	glPushMatrix();

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

void drawTexCube(void)
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	webcamTexture.activateTexture();
	glBegin(GL_QUADS);
	glTexCoord2f(2, 0);	glVertex3f(-1, -1, -1);
	glTexCoord2f(2, 1);	glVertex3f(1, -1, -1);
	glTexCoord2f(3, 1); glVertex3f(1, -1, 1);
	glTexCoord2f(3, 0);	glVertex3f(-1, -1, 1);

	glTexCoord2f(0, 0);	glVertex3f(-1, 1, -1);
	glTexCoord2f(0, 1);	glVertex3f(1, 1, -1);
	glTexCoord2f(1, 1);	glVertex3f(1, 1, 1);
	glTexCoord2f(1, 0);	glVertex3f(-1, 1, 1);

	glTexCoord2f(3, 0);	glVertex3f(-1, 1, -1);
	glTexCoord2f(3, 1);	glVertex3f(-1, -1, -1);
	glTexCoord2f(4, 1); glVertex3f(-1, -1, 1);
	glTexCoord2f(4, 0);	glVertex3f(-1, 1, 1);

	glTexCoord2f(3, 0);	glVertex3f(1, 1, -1);
	glTexCoord2f(3, 1);	glVertex3f(1, -1, -1);
	glTexCoord2f(4, 1); glVertex3f(1, -1, 1);
	glTexCoord2f(4, 0);	glVertex3f(1, 1, 1);

	glTexCoord2f(3, 0);	glVertex3f(-1, 1, -1); 		// T, L
	glTexCoord2f(3, 1);	glVertex3f(-1, -1, -1); 	// B, L
	glTexCoord2f(4, 1); glVertex3f(1, -1, -1); 		// B, R
	glTexCoord2f(4, 0);	glVertex3f(1, 1, -1);		// T, R

	glTexCoord2f(3, 0);	glVertex3f(-1, 1, 1);
	glTexCoord2f(3, 1);	glVertex3f(-1, -1, 1);
	glTexCoord2f(4, 1); glVertex3f(1, -1, 1);
	glTexCoord2f(4, 0);	glVertex3f(1, 1, 1);

	glEnd();

	glDisable(GL_TEXTURE_2D); // stops textures from overdrawing on glColor3f()

	glPopMatrix();
}

void drawWebcamPlane(void) {
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	webcamTexture.activateTexture();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);	glVertex3f(-10, 10, 20);
	glTexCoord2f(1, 0); glVertex3f(-10, 10, -20);
	glTexCoord2f(1, 1);	glVertex3f(-10, -10, -20);
	glTexCoord2f(0, 1);	glVertex3f(-10, -10, 20);	
	glEnd();

	glDisable(GL_TEXTURE_2D); // stops textures from overdrawing on glColor3f()

	glPopMatrix();
}

void Display(void)
{
	glEnable(GL_DEPTH_TEST); // prevents texture overlay
	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	gluPerspective(90, (windowWidth / windowHeight), 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cameraX = 10 * cos(eyeAngle);
	cameraZ = 10 * sin(eyeAngle);

	gluLookAt(cameraX, eyeY, cameraZ,	// eye
		0, 0, 0,	// center
		0, 1, 0);	// up

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawWebcamPlane();

	glRotatef(rotation, 0, 1, 0);
	if (models.size() > 0)
		models[currentModel].second->draw();

	
	//drawTexCube();
	//drawAxis();

	glutSwapBuffers();
}

void Reshape(GLint width, GLint height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, width / (float)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}


void MouseButton(int button, int state, int x, int y)
{
}

void MouseMotion(int x, int y)
{
}

void IdleFunc(void)
{
	angle += 0.1;
	rotation += 0.25f;
	glutPostRedisplay();		

	cap >> dataImage;
	
	//webcamTexture = Texture(dataImage.ptr(), 0, dataImage.cols, dataImage.rows);
	//webcamTexture.loadTexture(1);
	webcamTexture.changeTexture();	

	cv::imshow("Test", dataImage);
	//glGenTextures(1, &textureId); dont keep generating new textures! >:(
	
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:             // ESCAPE key
		exit(0);
		break;
	case '[':
		currentModel = (currentModel + models.size() - 1) % models.size();
		break;
	case ']':
		currentModel = (currentModel + 1) % models.size();
		break;
	case 'w':
		eyeY++;
		break;
	case 's':
		eyeY--;
		break;
	case 'a':
		eyeAngle -= 0.1f;
		break;
	case 'd':
		eyeAngle += 0.1f;
		break;
	case 'r':
		eyeY = eyeX = 0;
		break;
	}
}

int main(int argc, char** argv) 
{

	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("GLUT example");
	// Initialize OpenGL graphics state
	InitGraphics();
	// Register callbacks:
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutIdleFunc(IdleFunc);
	// Turn the flow of control over to GLUT

	// Mist
	//glEnable(GL_FOG);
	float FogCol[3] = { 0.9f, 0.0f, 0.0f };
	glFogfv(GL_FOG_COLOR, FogCol);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 10.0f);
	glFogf(GL_FOG_END, 60.0f);

	// OpenCV
	cap.open(1);

	cap >> dataImage; // do this once so it points to actual data later on
	webcamTexture = Texture(dataImage.ptr(), 0, dataImage.cols, dataImage.rows);
	webcamTexture.loadTexture(1);

	glutMainLoop();

	return 0;
}
