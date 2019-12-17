#include<GL/glut.h>
#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;

/*------------------绘图算法相关-----------------*/
typedef struct point {
	int x, y;
	point () {}
	point (int xx, int yy)
		:x (xx), y (yy) {}
}point;

typedef struct line {
	point start;
	point end;
	line (int x1i, int y1i, int x2i, int y2i) {
		start.x = x1i;
		start.y = y1i;
		end.x = x2i;
		end.y = y2i;
	}
	line (point starti, point endi) :start (starti), end (endi) {}
}line;

typedef struct rectangle {
	int top;
	int bottom;
	int left;
	int right;
	rectangle () {}
	rectangle (int ti, int bi, int li, int ri):top(ti),bottom(bi),left(li),right(ri){}
}rectangle;


/*liang裁剪算法原理
以参数方程x表示直线段x2=x1+u*dx
使用u1  u2分别代表起点和终点
u1=max 0，ul，ub
u2=min 1 ut，ur

如何求u？
uk=qk/pk
p1=-dx
p2=dx
p3=-dy
p4=dy

q1=x1-xl
q2=xr-x1
q3=y1-yb
q4=yt-y1
那么如何确定入出边？


如何求四u？


dx>0  p1<0 guol p2>0 guor
dy>0 p3<0 guob p4>0 guot
入边u与0比找最大 p<0
出边u与1比找最小 p>0
*/
void LB_LineClip (float x1, float y1, float x2, float y2, float XL, float XR, float YB, float YT) {
	glColor3f (1, 1, 1);

	glBegin (GL_LINES);

	glVertex2f (x1, y1);
	glVertex2f (x2, y2);
	glEnd ();
	auto ClipT = [](float p, float q, float* u1, float* u2)->bool {
		float r;
		if (p < 0) {
			r = q / p;
			if (r > * u2)//此时直线入割点都超过了线段远端点 直线不在框内，继续计算无意义
				return false;
			if (r > * u1)
				*u1 = r;
		} else if (p > 0) {
			r = q / p;
			if (r < *u1)//直线不在框内
				return false;
			if (r < *u2)
				*u2 = r;
		} else
			return q >= 0;
		return TRUE;
	};
	float dx, dy, u1, u2;
	u1 = 0;//小的
	u2 = 1;//大的
	dx = x2 - x1;
	dy = y2 - y1;
	if (ClipT (-dx, x1 - XL, &u1, &u2))//p1=-dx q1=x1-XL  左
		if (ClipT (dx, XR - x1, &u1, &u2))//p2=dx q2=XR-x1  右
			if (ClipT (-dy, y1 - YB, &u1, &u2))//p3=-dy q3=y1-YB  下
				if (ClipT (dy, YT - y1, &u1, &u2))//p4=dy q4=YT-y1  上
				{
					glColor3f (1,0,0);
					glBegin (GL_LINES);
					glVertex2f (x1 + u1 * dx, y1 + u1 * dy);
					glVertex2f (x1 + u2 * dx, y1 + u2 * dy);
					glEnd ();
				}
}


#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif
using namespace std;


/*------------------窗口功能相关-----------------*/
vector<line> lines;
rectangle rec;

const int SCREENY = 500;
const int SCREENX = 500;
typedef enum state { MOVING_POINT, DRAWING_LINE, DRAWING_DONE } state;
typedef enum menufunc { CLEAR, START_MOV_POINT, START_ADD_POINT, START_DEL_POINT };
state stated = DRAWING_LINE;
point* chosed_point = nullptr;
void display (void) {
	glClear (GL_COLOR_BUFFER_BIT);
	for(auto i : lines) {
		LB_LineClip (i.start.x, i.start.y, i.end.x, i.end.y,rec.left,rec.right,rec.bottom,rec.top);
	}

	glColor3f (1, 1, 1);
	glBegin (GL_LINE_LOOP);
	glVertex2f ( rec.left, rec.bottom);
	glVertex2f (rec.right,rec.bottom );
	glVertex2f (rec.right, rec.top);
	glVertex2f (rec.left,rec.top);
	glEnd ();

	glutSwapBuffers ();

}
inline float calDistance (point a1, point a2) {
	return pow ((pow ((a1.x - a2.x), 2) + pow ((a1.y - a2.y), 2)), 0.5);
};
point* getClosetPoint (int xi, int yi) {
	point* temp = nullptr;
	float tempdis = 999999999;
	point click = { xi,yi };
	for (line& i : lines) {
		float temp1 = calDistance (click, i.start);
		if (temp1 < tempdis) {
			tempdis = temp1;
			temp = &i.start;
		}
		temp1 = calDistance (click,i.end);
		if (temp1 < tempdis) {
			tempdis = temp1;
			temp = &i.end;
		}
	}
	return temp;
}

/*鼠标按键回调*/
void mouseButton (int button, int state, int xi, int yi) {
	float x = xi;
	float y = SCREENY - yi;
	point temp_point = { (int)x,(int)y };
	switch (stated) {
	case MOVING_POINT:
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
			if (stated == MOVING_POINT) {
				stated = DRAWING_LINE;
				dprintf ("位置移动结束，当前点新位置(%d, %d)\n", chosed_point->x, chosed_point->y);
			}
		}
		break;
	case DRAWING_LINE:
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			dprintf ("添加点(%d, %d)\n", temp_point.x, temp_point.y);
			lines.push_back (line(temp_point, temp_point));
		}
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
			chosed_point = getClosetPoint (x, y);
			if (chosed_point == nullptr) {
				stated = DRAWING_LINE;
				dprintf ("未选中合适的点\n");
			} else {
				stated = MOVING_POINT;
				dprintf ("位置移动开始，当前点位置(%d, %d)\n", chosed_point->x, chosed_point->y);
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
	line* temp;

	switch (stated) {
	case MOVING_POINT:
		dprintf ("当前点位置(%d, %d)， ", chosed_point->x, chosed_point->y);
		chosed_point->x = x;
		chosed_point->y = y;
		dprintf ("当前点新位置(%d, %d)\n", chosed_point->x, chosed_point->y);
		glutPostRedisplay ();
		break;
	case DRAWING_LINE:
		if (lines.size () >= 1) {
			temp = &lines[lines.size () - 1];

			lines[lines.size () - 1].end = point (x, y);
			dprintf ("当前起点(%d, %d)， 当前终点(%d, %d)\n", temp->start.x, temp->start.y, temp->end.x, temp->end.y);
		}
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
		lines.clear();
		dprintf ("清空所有记录\n");
		break;

		//case NOFUNC:
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
	stated = DRAWING_LINE;
	glutInit (&argc, argv);							//初始化
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	//设置显示模式
	glutInitWindowPosition (600, 100);				//设置窗口的顶部和左边位置
	glutInitWindowSize (SCREENX, SCREENY);			//设置窗口的高度和宽度
	glutCreateWindow ("实验四：填充与多边形扫描转换");	//设置窗口标题
	glClearColor (0, 0, 0, 0);						//设置窗口背景颜色
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
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	glutMainLoop ();								//开始主循环
	return 0;
}