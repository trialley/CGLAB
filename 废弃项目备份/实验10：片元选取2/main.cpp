#pragma comment(lib, "./glut32.lib")

#define GLEW_STATIC
#define GLUT_DISABLE_ATEXIT_HACK
#include <iostream>
#include <GL/glew.h> 
#include "glut.h"
using namespace std;
#pragma warning(disable:4996)
#define BUFSIZE 512
GLfloat scale = 1.0;
static float M_PI = 3.1415926;
static float c = M_PI / 180.0f; //弧度和角度转换参数
static int du = 80, oldmy = -1, oldmx = -1; //du是视点绕y轴的角度,opengl里默认y轴是上方向
static float r = 5.0f, h = 2.0f; //r是视点绕y轴的半径,h是视点高度即在y轴上的坐标
GLfloat normal[100][3];
GLfloat vertex[100][3];
int triangle_num = 0, normal_num = 0;

void readStl () {
	//读stl文件并显示
	FILE* fp = fopen ("next_prueba.stl", "r");
	if (fp != nullptr) {
		char str[100], test[100];

		//GLfloat color_v = 0.1;
		while (1) {
			fscanf (fp, "%s", &str);
			if (!strcmp (str, "facet")) {
				fscanf (fp, "%s", &test);
				fscanf (fp, "%f %f %f\n", &normal[normal_num][0], &normal[normal_num][1], &normal[normal_num][2]);
				normal_num++;
				//glNormal3fv(normal[triangle_num]);
				continue;
			} else if (!strcmp (str, "outer")) {
				fscanf (fp, "%s", &test);
				for (int i = 0; i < 3; i++) {
					fscanf (fp, "%s", &test);
					fscanf (fp, "%f %f %f\n", &vertex[triangle_num][0], &vertex[triangle_num][1], &vertex[triangle_num][2]);
					//cout << vertex[0] << " " << vertex[1] << " " << vertex[2] << endl;
					triangle_num++;
				}
				continue;
			} else if (!strcmp (str, "endsolid")) {
				break;
			} else {
				continue;
			}
		}
	}
	fclose (fp);
}
//设置光照
void setLightRes () {
	glEnable (GL_LIGHTING); // 启用光照
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };//光源位置
	GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };//rgb下的环境光颜色
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//漫反射光的颜色
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//镜面光的颜色
	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
	glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable (GL_LIGHT0); // 启用指定光源

	//glEnable(GL_LIGHTING);
	//GLfloat light0Position[] = { 0, 1, 0, 0 };//光源位置
	//GLfloat env_ambient[] = { 0.5,0.5,0.5 };//环境光颜色
	//GLfloat diffuseLight0[] = { 1,1,1 };//漫反射光的颜色
	//GLfloat specularlight0[] = { 1,1,1 };//镜面光的颜色
	//glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, env_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularlight0);//设置镜面光成分
}

float color[100] = { 0 };
void drawStl (GLenum mode) {
	int i = 0, j = 0;
	for (j = 0, i = 0; j < normal_num; j++) {
		if (color[i] == 1)continue;
		glNormal3fv (normal[j]);
		if (mode == GL_SELECT)
			glLoadName (j + 1);
		glBegin (GL_TRIANGLES);
		glColor3d (color[i], 1.0f, 1.0f);
		glVertex3f (vertex[i][0] * scale, vertex[i][1] * scale, vertex[i][2] * scale);
		i++;
		glVertex3f (vertex[i][0] * scale, vertex[i][1] * scale, vertex[i][2] * scale);
		i++;
		glVertex3f (vertex[i][0] * scale, vertex[i][1] * scale, vertex[i][2] * scale);
		i++;
		glEnd ();
	}
}
void display (void) {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glPolygonMode (GL_FRONT, GL_FILL);
	setLightRes ();
	gluLookAt (r * cos (c * du), h, r * sin (c * du), 0, 0, 0, 0, 1, 0); //从视点看远点,y轴方向(0,1,0)是上方向
	drawStl (GL_RENDER);

	glFlush ();
	glutSwapBuffers ();
}
void processHits (GLint hits, GLuint buffer[]) {
	for (float& i : color) {
		i = 0;
	}
	GLint i;
	GLuint j;
	GLuint names, * ptr;
	printf ("hit=%d\n", hits);
	ptr = (GLuint*)buffer;
	for (i = 0; i < hits; i++) {
		names = *ptr;
		//printf("number of names for this hit =%d\n", names);

		ptr++;
		//printf("z1 is %g;\n", (float)*ptr / 0x7fffffff);//selectBuffer缓冲中保存了名字栈中名字的个数，选择到的物体的最小最大深度值
		ptr++;
		//printf("z2 is %g;\n", (float)*ptr / 0x7fffffff);//我们用z1变量显示出物体最小的深度值，z2变量显示出物体最大的深度值，
		//这个值是0~1的值，离我们最近的为0最远的为1，但是selectbuffer是整形的数组，因此保存的时候乘以了0x7fffffff，因此显示的时候也要除以该值

		ptr++;
		printf ("names are ");

		for (j = 0; j < names; j++) {
			color[*ptr] = 1;
			printf ("%d", *ptr);
			ptr++;
		}
		printf ("\n");
	}
}

