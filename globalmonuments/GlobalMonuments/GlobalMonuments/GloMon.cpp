#include "Texture.h"
#include "ObjModel.h"
#include "CameraParams.h"
#include "PatternDetector.h"

using namespace ARma;

float angle, eyeAngle = 0;
const float ROTATE_FACTOR = 3.0f;
const float MOVE_FACTOR = 1.0f;
float moveX, moveY, moveZ = 0;
int eyeX, eyeY = 0;
float cameraX, cameraZ;
int windowWidth = 1000;
int windowHeight = 800;
GLuint textureId = 0;
Texture webcamTexture;

int currentModel = 0;
float rotationX, rotationY, rotationZ = 0;
vector<pair<int, ObjModel*> > models;

int orientation;

cv::VideoCapture cap;
cv::Mat dataImage;

// PATTERN DETECTION
#define PAT_SIZE 64//equal to pattern_size variable (see below)
#define SAVE_VIDEO 0 //if true, it saves the video in "output.avi"
#define NUM_OF_PATTERNS 3// define the number of patterns you want to use

char* filename1 = "pattern1.png";//id=1
char* filename2 = "gmMarker.png";//id=2
char* filename3 = "pattern4.png";//id=3

CvCapture* capture;
vector<cv::Mat> patternLibrary;
vector<Pattern> detectedPattern;
int patternCount = 0;

int normPatternSize = PAT_SIZE;
double fixedThresh = 40;
double adaptThresh = 5;//non-used with FIXED_THRESHOLD mode
int adaptBlockSize = 45;//non-used with FIXED_THRESHOLD mode
double confidenceThreshold = 0.35;
int mode = 2;//1:FIXED_THRESHOLD, 2: ADAPTIVE_THRESHOLD

PatternDetector myDetector(fixedThresh, adaptThresh, adaptBlockSize, confidenceThreshold, normPatternSize, mode);

static int loadPattern(const char*, std::vector<cv::Mat>&, int&);
void drawAxis(void);
void trackLoop(void);

void initGraphics(void)
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
	models.push_back(pair<int, ObjModel*>(75, new ObjModel("models/planets/earth_night.obj")));
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

	glColor3f(1.0f, 1.0f, 1.0f); // Stops color blending, but it does not seem best practice this way
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

void drawWebcamPlane(void) 
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	webcamTexture.activateTexture();
	glBegin(GL_QUADS);
	// Adjust values so they fit the viewport exactly
	glTexCoord2f(0, 0);	glVertex3f(-10, 14, 18);
	glTexCoord2f(1, 0); glVertex3f(-10, 14, -18);
	glTexCoord2f(1, 1);	glVertex3f(-10, -14, -18);
	glTexCoord2f(0, 1);	glVertex3f(-10, -14, 18);
	glEnd();
	glDisable(GL_TEXTURE_2D); // stops textures from overdrawing on glColor3f()

	glPopMatrix();
}

void display(void)
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

	glDisable(GL_BLEND);
	drawWebcamPlane();

	glTranslatef(moveX, moveY, moveZ);

	glTranslatef(0, 0, 0); // move back to origin so planet rotates in the origin
	rotationZ = orientation;
	glRotatef(rotationX, 1, 0, 0); // Rotations dont rotate around their axis (except last one)
	glRotatef(rotationY, 0, 1, 0);
	glRotatef(rotationZ, 0, 0, 1);
	
	if (models.size() > 0)
		models[currentModel].second->draw();

	//drawTexCube();
	drawAxis();
	glutSwapBuffers();
}

void reshape(GLint width, GLint height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, width / (float)height, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
}

void idleFunc(void)
{
	angle += 0.1;	
	glutPostRedisplay();		

	cap >> dataImage;
	
	//webcamTexture = Texture(dataImage.ptr(), 0, dataImage.cols, dataImage.rows);
	//webcamTexture.loadTexture(1);
	webcamTexture.changeTexture();	

	//cv::imshow("Test", dataImage);
	//glGenTextures(1, &textureId); dont keep generating new textures! >:(
	
	trackLoop();
}

