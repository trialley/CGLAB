#include<GL/glut.h>
#include<vector>
#include<algorithm>
#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif
using namespace std;


/*------------------��ͼ�㷨���-----------------*/
/*���Ա߱� �����ɨ��ת��*/
typedef struct XET {
	float x;//��ǰx���꣬�������
	float dx;//y����һλ����Ҫx���Ӷ���
	float ymax;//y�ﵽ��ֵ���߾ͱ�ɾ��
	XET* next;//ָ����һ����
} AET, NET;

typedef struct point {
	float x;
	float y;
}point;
bool operator == (point& p1, point p2) {
	return p1.x == p2.x && p1.y == p2.y;
}
typedef vector<point> points;
typedef vector<points> polys;

AET* merge (AET* l1, AET* l2) {
	AET* dummy = new AET ();
	AET* cur = dummy;
	while (l1 && l2) {
		if (l1->x < l2->x) {
			cur->next = l1;
			l1 = l1->next;
		} else {
			cur->next = l2;
			l2 = l2->next;
		}
		cur = cur->next;
	}
	if (l1) cur->next = l1;
	if (l2) cur->next = l2;
	return dummy->next;
}
AET* sortList (AET* head) {
	if (!head || !head->next) return head;
	AET* slow = head, * fast = head, * pre = head;
	while (fast && fast->next) {
		pre = slow;
		slow = slow->next;
		fast = fast->next->next;
	}
	pre->next = NULL;
	return merge (sortList (head), sortList (slow));
}
//AET* tq = pAET;
//p = pAET->next;
//tq->next = NULL;//����ָ���p��xֵ��ĵ�һ���ڵ�ĸ��ڵ�
//while (p) {
//	while (tq->next && p->x >= tq->next->x)
//		tq = tq->next;

//	//��p���뵽���ڵ����ӽڵ�֮��
//	NET* s = p->next;
//	p->next = tq->next;
//	tq->next = p;
//	p = s;
//	tq = pAET;
//}

void drawAET (points& polypoint) {



	int POINTNUM = polypoint.size ();
	/******������ߵ��y����(ɨ�赽�˽���)****************************************/
	int MaxY = 0;
	int i;
	for (i = 0; i < POINTNUM; i++)
		if (polypoint[i].y > MaxY)
			MaxY = polypoint[i].y;

	/******��ʼ��NET��************************************************************/
	NET* pNET[1024];
	for (i = 0; i <= MaxY; i++) {
		pNET[i] = new NET;
		pNET[i]->next = NULL;
	}
	glColor3f (1.0, 1.0, 1.0);             //����ֱ�ߵ���ɫ
	glBegin (GL_POINTS);
	//ɨ�貢����NET��****�����߱�
	for (i = 0; i <= MaxY; i++) {//����ÿһ��ɨ����
		for (int j = 0; j < POINTNUM; j++)
			if (polypoint[j].y == i) {//�ҵ��Ը�ɨ����yֵΪ��������
				//һ�����ǰ���һ�����γ�һ���߶Σ�������ĵ�Ҳ�γ��߶�

				//�ҵ��Ը�yΪ�µ����
				if (polypoint[(j - 1 + POINTNUM) % POINTNUM].y > polypoint[j].y) {
					NET* p = new NET;
					p->x = polypoint[j].x;
					p->ymax = polypoint[(j - 1 + POINTNUM) % POINTNUM].y;
					p->dx = (polypoint[(j - 1 + POINTNUM) % POINTNUM].x - polypoint[j].x) / (polypoint[(j - 1 + POINTNUM) % POINTNUM].y - polypoint[j].y);
					p->next = pNET[i]->next;
					pNET[i]->next = p;
				}
				if (polypoint[(j + 1 + POINTNUM) % POINTNUM].y > polypoint[j].y) {
					NET* p = new NET;
					p->x = polypoint[j].x;
					p->ymax = polypoint[(j + 1 + POINTNUM) % POINTNUM].y;
					p->dx = (polypoint[(j + 1 + POINTNUM) % POINTNUM].x - polypoint[j].x) / (polypoint[(j + 1 + POINTNUM) % POINTNUM].y - polypoint[j].y);
					p->next = pNET[i]->next;
					pNET[i]->next = p;
				}
			}
	}


	//��ʼ��AET����
	AET* pAET = new AET;
	pAET->next = NULL;

	/*���ϸ��»��Ա߱�AET ��ӻ�ɾ��*/
	for (i = 0; i <= MaxY; i++) {
		//�����µĽ���x,����AET����һ��ѭ����������κ�ֵ
		AET* p = pAET->next;
		while (p) {
			p->x = p->x + p->dx;
			p = p->next;
		}

		//���º�AET�����Ȱ���x�������� ��Ϊ����Ǵ���������
		sortList (pAET);


		//ɾ���ﵽymax�ı�
		AET* q = pAET;
		p = q->next;
		while (p) {
			if (p->ymax == i) {
				q->next = p->next;
				delete p;
				p = q->next;
			} else {
				q = q->next;
				p = q->next;
			}
		}

		//��ȡɨ�����½Ӵ��ı�
		p = pNET[i]->next;
		q = pAET;
		while (p) {
			while (q->next && p->x >= q->next->x)
				q = q->next;
			NET* s = p->next;
			p->next = q->next;
			q->next = p;
			p = s;
			q = pAET;
		}
		
		//��һ��ɨ���߽������
		glColor3f (1, 1, 1);
		p = pAET->next;

		//������ǰɨ�������б�
		while (p && p->next) {
			for (float j = p->x; j <= p->next->x; j++)
				glVertex2i (j, i);//���㣬��x������һ���ߵ�x

			//������һ�ߣ�ֱ�ӽ�����������
			p = p->next->next;
		}
	}
	glEnd ();
	glFlush ();
}


