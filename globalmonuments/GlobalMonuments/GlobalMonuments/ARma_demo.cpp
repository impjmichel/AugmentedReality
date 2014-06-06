#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cameraparams.h"
#include "patterndetector.h"

#include <GL/freeglut.h>
#include <GL/GL.h>

using namespace std;
using namespace cv;
using namespace ARma;

#define PAT_SIZE 64//equal to pattern_size variable (see below)
#define SAVE_VIDEO 0 //if true, it saves the video in "output.avi"
#define NUM_OF_PATTERNS 3// define the number of patterns you want to use

char* filename1="pattern1.png";//id=1
char* filename2="pattern4.png";//id=2
char* filename3="pattern5.png";//id=3

float angle, eyeAngle = 0;
int eyeX, eyeY = 0;
int windowWidth = 800;
int windowHeight = 600;
GLuint textureId = 0;

static int loadPattern(const char* , std::vector<cv::Mat>& , int& );
void drawAxis(void);

void InitGraphics(void)
{

}

int loadPattern(const char* filename, std::vector<cv::Mat>& library, int& patternCount){
	
	//try {

		Mat img = imread(filename, 0);

		if (img.cols != img.rows){
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

		for (int i = 1; i < 4; i++){
			Mat dst = Mat(msize, msize, CV_8UC1);
			rot_mat = getRotationMatrix2D(center, -i * 90, 1.0);
			warpAffine(src, dst, rot_mat, Size(msize, msize));
			Mat subImg = dst(Range(msize / 4, 3 * msize / 4), Range(msize / 4, 3 * msize / 4));
			library.push_back(subImg);
		}

		patternCount++;

	return 1;

	//}
	//catch (cv::Exception & e)
	//{
	//	cout << "Fout: " << e.msg << endl;
	//}
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

void Display(void)
{
	glEnable(GL_DEPTH_TEST); // prevents texture overlay
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, (windowWidth / windowHeight), 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	float cameraX = 10 * cos(eyeAngle);
	float cameraZ = 10 * sin(eyeAngle);

	gluLookAt(cameraX, eyeY, cameraZ,	// eye
		0, 0, 0,	// center
		0, 1, 0);	// up

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawAxis();
	glutSwapBuffers();
}

void Reshape(GLint width, GLint height)
{
	glViewport(0, 0, width, height);
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
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:             // ESCAPE key
		exit(0);
		break;
	case 'w':
		eyeY++;
		break;
	case 'a':
		eyeX--;
		break;
	case 's':
		eyeY--;
		break;
	case 'd':
		eyeX++;
		break;
	case 'q':
		eyeAngle -= 0.1f;
		break;
	case 'e':
		eyeAngle += 0.1f;
		break;
	case 'r':
		eyeY = eyeX = 0;
		break;
	}
}


int main(int argc, char** argv) {

	std::vector<cv::Mat> patternLibrary;
	std::vector<Pattern> detectedPattern;
	int patternCount = 0;


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
	glEnable(GL_FOG);
	float FogCol[3] = { 0.9f, 0.0f, 0.0f };
	glFogfv(GL_FOG_COLOR, FogCol);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogf(GL_FOG_START, 10.0f);
	glFogf(GL_FOG_END, 40.0f);

	glutMainLoop();

	// OPENGL


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


	int norm_pattern_size = PAT_SIZE;
	double fixed_thresh = 40;
	double adapt_thresh = 5;//non-used with FIXED_THRESHOLD mode
	int adapt_block_size = 45;//non-used with FIXED_THRESHOLD mode
	double confidenceThreshold = 0.35;
	int mode = 2;//1:FIXED_THRESHOLD, 2: ADAPTIVE_THRESHOLD

	PatternDetector myDetector(fixed_thresh, adapt_thresh, adapt_block_size, confidenceThreshold, norm_pattern_size, mode);

	CvCapture* capture = cvCaptureFromCAM(1);

#if (SAVE_VIDEO)
	CvVideoWriter *video_writer = cvCreateVideoWriter("output.avi", -1, 25, cvSize(640, 480));
#endif

	Mat imgMat;
	while(true)
	{
		//mycapture >> imgMat; 
		IplImage* img = cvQueryFrame(capture);
		Mat imgMat = Mat(img);
		//flip(imgMat, imgMat, 1);
		double tic = (double)cvGetTickCount();

		//run the detector
		myDetector.detect(imgMat, cameraMatrix, distortions, patternLibrary, detectedPattern);

		double toc=(double)cvGetTickCount();
		double detectionTime = (toc-tic)/((double) cvGetTickFrequency()*1000);
		//cout << "Detected Patterns: " << detectedPattern.size() << endl;
		//cout << "Detection time: " << detectionTime << endl;

		//augment the input frame (and print out the properties of pattern if you want)
		for (unsigned int i =0; i<detectedPattern.size(); i++)
		{
			detectedPattern.at(i).showPattern();
			detectedPattern.at(i).draw( imgMat, cameraMatrix, distortions);

			// Draw OpenGL here
		}

#if (SAVE_VIDEO)
		cvWriteFrame(video_writer, &((IplImage)imgMat));
#endif
		imshow("result", imgMat);
		cvWaitKey(1);

		detectedPattern.clear();
	}

#if (SAVE_VIDEO)
	cvReleaseVideoWriter(&video_writer);
#endif
	cvReleaseCapture(&capture);

	return 0;

}
