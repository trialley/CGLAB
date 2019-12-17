#include<GL/glut.h>
#include<iostream>
#include<algorithm>
#include<vector>
using namespace std;

/*------------------��ͼ�㷨���-----------------*/
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


/*liang�ü��㷨ԭ��
�Բ�������x��ʾֱ�߶�x2=x1+u*dx
ʹ��u1  u2�ֱ���������յ�
u1=max 0��ul��ub
u2=min 1 ut��ur

�����u��
uk=qk/pk
p1=-dx
p2=dx
p3=-dy
p4=dy

q1=x1-xl
q2=xr-x1
q3=y1-yb
q4=yt-y1
��ô���ȷ������ߣ�


�������u��


dx>0  p1<0 guol p2>0 guor
dy>0 p3<0 guob p4>0 guot
���u��0������� p<0
����u��1������С p>0
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
			if (r > * u2)//��ʱֱ�����㶼�������߶�Զ�˵� ֱ�߲��ڿ��ڣ���������������
				return false;
			if (r > * u1)
				*u1 = r;
		} else if (p > 0) {
			r = q / p;
			if (r < *u1)//ֱ�߲��ڿ���
				return false;
			if (r < *u2)
				*u2 = r;
		} else
			return q >= 0;
		return TRUE;
	};
	float dx, dy, u1, u2;
	u1 = 0;//С��
	u2 = 1;//���
	dx = x2 - x1;
	dy = y2 - y1;
	if (ClipT (-dx, x1 - XL, &u1, &u2))//p1=-dx q1=x1-XL  ��
		if (ClipT (dx, XR - x1, &u1, &u2))//p2=dx q2=XR-x1  ��
			if (ClipT (-dy, y1 - YB, &u1, &u2))//p3=-dy q3=y1-YB  ��
				if (ClipT (dy, YT - y1, &u1, &u2))//p4=dy q4=YT-y1  ��
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


/*------------------���ڹ������-----------------*/
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

/*��갴���ص�*/
void mouseButton (int button, int state, int xi, int yi) {
	float x = xi;
	float y = SCREENY - yi;
	point temp_point = { (int)x,(int)y };
	switch (stated) {
	case MOVING_POINT:
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
			if (stated == MOVING_POINT) {
				stated = DRAWING_LINE;
				dprintf ("λ���ƶ���������ǰ����λ��(%d, %d)\n", chosed_point->x, chosed_point->y);
			}
		}
		break;
	case DRAWING_LINE:
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			dprintf ("��ӵ�(%d, %d)\n", temp_point.x, temp_point.y);
			lines.push_back (line(temp_point, temp_point));
		}
		if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
			chosed_point = getClosetPoint (x, y);
			if (chosed_point == nullptr) {
				stated = DRAWING_LINE;
				dprintf ("δѡ�к��ʵĵ�\n");
			} else {
				stated = MOVING_POINT;
				dprintf ("λ���ƶ���ʼ����ǰ��λ��(%d, %d)\n", chosed_point->x, chosed_point->y);
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
	line* temp;

	switch (stated) {
	case MOVING_POINT:
		dprintf ("��ǰ��λ��(%d, %d)�� ", chosed_point->x, chosed_point->y);
		chosed_point->x = x;
		chosed_point->y = y;
		dprintf ("��ǰ����λ��(%d, %d)\n", chosed_point->x, chosed_point->y);
		glutPostRedisplay ();
		break;
	case DRAWING_LINE:
		if (lines.size () >= 1) {
			temp = &lines[lines.size () - 1];

			lines[lines.size () - 1].end = point (x, y);
			dprintf ("��ǰ���(%d, %d)�� ��ǰ�յ�(%d, %d)\n", temp->start.x, temp->start.y, temp->end.x, temp->end.y);
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
		//������ж����
		lines.clear();
		dprintf ("������м�¼\n");
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
	glutInit (&argc, argv);							//��ʼ��
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);	//������ʾģʽ
	glutInitWindowPosition (600, 100);				//���ô��ڵĶ��������λ��
	glutInitWindowSize (SCREENX, SCREENY);			//���ô��ڵĸ߶ȺͿ��
	glutCreateWindow ("ʵ���ģ����������ɨ��ת��");	//���ô��ڱ���
	glClearColor (0, 0, 0, 0);						//���ô��ڱ�����ɫ
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
	glutAttachMenu (GLUT_RIGHT_BUTTON);

	glutMainLoop ();								//��ʼ��ѭ��
	return 0;
}