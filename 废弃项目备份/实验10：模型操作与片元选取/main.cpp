// 3DModel.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include "ObjLoader.h"
#include <iostream>
#include"getFilePathName.h"
#include"glut.h"
#pragma comment(lib, "./glut32.lib")
//变换模式
#define ROTATE_X 1
#define ROTATE_Y 2
#define ROTATE_Z 3
#define SCALE_WHOLE 4
#define SCALE_X 5
#define SCALE_Y 6
#define SCALE_Z 7
#define TRANSLATION_X 8
#define TRANSLATION_Y 9
#define TRANSLATION_Z 10
int type = 0;
using namespace std;
//窗口大小
int width = 800;
int height = 800;
bool ok[5000] = { 0 };
//实现移动鼠标观察模型所需变量
//弧度
float radian = 3.1415926535 / 180.0f;
//旋转角度
float degree = 0;
int oldPosX = 0;
int oldPosY = 0;
float radius = 0;//定义摄像机位置和方向
GLfloat ShootPosition[] = { 0,0,0 };
GLfloat ShootDirect[] = { 0,0,0 };
//读取obj的类
ObjLoader obj;
typedef enum menuCtrfunc { NOTHING = 999, OPEN_NEW_FILE, DISPLAY_MODE_FILL, DISPLAY_MODE_LINE, DISPLAY_MODE_POINT }menuCtrfunc;
typedef enum mousemode {MOUSE_NOTHING=9999,LEFT_DOWN}mousemode;
mousemode mousemoded = MOUSE_NOTHING;
//设置光源
void setLightRes () {
	GLfloat lightPosition[] = { 1.0f, 1.0f, 0.5f, 1.0f };
	glLightfv (GL_LIGHT0, GL_AMBIENT, lightPosition);
	glEnable (GL_LIGHTING); //启用光源
	glEnable (GL_LIGHT0);   //使用指定灯光
}
int display_moded= GL_LINE;
//显示画面
void display () {
	glPolygonMode (GL_FRONT_AND_BACK, display_moded);
	//glPolygonMode (GL_BACK, display_moded);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f (1, 1, 1);

	//绘制文字
	glLoadIdentity ();
	glTranslatef (-11.0f, 10.0f, -20.0f);
	unsigned char str[] = "Press right button.";
	//glDrawString (str);

	//设置光源
//	setLightRes ();

	//设置投影矩阵
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (60.0, 1.0, 1.0, 40.0);

	//设置视觉
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	gluLookAt (1, 0, 0, 0, 0, 0, 0, 1, 0);

	//绘制模型
	obj.DrawModel (GL_RENDER,ok);

	glutSwapBuffers ();
	glutPostRedisplay ();
}


void ProcessPicks (GLint hits, GLuint buffer[]) {
	GLint i;
	GLuint j;
	GLuint names, * ptr;
	printf ("hit=%d-----------------------\n", hits);
	ptr = (GLuint*)buffer;
	float minz1 = 1;
	float minz2 = 1;
	for (i = 0; i < hits; i++) {
		names = *ptr;
		printf("number of names for this hit =%d\n", names);

		ptr++;
		float z1 = (float)((float)*ptr / (float)0xFFFFFFFF);
		printf("z1 is %g;\n", z1);//selectBuffer缓冲中保存了名字栈中名字的个数，选择到的物体的最小最大深度值
		if (z1 < minz1) {
			obj.picked = *(ptr + 2);
		}

		ptr++;
		printf("z2 is %g;\n", (float)((float)*ptr/ (float)0xFFFFFFFF));//我们用z1变量显示出物体最小的深度值，z2变量显示出物体最大的深度值，
		float z2 = (float)*ptr / 0xFFFFFFFF;
		if (z1<=minz1 && z2 < minz2) {
			obj.picked = *(ptr + 1);
		}

		//这个值是0~1的值，离我们最近的为0最远的为1，但是selectbuffer是整形的数组，因此保存的时候乘以了0x7fffffff，因此显示的时候也要除以该值
		//if((float)*ptr / 0x7fffffff <)
		ptr++;
		printf ("names are ");
		obj.picked = *ptr;

		for (j = 0; j < names; j++) {
			printf ("%d", *ptr);
			ptr++;
		}
		printf ("\n\n\n");
		cout << "pick:" << obj.picked << "\n";
	}
}

