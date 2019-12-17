//#include<GL/glut.h>
#include<vector>
#include<algorithm>
#include<iostream>
#include"glut.h"
#pragma comment(lib, "./glut32.lib")

#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
printf
#endif
using namespace std;


/*------------------绘图算法相关-----------------*/
typedef struct point {
	float x;
	float y;
	point (float xi, float yi) :x (xi), y (yi) {}
}point;
bool operator == (point& p1, point p2) {
	return p1.x == p2.x && p1.y == p2.y;
}
typedef vector<point> points;

void drawLine (point& p1, point& p2) {
	glBegin (GL_LINES);
	glLineWidth (4);
	glVertex2f (p1.x, p1.y);
	glVertex2f (p2.x, p2.y);
	glEnd ();
}

void drawBezier (points& input_vertice, int maxn) {
	int k = maxn;
	vector<point> b_spline;
	points control_point=input_vertice;
	int n = control_point.size () - 1;
	//cout <<"control_point_size-1=" << n << endl;

	float* t=new float[100*maxn];
	//均匀B样条
	/*for (int i = 0; i <= input_vertice.size() + k; i++)
		t[i] = i + 1;*/
		//准均匀B样条
	t[0] = 0;
	for (int i = 0; i <= k - 1; i++)
		t[i] = 0;
	for (int i = k; i < n + 1; i++)
		t[i] = t[i - 1] + 1.0 / (n + 1 - k + 1);
	for (int i = n + 1; i <= n + k; i++)
		t[i] = 1;

	for (int j = k - 1; j <= n; j++)//j为区间的末端点
	{
		for (double u = t[j]; u <= t[j + 1]; u += 0.001 / n) {
			for (int r = 1; r <= k - 1; r++)//递推k-1层
			{
				for (int i = j; i >= j - k + r + 1; i--)//由递推公式得，需要倒着来，i和i-1存在i中；；bezier为j和j+1存在j中
				{
					float x1 = u - t[i];
					float x2 = t[i + k - r] - t[i];
					float y1 = t[i + k - r] - u;
					float y2 = t[i + k - r] - t[i];

					float coefficient1, coefficient2;
					if (x1 == 0.0 && x2 == 0.0)
						coefficient1 = 0;
					else
						coefficient1 = x1 / x2;
					if (y1 == 0.0 && y2 == 0.0)
						coefficient2 = 0;
					else
						coefficient2 = y1 / y2;

					if (r == 1)//第一轮必须是输入的那几个控制点
					{
						control_point[i].x = input_vertice[i].x * coefficient1 + input_vertice[i - 1].x * coefficient2;
						control_point[i].y = input_vertice[i].y * coefficient1 + input_vertice[i - 1].y * coefficient2;
						continue;
					} else {
						control_point[i].x = control_point[i].x * coefficient1 + control_point[i - 1].x * coefficient2;
						control_point[i].y = control_point[i].y * coefficient1 + control_point[i - 1].y * coefficient2;
					}
				}
			}
			b_spline.push_back (control_point[j]);//递推的最后一层的点，即为求得的点
		}
	}
	//cout << "BSpline size = :" << b_spline.size() << endl;

	glClear (GL_COLOR_BUFFER_BIT);//清空屏幕上上一次的曲线
	//绘制控制点
	for (int i = 0; i < input_vertice.size (); i++)
		//draw_a_point (input_vertice[i].x, input_vertice[i].y, controlpoint_color);
	//绘制控制多边形
	glBegin (GL_LINE_STRIP);
	glColor3f (1,1,1);
	for (int i = 0; i < input_vertice.size (); i++)
		glVertex2f (input_vertice[i].x, input_vertice[i].y);
	glEnd ();
	glFlush ();

	//画出b_spline曲线
//	glLineWidth (3.0f);
	glBegin (GL_LINE_STRIP);
	glColor3f (1,0,0);
	for (int i = 0; i < b_spline.size (); i++)
		glVertex2f (b_spline[i].x, b_spline[i].y);
	glEnd ();
	glFlush ();
}


