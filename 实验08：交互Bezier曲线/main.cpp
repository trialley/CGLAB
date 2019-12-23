#include<vector>
#include<algorithm>
#include<iostream>
#include"glut.h"
#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif
using namespace std;

//贝塞尔曲线(Bézier curve)，又
/*------------------绘图算法相关-----------------*/
typedef struct point {
	float x;
	float y;
	point(float xi, float yi):x(xi),y(yi){}
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

void drawBezier (points& points,int segi) {

		int n = points.size ();
		vector<point> news;
		//double d = 1.0 / (segi * n);
		double d = 1.0 /  (float)segi;
		//cout << "t=" << d<<"\n";

		/*三重循环，一重分点，一重迭代，一重遍历*/
		/*进行递推的循环，共需要进行n-1次递推计算*/
		for (double t = 0.0; t <= 1; t += d) {
			vector<point> temp;

			/*本循环迭代   第 i 次递推，需要计算n-i个新的点*/
			for (int i = 1; i < n; ++i) {

				/*本循环遍历*/
				for (int j = 0; j < n - i; ++j) {
					if (i == 1) {// i==1时,第一次迭代,由已知控制点计算 
						temp.push_back (
							point (
								(float)((points[j].x) * (1 - t) + points[j + 1].x * t),
								points[j].y * (1 - t) + points[j + 1].y * t
							)
						);
					} else {// i != 1时,通过上一次迭代的结果计算  
						temp[j].x = temp[j].x * (1 - t) + temp[j + 1].x * t;
						temp[j].y = temp[j].y * (1 - t) + temp[j + 1].y * t;
					}
				}
			}
			/*一个点的递推完成，记录该点*/
			news.push_back (temp[0]);
		}



		/*递推完毕，开始绘制*/
		glColor3f (1, 0, 0);
		for (int i = 0; i < news.size () - 1; i++) {
			drawLine (news[i], news[i + 1]);
		}
		//封口
		drawLine (news[news.size () - 1], points[points.size () - 1]);


}

/*------------------窗口功能相关-----------------*/
const int SCREENY = 500;
const int SCREENX = 500;
points pointsd = { {250,50},{550,150},{550,400},{250,250},{100,350},{100,100},{120,30} }; //多边形顶点
points temp_poly;
typedef enum state { MOVING_POINT, ADDING_POINT, DRAWING_POLY, DRAWING_DONE } state;
typedef enum addstate { ADD = 9999, INSERT, DELETEP} add;
typedef enum menufunc { CLEAR, INC_SEG,DEC_SEG };
addstate addstated= ADD;
state stated = DRAWING_POLY;
point* chosed_point = nullptr;
int segi = 1;
void segiinc () {
	if (segi >= 64) {
		cout << "精度过大\n";
	} else {
		segi*=2;
		cout << "分段"<<segi<<"\n";

	}
	double d = 1.0 / (float)segi;
	cout << "t=" << d << "\n";
}
void segidec () {
	if (segi <= 1) {
		cout << "无法继续减少精度\n";
	} else {
		segi/=2;
		cout << "分段" << segi << "\n";

	}
	double d = 1.0 / (float)segi;
	cout << "t=" << d << "\n";
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
	point click = {(float) xi,yi };
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
		segidec ();
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
					point* to_be_delete= getClosetPoint (temp_point.x,temp_point.y );
					if (to_be_delete == nullptr) {
						printf ("抱歉，未选中任何点，无法删除\n");
					} else {
					dprintf ("删除点(%f, %f)\n", temp_point.x, temp_point.y);
					temp_poly.erase( find (temp_poly.begin (), temp_poly.end (), *to_be_delete));

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
		if (segi<1) {
			std::cout << "分段数过少，禁止降低\n";
		} else {
			segi--;
		}
		break;

	case INC_SEG:
		segi++;
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
		segi++;
		cout << "分段加:" << segi<<endl;

		break;

	case '-':
		if (segi > 1) {
			segi--;
			cout << "分段减:"<<segi<<endl;
		} else {
			cout << "不能再减了\n";
		}

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