void keyboard(unsigned char key, int x, int y)
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
	case 'x':
		rotationX -= ROTATE_FACTOR;
		break;
	case 'z':
		rotationX += ROTATE_FACTOR;
		break;
	case 'q' :
		rotationY -= ROTATE_FACTOR;
		break;
	case 'e':
		rotationY += ROTATE_FACTOR;
		break;
	case 'c':
		rotationZ -= ROTATE_FACTOR;
		break;
	case 'v':
		rotationZ += ROTATE_FACTOR;
		break;
	case 'j':
		moveZ += MOVE_FACTOR;
		break;
	case 'l':
		moveZ -= MOVE_FACTOR;
		break;
	case 'i':
		moveY += MOVE_FACTOR;
		break;
	case 'k':
		moveY -= MOVE_FACTOR;
		break;
	case 'o':
		moveX += MOVE_FACTOR;
		break;
	case 'u':
		moveX -= MOVE_FACTOR;
		break;
	}
}

// PATTERN DETECTION
int loadPattern(const char* filename, std::vector<cv::Mat>& library, int& patternCount)
{
	Mat img = imread(filename, 0);

	if (img.cols != img.rows)
	{
		return -1;
		printf("Not a square pattern");
	}

	int msize = PAT_SIZE;

	Mat src(msize, msize, CV_8UC1);
	Point2f center((msize - 1) / 2.0f, (msize - 1) / 2.0f);
	Mat rot_mat(2, 3, CV_32F);

	resize(img, src, Size(msize, msize));
	Mat subImg = src(Range(msize / 4, 3 * msize / 4), Range(msize / 4, 3 * msize / 4));
	library.push_back(subImg);

	rot_mat = getRotationMatrix2D(center, 90, 1.0);

	for (int i = 1; i < 4; i++)
	{
		Mat dst = Mat(msize, msize, CV_8UC1);
		rot_mat = getRotationMatrix2D(center, -i * 90, 1.0);
		warpAffine(src, dst, rot_mat, Size(msize, msize));
		Mat subImg = dst(Range(msize / 4, 3 * msize / 4), Range(msize / 4, 3 * msize / 4));
		library.push_back(subImg);
	}
	patternCount++;
	return 1;
}

int main(int argc, char** argv) 
{
	// PATTERN DETECTION
	/*create patterns' library using rotated versions of patterns
	*/
		loadPattern(filename1, patternLibrary, patternCount);
	#if (NUM_OF_PATTERNS==2)
		loadPattern(filename2, patternLibrary, patternCount);
	#endif
	#if (NUM_OF_PATTERNS==3)
		loadPattern(filename2, patternLibrary, patternCount);
		loadPattern(filename3, patternLibrary, patternCount);
	#endif

	cout << patternCount << " patterns are loaded." << endl;
	capture = cvCaptureFromCAM(1);

	// GLUT
	glutInit(&argc, argv);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("GLUT example");
	// Initialize OpenGL graphics state
	initGraphics();
	// Register callbacks:
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idleFunc);
	// Turn the flow of control over to GLUT

	// Mist
	//glEnable(GL_FOG);
	float FogCol[3] = { 0.9f, 0.0f, 0.0f };
	glFogfv(GL_FOG_COLOR, FogCol);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 10.0f);
	glFogf(GL_FOG_END, 60.0f);

	// OpenCV
	cap.open(2);

	cap >> dataImage; // do this once so it points to actual data later on
	webcamTexture = Texture(dataImage.ptr(), 0, dataImage.cols, dataImage.rows);
	webcamTexture.loadTexture(1);

	glutMainLoop();	
	return 0;
}

void trackLoop(void) 
{
	//mycapture >> imgMat; 
	IplImage* img = cvQueryFrame(capture);
	Mat imgMat = Mat(img);
	//flip(imgMat, imgMat, 1);
	double tic = (double)cvGetTickCount();

	//run the detector
	myDetector.detect(imgMat, cameraMatrix, distortions, patternLibrary, detectedPattern);

	double toc = (double)cvGetTickCount();
	double detectionTime = (toc - tic) / ((double)cvGetTickFrequency() * 1000);
	//cout << "Detected Patterns: " << detectedPattern.size() << endl;
	//cout << "Detection time: " << detectionTime << endl;

	//augment the input frame (and print out the properties of pattern if you want)
	for (unsigned int i = 0; i < detectedPattern.size(); i++)
	{
		detectedPattern.at(i).showPattern();
		detectedPattern.at(i).draw(imgMat, cameraMatrix, distortions);
		orientation = detectedPattern.at(i).orientation;
		cout << "Orientation: " << detectedPattern.at(i).orientation << endl;
		// Draw OpenGL here
	}

	imshow("result", imgMat);
	cvWaitKey(1);

	detectedPattern.clear();
}
