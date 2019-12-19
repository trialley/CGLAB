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


/*------------------��ͼ�㷨���-----------------*/
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
�ֶ���֮��˼��
һ���߲ü�����������µ���ɵ㣿
*/
void clipEdge (points Inpoints, points& output_points, Edge clip_line) {
	auto isInside = [](point& Testp2t, Edge clip_line)->bool {
		if (clip_line[1].x > clip_line[0].x) {//�ü���Ϊ�����±�
			if (Testp2t.y >= clip_line[0].y)
				return true;
		} else if (clip_line[1].x < clip_line[0].x) {//�ü���Ϊ�����ϱ�
			if (Testp2t.y <= clip_line[0].y)
				return true;
		} else if (clip_line[1].y > clip_line[0].y) {
			//�ü���Ϊ�����ұ�
			if (Testp2t.x <= clip_line[0].x)
				return true;
		} else if (clip_line[1].y < clip_line[0].y) {
			//�ü���Ϊ�������
			if (Testp2t.x >= clip_line[0].x)
				return true;
		} else {
			return  false;
		}
	};
	//ֱ�߶�p1p2�ʹ��ڱ߽��󽻣����ؽ��㣻����LB�ü��㷨
	auto getInterPoint = [](point& p1, point& p2, Edge clip_line)->point {
		point inter_point;
		if (clip_line[0].y == clip_line[1].y) {//ˮƽ�ü���
			inter_point.y = clip_line[0].y;
			inter_point.x = p1.x + (clip_line[0].y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
		} else {//��ֱ�ü���
			inter_point.x = clip_line[0].x;
			inter_point.y = p1.y + (clip_line[0].x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
		}
		return inter_point;
	};
	point p1, p2, inter_point;
	int j;

	p1 = Inpoints[Inpoints.size() - 1];//p1����ĩһ����

	for (point p2:Inpoints) {
		if (isInside (p2, clip_line)) {
			if (isInside (p1, clip_line)) {//ͷβ���ڣ�β���Σ�����Ϊͷ�������� ��
				output_points.push_back (p2); 
			}else { //ͷ��β�ڣ���β���� ��
				output_points.push_back(getInterPoint (p1, p2, clip_line));
				output_points.push_back(p2);
			}
		} else if (isInside (p1, clip_line)) { //ͷ��β�⣬����� ��
			output_points.push_back(getInterPoint (p1, p2, clip_line));
		} //ͷβ���⣬û�е�����
		p1 = p2;
	}
}

void shClip (points& Inpoints, int Inlength, rectangle rect) {
	//���Ȼ���ԭͼ��
	glColor3f (1, 1, 1);
	for (int i = 0; i < Inpoints.size () - 1; i++) {
		drawLine (Inpoints[i], Inpoints[i + 1]);
	}
	drawLine (Inpoints[Inpoints.size () - 1], Inpoints[0]);
	Edge clip_line;
	points output_points;
	int Outlength = 0;

	//ʹ�ô����±߽�����и�
	clip_line[0].x = rect.left;
	clip_line[0].y = rect.bottom;
	clip_line[1].x = rect.right;
	clip_line[1].y = rect.bottom;
	clipEdge (Inpoints, output_points, clip_line);
	//Ȼ��ʹ�ô���shang�߽�����и�
	points output_points1;
	int Outlength1 = 0;
	clip_line[0].x = rect.right;
	clip_line[0].y = rect.top;
	clip_line[1].x = rect.left;
	clip_line[1].y = rect.top;
	clipEdge (output_points, output_points1, clip_line);
	//Ȼ��ʹ�ô�����߽�����и�
	points output_points2;
	int Outlength2 = 0;
	clip_line[0].x = rect.left;
	clip_line[0].y = rect.top;
	clip_line[1].x = rect.left;
	clip_line[1].y = rect.bottom;
	clipEdge (output_points1, output_points2, clip_line);
	//Ȼ��ʹ�ô����ұ߽�����и�
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

/*------------------���ڹ������-----------------*/
rectangle rec;
const int SCREENY = 500;
const int SCREENX = 500;
points pointsd = { {250,50},{550,150},{550,400},{250,250},{100,350},{100,100},{120,30} }; //����ζ���
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

	/*����*/
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
		dprintf ("δѡ�к��ʵĵ�\n");
		return nullptr;
	}
	dprintf ("ѡ���˵㣺(%f, %f)����λ�ã�%p\n", temp->x, temp->y, temp);
	return temp;
}
void insertPoint (int x, int y) {
	point ti = { (float)x,(float)y };
	point* t1 = getClosetPoint (x, y);
	if (t1 == nullptr) {
		printf ("�޷��ҵ����ʵĲ����\n");
	} else {

		temp_poly.insert (find (temp_poly.begin (), temp_poly.end (), *t1), ti);
	}

}
/*��갴���ص�*/
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
				dprintf ("λ���ƶ���������ǰ����λ��(%f, %f)\n", chosed_point->x, chosed_point->y);
			}
		}
		break;
	case ADDING_POINT:
		break;
	case DRAWING_POLY:
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			switch (addstated) {
			case ADD:
				dprintf ("��ӵ�(%f, %f)\n", temp_point.x, temp_point.y);
				temp_poly.emplace_back (temp_point);
				break;
			case INSERT:
				if (temp_poly.size () < 2) {
					printf ("��Ǹ����ǰ����ĿС��2���޷����룬������Ӹ���㣬Ĭ��ѡ����ӵ�\n");
					dprintf ("��ӵ�(%f, %f)\n", temp_point.x, temp_point.y);
					temp_poly.emplace_back (temp_point);
				} else {
					dprintf ("�����(%f, %f)\n", temp_point.x, temp_point.y);
					insertPoint (temp_point.x, temp_point.y);
				}
				break;
			case DELETEP:
				if (temp_poly.size () < 2) {
					printf ("��Ǹ����ǰ����ĿС��1���޷�ɾ����������Ӹ����\n");

				} else {
					point* to_be_delete = getClosetPoint (temp_point.x, temp_point.y);
					if (to_be_delete == nullptr) {
						printf ("��Ǹ��δѡ���κε㣬�޷�ɾ��\n");
					} else {
						dprintf ("ɾ����(%f, %f)\n", temp_point.x, temp_point.y);
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
				dprintf ("δѡ�к��ʵĵ�\n");
			} else {
				stated = MOVING_POINT;
				dprintf ("λ���ƶ���ʼ����ǰ��λ��(%f, %f)\n", chosed_point->x, chosed_point->y);
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


/*����ƶ��ص�*/
void mouseMove (int xi, int yi) {
	float x = xi;
	float y = SCREENY - yi;
	switch (stated) {
	case MOVING_POINT:
		dprintf ("��ǰ��λ��(%f, %f)�� ", chosed_point->x, chosed_point->y);
		chosed_point->x = x;
		chosed_point->y = y;
		dprintf ("��ǰ����λ��(%f, %f)\n", chosed_point->x, chosed_point->y);
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
		//������ж����
		temp_poly.clear ();
		dprintf ("������м�¼\n");
		break;
	case ADD:
		cout << "�л�����ӵ�ģʽ\n";
		addstated = ADD;
		break;

	case INSERT:
		cout << "�л��������ģʽ\n";
		addstated = INSERT;
		break;

	case DELETEP:
		cout << "�л���ɾ����ģʽ\n";
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
	glutInit (&argc, argv);							//��ʼ��
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	//������ʾģʽ
	glutInitWindowPosition (600, 100);				//���ô��ڵĶ��������λ��
	glutInitWindowSize (SCREENX, SCREENY);					//���ô��ڵĸ߶ȺͿ��
	glutCreateWindow ("ʵ���ģ����������ɨ��ת��");	//���ô��ڱ���
	glClearColor (0, 0, 0, 0);							//���ô��ڱ�����ɫ
	glMatrixMode (GL_PROJECTION);					//���ô�����ͼ
	gluOrtho2D (0, SCREENX, 0, SCREENY);
	glutDisplayFunc (display);						//ע����ʾ����

	/*ע������Զ��庯��*/
	//glutTimerFunc (50, &_timeCtr, 1);
	glutMouseFunc (mouseButton);
	glutMotionFunc (mouseMove);

	/*ע���Ҽ��˵���*/
	glutCreateMenu (menuCtr);
	glutAddMenuEntry ("clear", CLEAR);
	glutAddMenuEntry ("add", ADD);
	glutAddMenuEntry ("insert", INSERT);
	glutAddMenuEntry ("delete", DELETEP);
	glutAttachMenu (GLUT_RIGHT_BUTTON);


	glutMainLoop ();								//��ʼ��ѭ��
	return 0;
}