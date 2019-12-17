//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#include<GL/glut.h>
#include<math.h>
#include<cstdlib>
#include<iostream>
using namespace std;

/*debug专用宏定义*/
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif // DEBUG


const GLfloat PI = 3.1415926;		//PI
GLfloat rotation = 0;				//总旋转角度，每次time函数都加上rotation_d
GLfloat rotation_d = 1;				//旋转速度，每次time函数把它加到rotation上
inline GLfloat angleToRadian (GLfloat degree) { return degree * PI / 180; }//角度转弧度
inline GLfloat radianToAngle (GLfloat radian) { return radian * 180 / PI; }//弧度转角度

/*输入半径，旋转角，分段数，中点x坐标，中点y坐标*/
void drawCircle (GLfloat r, GLfloat rotation_angle = 0, int segments_num = 100, GLfloat cx = 0, GLfloat cy = 0) {
	dprintf( "draw circle r: %f rotation: %f",r,rotation_angle);

	rotation_angle = angleToRadian (rotation_angle);
	glBegin (GL_LINE_LOOP);
	for (int i = 0; i < segments_num; i++) {
		//通过画点形成loop的方法来画封闭图形
		glVertex2f (
			r * cos ((2 * PI / segments_num) * i+ rotation_angle)+cx,
			r * sin ((2 * PI / segments_num) * i+ rotation_angle)+cy
		);
	}
	glEnd ();
}

/*对画圆的方法进行包装，形成画正方形算法*/
void drawsquare (GLfloat father_r, GLfloat rotation_radian = 0) {
	drawCircle (father_r, rotation_radian, 4);
}

/*主显示函数*/
void display () {
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0f, 1.0f, 1.0f);				//设置绘图颜色

	GLfloat father_r = 1;						//用于保存循环计算中每一轮图形的半径
	GLfloat father_rotation = rotation;			//用于保存循环计算中每一轮图形的旋转角度
	for (int i = 0; i < 15; i++) {
		/*绘制方圆*/
		drawCircle (father_r);
		drawsquare (father_r, father_rotation);

		/*计算下一层方圆的尺寸与旋转角度*/
		father_r = father_r / sqrtf (2);
		father_rotation = father_rotation + rotation;
	}
	glutSwapBuffers (); //交换两个缓冲区的内容
}

/*定时执行的时间控制函数*/
void timeFunc (int value) {
	rotation += rotation_d;			//进行旋转参数的调整
	display ();						//重新绘制图像
	glutTimerFunc (50, &timeFunc, 1);//重新注册时间函数
	return;
}

/*鼠标点击控制回调函数*/
void mouseCtr (int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		printf ("按下左键，逆时针加速 %f\n", rotation_d);
		rotation_d++;
	}else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		rotation_d--;
		printf ("按下右键，顺时针加速 %f\n", rotation_d);
	}
}

/*程序入口，进行opengl初始化与各种控制函数的注册*/
int main (int argc, char* argv[]) {
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition (-1, -1);
	glutInitWindowSize (800, 800);
	glutCreateWindow ("图形学实验一：方圆");
	glutTimerFunc (50, &timeFunc, 1);
	glutMouseFunc (&mouseCtr);
	glutMainLoop ();
	return 0;
}
