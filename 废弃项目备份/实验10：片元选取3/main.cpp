//
//#include <iostream>
//#include <stdlib.h>
//#include<math.h>
//#include "glut.h"
//using namespace std;
//int SCREEN_WIDTH = 400; //��Ļ�ߴ�
//int SCREEN_HEIGHT = 400;
//int posx1 = 150;
//int posy1 = 200;
//int posx2 = 250;
//int posy2 = 200;
//int select_point = 0; //1 �ǵ�һ���㣬2�ǵڶ������Դ�����
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
//	for (int i = 1; i <= 5; i++)//5����
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
//	draw (GL_RENDER); //������Ⱦģʽ
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
//		select_point = name;//ÿ��ѡ��������ռ������ջ��4����λ����4������������ֵ
//		cout << "��" << name << "����" << endl;
//	}
//}
//
//#define SIZE 500
//
//#define N 10
//
//void mouse (int button, int state, int x, int y) {
//	GLuint selectBuffer[SIZE]; //����������Ƶ�ջ
//	GLint hits; //��ű�ѡ�ж������
//	GLint viewport[4]; //��ſ���������
//	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON) //������������ʱ
//	{
//		glGetIntegerv (GL_VIEWPORT, viewport); //��ȡ��ǰ�ӿ��������
//		glSelectBuffer (SIZE, selectBuffer); //ѡ������ջ��ű�ѡ�е�����
//		glRenderMode (GL_SELECT); //���õ�ǰΪѡ��ģʽ
//		glInitNames (); //��ʼ������ջ
//		glPushName (0);
//		glMatrixMode (GL_PROJECTION);
//		glPushMatrix ();
//		glLoadIdentity ();
//		gluPickMatrix (x, viewport[3] - y, N, N, viewport); //��������ѡ���ͶӰ����ջ
//		gluOrtho2D (0, SCREEN_WIDTH, 0, SCREEN_HEIGHT); //����ͶӰ��ʽ
//		draw (GL_SELECT); //���Ƴ���,ѡ��ģʽ
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
//	if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) //��������̧��ʱ
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
//	glutCreateWindow ("ͼ��ʰȡ���ʵ��");
//	init ();
//	glutDisplayFunc (display);
//	glutReshapeFunc (reshape);
//	glutMouseFunc (mouse);
//	glutMainLoop ();
//	return 1;
//}

/*
�����е�Gvector,Gvector3�ȵ���ص��������Լ�
ʵ�ֵ����������ࡣ���иó���ʱ�����Լ��Ը�
�����޸�һ�£����Լ������������������ʾ
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

//�����Χ��Ľṹ��
struct boundingsphere {
	GVector c; //���������������е�λ��
	double r;  //�뾶
};

//�������߽ṹ��
struct Ray {
	GVector pos; //������ʼλ��
	GVector dir; //���߷���
} PickingRay; // ����һ������


boundingsphere b = { GVector (4), 1.5 }; //����һ����Χ��,�뾶Ϊ1.4


//��Ļ���굽ͶӰ�������ת��
GVector3 screen2viewport (int x, int y, int w, int h) {
	double _x = 2.0 * x / w - 1;
	double _y = 1 - 2.0 * y / h;
	return GVector3 (_x, _y, 1.0);
}


//ͶӰ���굽��������ת��
GVector3 viewport2viewspace (GVector3 vp) {
	GLdouble m[16];
	glGetDoublev (GL_PROJECTION_MATRIX, m);

	double _x = vp[0] / m[0];
	double _y = vp[1] / m[5];

	return GVector3 (_x, _y, -1.0);
}

//����ʰȡ����
Ray computeRay (GVector3 s, GVector3 e) {
	Ray ray;
	ray.dir = GVector (4, e[0] - s[0], e[1] - s[1], e[2], -s[2], 0.0).Normalize ();
	ray.pos = GVector (4, s[0], s[1], s[2], 1.0);
	return ray;
}

//��ʰȡ���ߴ��������ת������������
void ray2world (Ray* ray) {
	// �������һ��Ϊ(0,0,5,1)����Ϊ�����λ�������������(0,0,5)����ط�
	double m[16] =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 5,
		0, 0, 0, 1
	};

	GMatrix M (4, 4, m);

	ray->pos = M * ray->pos; //������Ե� ת��������ʼ���굽��������
	ray->dir = M * ray->dir; //�����������  ת�����߷�����������

}


//�ж��Ƿ��ཻ
bool intersect (Ray r, boundingsphere b) {
	double A = r.dir * r.dir;
	double B = 2 * r.dir * (r.pos - b.c);
	double C = (r.pos - b.c) * (r.pos - b.c) - b.r * b.r;

	//�����ڲ���Ϊ0,��ô�϶���ʵ����,˵�����ߺ����ཻ
	if (B * B - 4 * C < 0)
		return false;

	double x1 = (-B + sqrt (B * B - 4 * C)) / (2.0);
	double x2 = (-B - sqrt (B * B - 4 * C)) / (2.0);

	//����н�,����һ������0,˵�����ߺ����ཻ,С��0�Ļ�˵�����ߵķ���������ཻ
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

	//���������λ��
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

	//������Ͱ�Χ��һ���ƶ�
	glTranslatef (r * cosf (a), r * sinf (a), -8.0);
	b.c.Set (r * cosf (a), r * sinf (a), -8.0, 1.0);
	glutSolidTeapot (1.0);

	GLfloat d[] = { 1.0,1.0,0.0,0.3 };
	glMaterialfv (GL_FRONT, GL_DIFFUSE, d);


	//��Ⱦ��Χ��
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
		//��Ļ����ת��ΪͶӰ������,������v1��
		GVector3 v1 = screen2viewport (x, y, WIDTH, HEIGHT);

		//��ͶӰ����v1ת��Ϊ�������,������v2��
		GVector3 v2 = viewport2viewspace (v1);

		//���������������ʰȡ����
		PickingRay = computeRay (GVector3 (.0, .0, .0), v2);

		//������ת������������
		ray2world (&PickingRay);

		//�ж��Ƿ�Ͱ�Χ���ཻ
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