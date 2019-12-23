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

//����������(B��zier curve)����
/*------------------��ͼ�㷨���-----------------*/
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

		/*����ѭ����һ�طֵ㣬һ�ص�����һ�ر���*/
		/*���е��Ƶ�ѭ��������Ҫ����n-1�ε��Ƽ���*/
		for (double t = 0.0; t <= 1; t += d) {
			vector<point> temp;

			/*��ѭ������   �� i �ε��ƣ���Ҫ����n-i���µĵ�*/
			for (int i = 1; i < n; ++i) {

				/*��ѭ������*/
				for (int j = 0; j < n - i; ++j) {
					if (i == 1) {// i==1ʱ,��һ�ε���,����֪���Ƶ���� 
						temp.push_back (
							point (
								(float)((points[j].x) * (1 - t) + points[j + 1].x * t),
								points[j].y * (1 - t) + points[j + 1].y * t
							)
						);
					} else {// i != 1ʱ,ͨ����һ�ε����Ľ������  
						temp[j].x = temp[j].x * (1 - t) + temp[j + 1].x * t;
						temp[j].y = temp[j].y * (1 - t) + temp[j + 1].y * t;
					}
				}
			}
			/*һ����ĵ�����ɣ���¼�õ�*/
			news.push_back (temp[0]);
		}



		/*������ϣ���ʼ����*/
		glColor3f (1, 0, 0);
		for (int i = 0; i < news.size () - 1; i++) {
			drawLine (news[i], news[i + 1]);
		}
		//���
		drawLine (news[news.size () - 1], points[points.size () - 1]);


}

/*------------------���ڹ������-----------------*/
const int SCREENY = 500;
const int SCREENX = 500;
points pointsd = { {250,50},{550,150},{550,400},{250,250},{100,350},{100,100},{120,30} }; //����ζ���
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
		cout << "���ȹ���\n";
	} else {
		segi*=2;
		cout << "�ֶ�"<<segi<<"\n";

	}
	double d = 1.0 / (float)segi;
	cout << "t=" << d << "\n";
}
void segidec () {
	if (segi <= 1) {
		cout << "�޷��������پ���\n";
	} else {
		segi/=2;
		cout << "�ֶ�" << segi << "\n";

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
	point click = {(float) xi,yi };
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
					point* to_be_delete= getClosetPoint (temp_point.x,temp_point.y );
					if (to_be_delete == nullptr) {
						printf ("��Ǹ��δѡ���κε㣬�޷�ɾ��\n");
					} else {
					dprintf ("ɾ����(%f, %f)\n", temp_point.x, temp_point.y);
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

	case DEC_SEG:
		if (segi<1) {
			std::cout << "�ֶ������٣���ֹ����\n";
		} else {
			segi--;
		}
		break;

	case INC_SEG:
		segi++;
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
void keyPress (unsigned char key, int x, int y) {
	switch (key) {
	case '+':
		segi++;
		cout << "�ֶμ�:" << segi<<endl;

		break;

	case '-':
		if (segi > 1) {
			segi--;
			cout << "�ֶμ�:"<<segi<<endl;
		} else {
			cout << "�����ټ���\n";
		}

		break;
	default:
		break;
	}
	glutPostRedisplay ();

}

int main (int argc, char** argv) {
	stated = DRAWING_POLY;
	glutInit (&argc, argv);							//��ʼ��
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	//������ʾģʽ
	glutInitWindowPosition (600, 100);				//���ô��ڵĶ��������λ��
	glutInitWindowSize (SCREENX, SCREENY);					//���ô��ڵĸ߶ȺͿ��
	glutCreateWindow ("ʵ��08������Bezier����");	//���ô��ڱ���
	glClearColor (0, 0, 0, 0);							//���ô��ڱ�����ɫ
	glMatrixMode (GL_PROJECTION);					//���ô�����ͼ
	gluOrtho2D (0, SCREENX, 0, SCREENY);
	glutDisplayFunc (display);						//ע����ʾ����

	/*ע������Զ��庯��*/
	//glutTimerFunc (50, &_timeCtr, 1);
	glutMouseFunc (mouseButton);
	glutMotionFunc (mouseMove);
	glutKeyboardFunc (keyPress);

	/*ע���Ҽ��˵���*/
	glutCreateMenu (menuCtr);
	glutAddMenuEntry ("clear", CLEAR);
	glutAddMenuEntry ("inc seg num", INC_SEG);
	glutAddMenuEntry ("dec seg num", DEC_SEG);
	glutAddMenuEntry ("add", ADD);
	glutAddMenuEntry ("insert", INSERT);
	glutAddMenuEntry ("delete", DELETEP);
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	glutMainLoop ();								//��ʼ��ѭ��
	return 0;
}