/*------------------窗口功能相关-----------------*/
const int SCREENY = 500;
const int SCREENX = 500;
points pointsd = { {250,50},{550,150},{550,400},{250,250},{100,350},{100,100},{120,30} }; //多边形顶点
points temp_poly;
typedef enum state { MOVING_POINT, ADDING_POINT, DRAWING_POLY, DRAWING_DONE } state;
typedef enum addstate { ADD = 9999, INSERT, DELETEP } add;
typedef enum menufunc { CLEAR, INC_SEG, DEC_SEG };
addstate addstated = ADD;
state stated = DRAWING_POLY;
point* chosed_point = nullptr;
int segi = 1;
void segiinc() {
	if (segi >= temp_poly.size ()) {
		cout << "无法继续增加分段\n";
	} else {
		segi++;
	}
}
void segidec () {
	if (segi <= 1) {
		cout << "无法继续减少分段\n";
	} else {
		segi--;
	}
}
void display (void) {
	glClear (GL_COLOR_BUFFER_BIT);

	glColor3f (1, 1, 1);
	int size = temp_poly.size () - 1;
	for (int i = 0; i < size; i++) {
		drawLine (temp_poly[i], temp_poly[i + 1]);
	}

	if (temp_poly.size () >= 2) {
		drawBezier (temp_poly, segi);
	}


	/*画点*/
	glPointSize (5);
	glColor3f (1.0, 1.0, 1.0);
	glBegin (GL_POINTS);
	for (auto i : temp_poly) {
		glVertex2i (i.x, i.y);
	}
	glEnd ();

	glutSwapBuffers ();
}
inline float calDistance (point a1, point a2) {
	return pow ((pow ((a1.x - a2.x), 2) + pow ((a1.y - a2.y), 2)), 0.5);
};
point* getClosetPoint (float xi, float yi) {
	point* temp = nullptr;
	float tempdis = 999999999;
	point click = { (float)xi,yi };
	for (point& i : temp_poly) {
		float temp1 = calDistance (click, i);
		if (temp1 < tempdis) {
			tempdis = temp1;
			temp = &i;
		}
	}
	if (tempdis > 60.0) {
		dprintf ("未选中合适的点\n");
		return nullptr;
	}
	dprintf ("选中了点：(%f, %f)，于位置：%p\n", temp->x, temp->y, temp);
	return temp;
}
void insertPoint (int x, int y) {
	point ti = { (float)x,(float)y };
	point* t1 = getClosetPoint (x, y);
	if (t1 == nullptr) {
		printf ("无法找到合适的插入点\n");
	} else {

		temp_poly.insert (find (temp_poly.begin (), temp_poly.end (), *t1), ti);
	}

}

/*鼠标按键回调*/
void mouseButton (int button, int state, int xi, int yi) {
	float x = xi;
	float y = SCREENY - yi;
	point temp_point = { x,y };

	if (button == GLUT_WHEEL_DOWN) {
		segidec();
	}
	if (button == GLUT_WHEEL_UP) {
		segiinc ();
	}

	switch (stated) {
	case MOVING_POINT:
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
			if (stated == MOVING_POINT) {
				stated = DRAWING_POLY;
				dprintf ("位置移动结束，当前点新位置(%f, %f)\n", chosed_point->x, chosed_point->y);
			}
		}
		break;
	case ADDING_POINT:
		break;
	case DRAWING_POLY:
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			switch (addstated) {
			case ADD:
				dprintf ("添加点(%f, %f)\n", temp_point.x, temp_point.y);
				temp_poly.emplace_back (temp_point);
				break;
			case INSERT:
				if (temp_poly.size () < 2) {
					printf ("抱歉，当前点数目小于2，无法插入，请您添加更多点，默认选择添加点\n");
					dprintf ("添加点(%f, %f)\n", temp_point.x, temp_point.y);
					temp_poly.emplace_back (temp_point);
				} else {
					dprintf ("插入点(%f, %f)\n", temp_point.x, temp_point.y);
					insertPoint (temp_point.x, temp_point.y);
				}
				break;
			case DELETEP:
				if (temp_poly.size () < 2) {
					printf ("抱歉，当前点数目小于1，无法删除，请您添加更多点\n");

				} else {
					point* to_be_delete = getClosetPoint (temp_point.x, temp_point.y);
					if (to_be_delete == nullptr) {
						printf ("抱歉，未选中任何点，无法删除\n");
					} else {
						dprintf ("删除点(%f, %f)\n", temp_point.x, temp_point.y);
						temp_poly.erase (find (temp_poly.begin (), temp_poly.end (), *to_be_delete));

					}
				}
				break;
			default:
				break;
			}

		}
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
			chosed_point = getClosetPoint (x, y);
			if (chosed_point == nullptr) {
				stated = DRAWING_POLY;
				dprintf ("未选中合适的点\n");
			} else {
				stated = MOVING_POINT;
				dprintf ("位置移动开始，当前点位置(%f, %f)\n", chosed_point->x, chosed_point->y);
			}
		}
		break;
	case DRAWING_DONE:

		break;
	default:
		break;
	}

	glutPostRedisplay ();


}


