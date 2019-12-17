//
//#include <iostream>
//#include <stdlib.h>
//#include<math.h>
//#include "glut.h"
//using namespace std;
//int SCREEN_WIDTH = 400; //屏幕尺寸
//int SCREEN_HEIGHT = 400;
//int posx1 = 150;
//int posy1 = 200;
//int posx2 = 250;
//int posy2 = 200;
//int select_point = 0; //1 是第一个点，2是第二个，以此类推
//
//void init () {
//	glClearColor (1, 1, 1, 1);
//}
//
//void draw (GLenum mode) {
//	glPointSize (10.0f);
//	const static GLfloat color_selected[] = { 1.0f,0.0f,0.0f };
//	const static GLfloat color_unselected[] = { 0.0f,0.0f,1.0f };
//
//	for (int i = 1; i <= 5; i++)//5个点
//	{
//		if (mode == GL_SELECT)
//			glLoadName (i);
//		glColor3fv ((select_point == i) ? color_selected : color_unselected);
//		glBegin (GL_POINTS);
//		glVertex2f (i * 70, 200);
//		glEnd ();
//	}
//}
//
//void display () {
//	glClear (GL_COLOR_BUFFER_BIT);
//	draw (GL_RENDER); //设置渲染模式
//	glFlush ();
//}
//
//void reshape (int width, int height) {
//	glViewport (0, 0, width, height);
//	glMatrixMode (GL_PROJECTION);
//	glLoadIdentity ();
//	gluOrtho2D (0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
//	glMatrixMode (GL_MODELVIEW);
//
//}
//
//void processHits (GLint hits, GLuint buffer[]) {
//	unsigned int i;
//	GLuint name;
//	for (i = 0; i < hits; i++) {
//		name = buffer[3 + i * 4];
//		select_point = name;//每个选中物体有占用名字栈中4个单位，第4个是物体名字值
//		cout << "第" << name << "个点" << endl;
//	}
//}
//
//#define SIZE 500
//
//#define N 10
//
//void mouse (int button, int state, int x, int y) {
//	GLuint selectBuffer[SIZE]; //存放物体名称的栈
//	GLint hits; //存放被选中对象个数
//	GLint viewport[4]; //存放可视区参数
//	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) //当鼠标左键按下时
//	{
//		glGetIntegerv (GL_VIEWPORT, viewport); //获取当前视口坐标参数
//		glSelectBuffer (SIZE, selectBuffer); //选择名称栈存放被选中的名称
//		glRenderMode (GL_SELECT); //设置当前为选择模式
//		glInitNames (); //初始化名称栈
//		glPushName (0);
//		glMatrixMode (GL_PROJECTION);
//		glPushMatrix ();
//		glLoadIdentity ();
//		gluPickMatrix (x, viewport[3] - y, N, N, viewport); //创建用于选择的投影矩阵栈
//		gluOrtho2D (0, SCREEN_WIDTH, 0, SCREEN_HEIGHT); //设置投影方式
//		draw (GL_SELECT); //绘制场景,选择模式
//		glPopMatrix ();
//
//		glFlush ();
//		hits = glRenderMode (GL_RENDER);
//		glMatrixMode (GL_MODELVIEW);
//
//		if (hits > 0)
//			processHits (hits, selectBuffer);
//		glutPostRedisplay ();
//	}
//
//	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) //当鼠标左键抬起时
//	{
//		select_point = 0;
//		glRenderMode (GL_RENDER);
//		draw (GL_RENDER);
//		glutPostRedisplay ();
//	}
//
//}
//
//int main (int argc, char* argv[]) {
//	glutInit (&argc, argv);
//	glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH);
//	glutInitWindowSize (SCREEN_WIDTH, SCREEN_HEIGHT);
//	glutInitWindowPosition (100, 100);
//	glutCreateWindow ("图形拾取编程实例");
//	init ();
//	glutDisplayFunc (display);
//	glutReshapeFunc (reshape);
//	glutMouseFunc (mouse);
//	glutMainLoop ();
//	return 1;
//}

/*
代码中的Gvector,Gvector3等等相关的类是我自己
实现的向量操作类。运行该程序时请大家自己对该
程序修改一下，用自己的向量类或就是数组表示
*/

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<gl/GLU.h>
#include<gl/GL.h>
#include "glut.h"

int WIDTH = 800;
int HEIGHT = 600;

//定义包围球的结构体
struct boundingsphere {
	GVector c; //球心在世界坐标中的位置
	double r;  //半径
};

//定义射线结构体
struct Ray {
	GVector pos; //射线起始位置
	GVector dir; //射线方向
} PickingRay; // 定义一个射线


boundingsphere b = { GVector (4), 1.5 }; //定义一个包围球,半径为1.4


//屏幕坐标到投影后坐标的转换
GVector3 screen2viewport (int x, int y, int w, int h) {
	double _x = 2.0 * x / w - 1;
	double _y = 1 - 2.0 * y / h;
	return GVector3 (_x, _y, 1.0);
}