/*------------------���ڹ������-----------------*/
const int SCREENY = 500;
const int SCREENX = 500;
points pointsd = { {250,50},{550,150},{550,400},{250,250},{100,350},{100,100},{120,30} }; //����ζ���
points temp_poly;
typedef enum state { MOVING_POINT, ADDING_POINT, DRAWING_POLY, DRAWING_DONE } state;
typedef enum addstate { ADD=9999, INSERT, DELETEP} addstate;
typedef enum menufunc { CLEAR, START_MOV_POINT, START_ADD_POINT, START_DEL_POINT };
state stated = DRAWING_POLY;
addstate addstated = ADD;
point* chosed_point = nullptr;
void display (void) {
	glClear (GL_COLOR_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	glPointSize (4);
	glBegin (GL_POINTS);
	glClear (GL_COLOR_BUFFER_BIT);        //��ֵ�Ĵ�����ʾ.
	if (temp_poly.size () >= 3) {
		drawAET (temp_poly);
	}
	glEnd ();

	glColor3f (1.0, .0, .0);
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
point* getClosetPoint (int xi, int yi) {
	point* temp = nullptr;
	float tempdis = 999999999;
	point click = { xi,yi };
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
	point ti = {x,y};
	point* t1 = getClosetPoint (x, y);
	if (t1 == nullptr) {
		printf ("�޷��ҵ����ʵĲ����\n");
	} else {

	temp_poly.insert (find (temp_poly.begin (), temp_poly.end (), *t1),ti);
	}

}
/*��갴���ص�*/
void mouseButton (int button, int state, int xi, int yi) {
	float x = xi;
	float y = SCREENY - yi;
	point temp_point = { x,y };
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
				if (temp_poly .size()< 2) {
					printf ("��Ǹ����ǰ����ĿС��2���޷����룬������Ӹ���㣬Ĭ��ѡ����ӵ�");
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
		glutPostRedisplay ();
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
}

void menuCtr (int funci) {
	switch (funci) {
	case CLEAR:
		//������ж����
		temp_poly.clear ();
		dprintf ("������м�¼\n");
		break;
	case ADD:
		addstated = ADD;
		printf ("ѡ������ӵ�ģʽ\n");
		break;

	case INSERT:
		addstated = INSERT;
		printf ("ѡ���˲����ģʽ\n");

		break;

	case DELETEP:
		addstated = DELETEP;
		printf ("ѡ����ɾ����ģʽ\n");
		break;
	default:
		break;
	}
	glutPostRedisplay ();
}

int main (int argc, char** argv) {
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