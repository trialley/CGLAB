#include<GL/glut.h>
#include<vector>
#include<algorithm>
#include<iostream>
#include<set>
using namespace std;
#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif
using namespace std;


/*------------------绘图算法相关-----------------*/
typedef struct point {
	float x;
	float y;
}point;
bool operator == (point& p1, point p2) {
	return p1.x == p2.x && p1.y == p2.y;
}
typedef point Edge[2];
typedef vector<point> points;
typedef vector<points> polys;
typedef struct rectangle {
	int top;
	int bottom;
	int left;
	int right;
	rectangle () {}
	rectangle (int ti, int bi, int li, int ri) {

	}
}rectangle;

void drawLine (point& p1, point& p2) {
	glBegin (GL_LINES);
	glLineWidth (4);
	glVertex2f (p1.x, p1.y);
	glVertex2f (p2.x, p2.y);
	glEnd ();
}

/*
分而治之的思想
一条线裁剪，如何连接新点与旧点？
*/
void clipEdge (points Inpoints, points& output_points, Edge clip_line) {
	auto isInside = [](point& Testp2t, Edge clip_line)->bool {
		if (clip_line[1].x > clip_line[0].x) {//裁剪边为窗口下边
			if (Testp2t.y >= clip_line[0].y)
				return true;
		} else if (clip_line[1].x < clip_line[0].x) {//裁剪边为窗口上边
			if (Testp2t.y <= clip_line[0].y)
				return true;
		} else if (clip_line[1].y > clip_line[0].y) {
			//裁剪边为窗口右边
			if (Testp2t.x <= clip_line[0].x)
				return true;
		} else if (clip_line[1].y < clip_line[0].y) {
			//裁剪边为窗口左边
			if (Testp2t.x >= clip_line[0].x)
				return true;
		} else {
			return  false;
		}
	};
	//直线段p1p2和窗口边界求交，返回交点；类似LB裁剪算法
	auto getInterPoint = [](point& p1, point& p2, Edge clip_line)->point {
		point inter_point;
		if (clip_line[0].y == clip_line[1].y) {//水平裁剪边
			inter_point.y = clip_line[0].y;
			inter_point.x = p1.x + (clip_line[0].y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
		} else {//垂直裁剪边
			inter_point.x = clip_line[0].x;
			inter_point.y = p1.y + (clip_line[0].x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
		}
		return inter_point;
	};
	point p1, p2, inter_point;
	int j;

	p1 = Inpoints[Inpoints.size() - 1];//p1是最末一个点

	for (point p2:Inpoints) {
		if (isInside (p2, clip_line)) {
			if (isInside (p1, clip_line)) {//头尾都内，尾入形，引因为头必已在形 内
				output_points.push_back (p2); 
			}else { //头外尾内，交尾入形 入
				output_points.push_back(getInterPoint (p1, p2, clip_line));
				output_points.push_back(p2);
			}
		} else if (isInside (p1, clip_line)) { //头内尾外，交入队 出
			output_points.push_back(getInterPoint (p1, p2, clip_line));
		} //头尾均外，没有点入形
		p1 = p2;
	}
}

void shClip (points& Inpoints, int Inlength, rectangle rect) {
	//首先绘制原图形
	glColor3f (1, 1, 1);
	for (int i = 0; i < Inpoints.size () - 1; i++) {
		drawLine (Inpoints[i], Inpoints[i + 1]);
	}
	drawLine (Inpoints[Inpoints.size () - 1], Inpoints[0]);
	Edge clip_line;
	points output_points;
	int Outlength = 0;

	//使用窗口下边界进行切割
	clip_line[0].x = rect.left;
	clip_line[0].y = rect.bottom;
	clip_line[1].x = rect.right;
	clip_line[1].y = rect.bottom;
	clipEdge (Inpoints, output_points, clip_line);
	//然后使用窗口shang边界进行切割
	points output_points1;
	int Outlength1 = 0;
	clip_line[0].x = rect.right;
	clip_line[0].y = rect.top;
	clip_line[1].x = rect.left;
	clip_line[1].y = rect.top;
	clipEdge (output_points, output_points1, clip_line);
	//然后使用窗口左边界进行切割
	points output_points2;
	int Outlength2 = 0;
	clip_line[0].x = rect.left;
	clip_line[0].y = rect.top;
	clip_line[1].x = rect.left;
	clip_line[1].y = rect.bottom;
	clipEdge (output_points1, output_points2, clip_line);
	//然后使用窗口右边界进行切割
	points output_points3;
	int Outlength3 = 0;
	clip_line[0].x = rect.right;
	clip_line[0].y = rect.bottom;
	clip_line[1].x = rect.right;
	clip_line[1].y = rect.top;
	clipEdge (output_points2, output_points3, clip_line);

	set<point> drawed;
	glColor3f (1, 0, 0);
	for (int i = 0; i < output_points3.size() - 1; i++) {
		drawLine (output_points3[i], output_points3[i + 1]);
	}
	drawLine (output_points3[output_points3.size() - 1], output_points3[0]);

	glutPostRedisplay ();
}

/*------------------窗口功能相关-----------------*/
rectangle rec;
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
void display (void) {
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1, 1, 1);
	glBegin (GL_LINE_LOOP);
	glVertex2f (rec.left, rec.bottom);
	glVertex2f (rec.right, rec.bottom);
	glVertex2f (rec.right, rec.top);
	glVertex2f (rec.left, rec.top);
	glEnd ();
	if (temp_poly.size () >= 3) {
		shClip (temp_poly,temp_poly.size(),rec);
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

	//if (button == GLUT_WHEEL_DOWN) {
	//	segidec ();
	//}
	//if (button == GLUT_WHEEL_UP) {
	//	segiinc ();
	//}

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
int main (int argc, char** argv) {
	rec.top = 400;
	rec.bottom = 100;
	rec.left = 100;
	rec.right = 400;
	stated = DRAWING_POLY;
	glutInit (&argc, argv);							//初始化
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	//设置显示模式
	glutInitWindowPosition (600, 100);				//设置窗口的顶部和左边位置
	glutInitWindowSize (SCREENX, SCREENY);					//设置窗口的高度和宽度
	glutCreateWindow ("实验四：填充与多边形扫描转换");	//设置窗口标题
	glClearColor (0, 0, 0, 0);							//设置窗口背景颜色
	glMatrixMode (GL_PROJECTION);					//设置窗口视图
	gluOrtho2D (0, SCREENX, 0, SCREENY);
	glutDisplayFunc (display);						//注册显示函数

	/*注册各种自定义函数*/
	//glutTimerFunc (50, &_timeCtr, 1);
	glutMouseFunc (mouseButton);
	glutMotionFunc (mouseMove);

	/*注册右键菜单栏*/
	glutCreateMenu (menuCtr);
	glutAddMenuEntry ("clear", CLEAR);
	glutAddMenuEntry ("add", ADD);
	glutAddMenuEntry ("insert", INSERT);
	glutAddMenuEntry ("delete", DELETEP);
	glutAttachMenu (GLUT_RIGHT_BUTTON);


	glutMainLoop ();								//开始主循环
	return 0;
}