/*鼠标移动回调*/
void mouseMove (int xi, int yi) {
	float x = xi;
	float y = SCREENY - yi;
	switch (stated) {
	case MOVING_POINT:
		dprintf ("当前点位置(%f, %f)， ", chosed_point->x, chosed_point->y);
		chosed_point->x = x;
		chosed_point->y = y;
		dprintf ("当前点新位置(%f, %f)\n", chosed_point->x, chosed_point->y);
		break;
	case ADDING_POINT:
		break;
	case DRAWING_POLY:
		break;
	case DRAWING_DONE:

		break;
	default:
		break;
	}

	glutPostRedisplay ();

}

void menuCtr (int funci) {
	switch (funci) {
	case CLEAR:
		//清空所有多边形
		temp_poly.clear ();
		dprintf ("清空所有记录\n");
		break;

	case DEC_SEG:
		segidec ();
		break;

	case INC_SEG:
		segiinc ();

		break;
	case ADD:
		cout << "切换到添加点模式\n";
		addstated = ADD;
		break;

	case INSERT:
		cout << "切换到插入点模式\n";
		addstated = INSERT;
		break;

	case DELETEP:
		cout << "切换到删除点模式\n";
		addstated = DELETEP;
		break;
	default:
		break;
	}
	glutPostRedisplay ();
}
void keyPress (unsigned char key, int x, int y) {
	switch (key) {
	case '+':
		segiinc ();
		break;

	case '-':
		segidec();

		break;
	default:
		break;
	}
	glutPostRedisplay ();

}

int main (int argc, char** argv) {
	stated = DRAWING_POLY;
	glutInit (&argc, argv);							//初始化
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	//设置显示模式
	glutInitWindowPosition (600, 100);				//设置窗口的顶部和左边位置
	glutInitWindowSize (SCREENX, SCREENY);					//设置窗口的高度和宽度
	glutCreateWindow ("实验08：交互Bezier曲线");	//设置窗口标题
	glClearColor (0, 0, 0, 0);							//设置窗口背景颜色
	glMatrixMode (GL_PROJECTION);					//设置窗口视图
	gluOrtho2D (0, SCREENX, 0, SCREENY);
	glutDisplayFunc (display);						//注册显示函数

	/*注册各种自定义函数*/
	//glutTimerFunc (50, &_timeCtr, 1);
	glutMouseFunc (mouseButton);
	glutMotionFunc (mouseMove);
	glutKeyboardFunc (keyPress);

	/*注册右键菜单栏*/
	glutCreateMenu (menuCtr);
	glutAddMenuEntry ("clear", CLEAR);
	glutAddMenuEntry ("inc seg num", INC_SEG);
	glutAddMenuEntry ("dec seg num", DEC_SEG);
	glutAddMenuEntry ("add", ADD);
	glutAddMenuEntry ("insert", INSERT);
	glutAddMenuEntry ("delete", DELETEP);
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	glutMainLoop ();								//开始主循环
	return 0;
}