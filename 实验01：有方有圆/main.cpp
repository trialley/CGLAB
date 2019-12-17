//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include<GL/glut.h>
#include<math.h>
#include<cstdlib>
#include<iostream>
using namespace std;

/*debugר�ú궨��*/
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif // DEBUG


const GLfloat PI = 3.1415926;		//PI
GLfloat rotation = 0;				//����ת�Ƕȣ�ÿ��time����������rotation_d
GLfloat rotation_d = 1;				//��ת�ٶȣ�ÿ��time���������ӵ�rotation��
inline GLfloat angleToRadian (GLfloat degree) { return degree * PI / 180; }//�Ƕ�ת����
inline GLfloat radianToAngle (GLfloat radian) { return radian * 180 / PI; }//����ת�Ƕ�

/*����뾶����ת�ǣ��ֶ������е�x���꣬�е�y����*/
void drawCircle (GLfloat r, GLfloat rotation_angle = 0, int segments_num = 100, GLfloat cx = 0, GLfloat cy = 0) {
	dprintf( "draw circle r: %f rotation: %f",r,rotation_angle);

	rotation_angle = angleToRadian (rotation_angle);
	glBegin (GL_LINE_LOOP);
	for (int i = 0; i < segments_num; i++) {
		//ͨ�������γ�loop�ķ����������ͼ��
		glVertex2f (
			r * cos ((2 * PI / segments_num) * i+ rotation_angle)+cx,
			r * sin ((2 * PI / segments_num) * i+ rotation_angle)+cy
		);
	}
	glEnd ();
}

/*�Ի�Բ�ķ������а�װ���γɻ��������㷨*/
void drawsquare (GLfloat father_r, GLfloat rotation_radian = 0) {
	drawCircle (father_r, rotation_radian, 4);
}

/*����ʾ����*/
void display () {
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0f, 1.0f, 1.0f);				//���û�ͼ��ɫ

	GLfloat father_r = 1;						//���ڱ���ѭ��������ÿһ��ͼ�εİ뾶
	GLfloat father_rotation = rotation;			//���ڱ���ѭ��������ÿһ��ͼ�ε���ת�Ƕ�
	for (int i = 0; i < 15; i++) {
		/*���Ʒ�Բ*/
		drawCircle (father_r);
		drawsquare (father_r, father_rotation);

		/*������һ�㷽Բ�ĳߴ�����ת�Ƕ�*/
		father_r = father_r / sqrtf (2);
		father_rotation = father_rotation + rotation;
	}
	glutSwapBuffers (); //��������������������
}

/*��ʱִ�е�ʱ����ƺ���*/
void timeFunc (int value) {
	rotation += rotation_d;			//������ת�����ĵ���
	display ();						//���»���ͼ��
	glutTimerFunc (50, &timeFunc, 1);//����ע��ʱ�亯��
	return;
}

/*��������ƻص�����*/
void mouseCtr (int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		printf ("�����������ʱ����� %f\n", rotation_d);
		rotation_d++;
	}else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		rotation_d--;
		printf ("�����Ҽ���˳ʱ����� %f\n", rotation_d);
	}
}

/*������ڣ�����opengl��ʼ������ֿ��ƺ�����ע��*/
int main (int argc, char* argv[]) {
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition (-1, -1);
	glutInitWindowSize (800, 800);
	glutCreateWindow ("ͼ��ѧʵ��һ����Բ");
	glutTimerFunc (50, &timeFunc, 1);
	glutMouseFunc (&mouseCtr);
	glutMainLoop ();
	return 0;
}
