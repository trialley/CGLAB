#include"getFilePathName.h"
#include<iostream>
#include<fstream>
#include <math.h>
#include<string>
#include<queue>
#include "glut.h"
#include"loader.h"
#pragma comment(lib, "./glut32.lib")
using namespace std;


/*加载器*/
loader loader1;

/*确定窗口大小*/
int WIDTH = 700;
int HEIGHT = 700;

/*用于旋转*/
float px;
float py;
float theta1 = 0;
float theta2 = 0;

void onMouseClick (int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		px = x;
		py = y;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
			loader1.displaystate = (loader1.displaystate + 1) % 3;
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		string temp = getFilePathName ();
		cout << "新文件：" << temp << "\n";
		loader1.reload (temp);
	}
	if (button == GLUT_WHEEL_DOWN) {
		loader1.scale/=2;
	}
	if (button == GLUT_WHEEL_UP) {
		loader1.scale*=2;
	}

	glutPostRedisplay ();
}
void onMouseMove (int x, int y) {
	float dx = 20.0f / WIDTH;
	float dy = 20.0f / HEIGHT;

	if (x - px > 2) {
		theta1 -= 0.1;
		px = x;
		loader1.rotate (theta1, theta2);
	} else if (x - px < -2) {
		theta1 += 0.1;
		px = x;
		loader1.rotate (theta1, theta2);
	}

	if (y - py > 5) {
		theta2 += 0.1;
		py = y;
		loader1.rotate (theta1, theta2);
	}else if (y - py < -5) {
		theta2 -= 0.1;
		py = y;
		loader1.rotate (theta1, theta2);
	}

}
void onKeyboardPress (unsigned char key, int mouseX, int mouseY) {
	switch (key) {
	case '\r':
		loader1.loop ();
		break;
	}
	glutPostRedisplay ();
}

void myDisplay () {
	loader1.display ();
}

void reshape (int width, int height) {
	glViewport (0, 0, (GLsizei)width, (GLsizei)height);
	//设置投影参数

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	//正交投影
	float scale = 0.5;
	if (width <= height)
		glOrtho (-1.5 * scale, 1.5 * scale, -1.5 * scale * (GLfloat)height / (GLfloat)width, 1.5 * scale * (GLfloat)height / (GLfloat)width, -10.0 * scale, 10.0 * scale);
	else
		glOrtho (-1.5 * scale * (GLfloat)width / (GLfloat)height, 1.5 * scale * (GLfloat)width / (GLfloat)height, -1.5 * scale, 1.5 * scale, -10.0 * scale, 10.0 * scale);
	//设置模型参数--几何体参数
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

int main (int argc, char* argv[]) {
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition (100, 100);
	glutInitWindowSize (WIDTH, HEIGHT);
	glutCreateWindow ("实验11：loop细分 加载器");
	glEnable (GL_CULL_FACE);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	glutKeyboardFunc (&onKeyboardPress);
	glutMotionFunc (&onMouseMove);
	glutMouseFunc (&onMouseClick);
	glEnable (GL_LIGHT0);
	glEnable (GL_NORMALIZE);
	glutDisplayFunc (&myDisplay);
	glutReshapeFunc (&reshape);

	loader1.load("../off/bunny.off");
	
	glutMainLoop ();
	return 0;
}