void pick (int x,int y){
	GLuint pickBuffer[3200];
	GLint nPicks, vp[4];

	glGetIntegerv (GL_VIEWPORT, vp);//获得当前窗口显示区域的参数
	//glReadPixels (x, vp[3]-y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, pickBuffer);
	//for (int i = 0; i < 10;i++) {
	//	cout << (float	)pickBuffer[i]<<"\n";
	//}
	glSelectBuffer (3200, pickBuffer);//设置选择缓冲区
	glRenderMode (GL_SELECT);//激活选择模式
	glInitNames ();//初始化名字堆栈
	glPushName (0);
	glMatrixMode (GL_PROJECTION);
	glEnable (GL_DEPTH_TEST);

	glPushMatrix ();//将当前的投影矩阵复制一个并压入堆栈
	glLoadIdentity ();

	//定义一个10*10的选择区域
	//gluLookAt (r * cos (c * degree), r * sin (c * 0.5 * degreey), r * sin (c * degree), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluLookAt (1, 0, 0, 0, 0, 0, 0, 1, 0);
	gluPickMatrix (GLdouble (x), GLdouble (vp[3] - y), 1.0, 1.0, vp);
	//gluLookAt (r * cos (c * degree), r * sin (c * 0.5 * degreey), r * sin (c * degree), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	gluPerspective (60.0, 1.0, 0.0, 40.0);
	obj.DrawModel ( GL_SELECT, ok);


	//恢复投影变换
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();//将投影矩阵堆栈中的栈顶元素删除
	glFlush ();
	//获得选择集并输出
	ProcessPicks (glRenderMode (GL_RENDER), pickBuffer);//输出选择结果
}
//移动鼠标来控制
void moveClick (int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mousemoded = LEFT_DOWN;
		oldPosX = x; oldPosY = y;
		glutPostRedisplay ();

	} else	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		mousemoded = MOUSE_NOTHING;
		oldPosX = x; oldPosY = y;
		glutPostRedisplay ();

	} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		pick (x,y);
	} else if (button == GLUT_WHEEL_UP) {
		obj.scaleCal (1,1,1);
	} else if (button == GLUT_WHEEL_DOWN) {
		obj.scaleCal (-1, -1, -1);

	}
}

//根据鼠标左右滑动来进行操作
void moveMouse (int x, int y) {
	if (mousemoded == LEFT_DOWN) {
		float dx = x - oldPosX;
		float dy = y - oldPosY;
		obj.rotateCal (0,dy, dx);

		degree += dx;


		oldPosX = x;
		oldPosY = y;

		glutPostRedisplay ();
	}

}

//菜单栏
void menuCtr (int id) {
	string file = "hhh";
	switch (id) {
	case NOTHING:
		break;
	case DISPLAY_MODE_POINT:
		display_moded=GL_POINT;
		break;
	case DISPLAY_MODE_LINE:
		display_moded = GL_LINE;
		break;
	case DISPLAY_MODE_FILL:
		display_moded = GL_FILL;
		break;
	case OPEN_NEW_FILE:
		file = getFilePathName ();
		obj.ReadModel (file);
	default:
		break;
	}
	glutPostRedisplay ();
}

//旋转选择事件
void rotateMenu (int id) {
	switch (id) {
	case 1:
		type = ROTATE_X;
		break;
	case 2:
		type = ROTATE_Y;
		break;
	case 3:
		type = ROTATE_Z;
		break;
	}
}

//创建菜单栏
void createMenu () {
	glutCreateMenu (menuCtr);
	glutAddMenuEntry ("fill", DISPLAY_MODE_FILL);
	glutAddMenuEntry ("mesh", DISPLAY_MODE_LINE);
	glutAddMenuEntry ("point", DISPLAY_MODE_POINT);
	glutAddMenuEntry ("-----------", NOTHING);
	glutAddMenuEntry ("open new file", OPEN_NEW_FILE);
	glutAttachMenu (GLUT_RIGHT_BUTTON);
}

int main (int argc, char** argv) {
	obj.ReadModel ("obj/bunny.obj");
	obj.setScale (10, 10, 10);
	for (bool& i : ok) {
		i = true;
	}
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition (100, 100);
	glutInitWindowSize (width, height);
	glutCreateWindow ("3D模型");
	createMenu ();
	glEnable (GL_NORMALIZE);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_TEXTURE_2D);
	glutDisplayFunc (display);
	//glutIdleFunc (display);
	glutMouseFunc (moveClick);
	glutMotionFunc (moveMouse);
	glutMainLoop ();
	return 0;
}