//投影坐标到相机坐标的转换
GVector3 viewport2viewspace (GVector3 vp) {
	GLdouble m[16];
	glGetDoublev (GL_PROJECTION_MATRIX, m);

	double _x = vp[0] / m[0];
	double _y = vp[1] / m[5];

	return GVector3 (_x, _y, -1.0);
}

//生成拾取射线
Ray computeRay (GVector3 s, GVector3 e) {
	Ray ray;
	ray.dir = GVector (4, e[0] - s[0], e[1] - s[1], e[2], -s[2], 0.0).Normalize ();
	ray.pos = GVector (4, s[0], s[1], s[2], 1.0);
	return ray;
}

//将拾取射线从相机坐标转换到世界坐标
void ray2world (Ray* ray) {
	// 这里最后一列为(0,0,5,1)是因为相机的位置在世界坐标的(0,0,5)这个地方
	double m[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 5,
		0, 0, 0, 1
	};

	GMatrix M (4, 4, m);

	ray->pos = M * ray->pos; //矩阵乘以点 转换射线起始坐标到世界坐标
	ray->dir = M * ray->dir; //矩阵乘以向量  转换射线方向到世界坐标

}


//判断是否相交
bool intersect (Ray r, boundingsphere b) {
	double A = r.dir * r.dir;
	double B = 2 * r.dir * (r.pos - b.c);
	double C = (r.pos - b.c) * (r.pos - b.c) - b.r * b.r;

	//根号内部分为0,那么肯定无实数解,说明射线和球不相交
	if (B * B - 4 * C < 0)
		return false;

	double x1 = (-B + sqrt (B * B - 4 * C)) / (2.0);
	double x2 = (-B - sqrt (B * B - 4 * C)) / (2.0);

	//如果有解,且有一个大于0,说明射线和球相交,小于0的话说明射线的反方向和球相交
	if (x1 >= 0 || x2 >= 0)
		return true;

	return false;
}

void glinit () {
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);

}

void render () {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor (1.0, 1.0, 1.0, 1.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	//设置相机的位置
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	float m[16];

	GLfloat diff[] = { 1.0,.0,.0,0.7 };
	GLfloat amb[] = { 1.0,.0,.0,0.6 };
	GLfloat sp[] = { 1.0,1.0,1.0,0.5 };
	glMaterialfv (GL_FRONT, GL_DIFFUSE, diff);
	glMaterialfv (GL_FRONT, GL_AMBIENT, amb);
	glMaterialfv (GL_FRONT, GL_SPECULAR, sp);
	glMateriali (GL_FRONT, GL_SHININESS, 64);

	static float a = 0.0;
	static float r = 0.0;

	//将物体和包围球一起移动
	glTranslatef (r * cosf (a), r * sinf (a), -8.0);
	b.c.Set (r * cosf (a), r * sinf (a), -8.0, 1.0);
	glutSolidTeapot (1.0);

	GLfloat d[] = { 1.0,1.0,0.0,0.3 };
	glMaterialfv (GL_FRONT, GL_DIFFUSE, d);


	//渲染包围球
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutSolidSphere (b.r, 20, 20);
	glDisable (GL_BLEND);

	glGetFloatv (GL_MODELVIEW_MATRIX, m);

	glMatrixMode (GL_PROJECTION);
	glGetFloatv (GL_PROJECTION_MATRIX, m);

	a += 0.02;

	r += 0.005;
	if (r >= 5) r = 0;
	if (a >= 3.14 * 2) a = 0;

	glutSwapBuffers ();
	glutPostRedisplay ();
}

void resize (int w, int h) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (45, (double)WIDTH / HEIGHT, 0.1, 1000);
	glViewport (0, 0, w, h);
	glMatrixMode (GL_MODELVIEW);
	WIDTH = w;
	HEIGHT = h;
}



void mouseDown (int btn, int state, int x, int y) {
	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		//屏幕坐标转换为投影后坐标,保存在v1中
		GVector3 v1 = screen2viewport (x, y, WIDTH, HEIGHT);

		//将投影坐标v1转换为相机坐标,保存在v2中
		GVector3 v2 = viewport2viewspace (v1);

		//利用相机坐标生成拾取射线
		PickingRay = computeRay (GVector3 (.0, .0, .0), v2);

		//将射线转换到世界坐标
		ray2world (&PickingRay);

		//判断是否和包围球相交
		bool is = intersect (PickingRay, b);
		if (is)
			MessageBox (0, "Picked!", "message", 0);
	}
}

int main (int argc, char** argv) {
	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize (WIDTH, HEIGHT);
	glutCreateWindow ("PickingRay");

	glinit ();

	glutMouseFunc (mouseDown);
	glutDisplayFunc (render);
	glutReshapeFunc (resize);

	glutMainLoop ();

	return 0;
}