void pick (int x, int y) {
	oldmx = x, oldmy = y;
	GLuint selectBuff[BUFSIZE];
	GLint hits, viewport[4];

	glGetIntegerv (GL_VIEWPORT, viewport); //获得viewport
	glSelectBuffer (BUFSIZE, selectBuff); //告诉OpenGL初始化selectbuffer 
	(void)glRenderMode (GL_SELECT); //进入选择模式
	glInitNames ();//初始化名字栈
	glPushName (0);//在名字栈中放入一个初始化名字

	glMatrixMode (GL_PROJECTION);//进入投影阶段准备拾取
	glPushMatrix ();//保存以前的投影矩阵
	glLoadIdentity ();//载入单位矩阵
	gluPickMatrix ((GLdouble)x, (GLdouble)(viewport[3] - y), .1, .1, viewport);
	//影响画框的容量
	//投影变换为正投影
	gluPerspective (50.0f, 1.0f, 0.1f, 1000.0f);

	drawStl (GL_SELECT);
	//display();
	glPopMatrix ();
	glFlush ();
	glutSwapBuffers ();
	hits = glRenderMode (GL_RENDER);
	// 从选择模式返回正常模式,该函数返回选择到对象的个数
	processHits (hits, selectBuff);
}
void Mouse (int button, int state, int x, int y) //处理鼠标点击
{
	//printf("button: %d state: %d\n", button, state);
	if (button == GLUT_LEFT_BUTTON) {
		oldmx = x, oldmy = y;
		GLuint selectBuff[BUFSIZE];
		GLint hits, viewport[4];
		if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
			return;

		pick (x,y);
	} else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
		oldmx = x, oldmy = y;
		scale = (scale == 1.0f ? 0.5f : 1.0f);
		printf ("scale: %f\n", scale);
	}
}
void onMouseMove (int x, int y) //处理鼠标拖动
{
	du += x - oldmx; //鼠标在窗口x轴方向上的增量加到视点绕y轴的角度上，这样就左右转了
	h += 0.03f * (y - oldmy); //鼠标在窗口y轴方向上的改变加到视点的y坐标上，就上下转了
	if (h > 10.0f) h = 2.0f; //视点y坐标作一些限制，不会使视点太奇怪
	else if (h < -10.0f) h = -2.0f;
	oldmx = x, oldmy = y; //把此时的鼠标坐标作为旧值，为下一次计算增量做准备

}
void init () {
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_FLAT);
}
void reshape (int w, int h) {
	glViewport (0, 0, (GLsizei)w, (GLsizei)w);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective (75.0f, (float)w / h, 1.0f, 1000.0f);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

}
int main (int argc, char* argv[]) {
	readStl ();
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition (100, 100);
	glutInitWindowSize (400, 400);
	glutCreateWindow ("OpenGL_task1_picking");
	init ();

	glutMouseFunc (Mouse);
	glutMotionFunc (onMouseMove);
	glutReshapeFunc (reshape);
	glutDisplayFunc (display);//显示回调
	glutIdleFunc (display);
	glutMainLoop ();

	return 0;
}

