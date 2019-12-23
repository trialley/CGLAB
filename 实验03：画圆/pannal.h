//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#pragma once
#include<GL/glut.h>
#include<math.h>
#include<cstdlib>
#include<thread>
#include<vector>
#include<iostream>
#include"types.h"
#include"circle.h"

using std::cout;

#define DEBUG
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf /\
/printf
#endif


namespace TA {
	class pannal {
	public:
		/*һϵ���Զ�������*/
		enum err { INDEX_OUT_OF_RANGE };
		typedef enum state { NO_ACTION, ADDING_CIRCLE, MOVING_C_POINT , MOVING_R_POINT} state;
		typedef enum menufunc {NOFUNC = 999 , CLEAR,MID,BER} menufunc;

	private:
		/*��ֹ����*/
		pannal (const pannal&) {}
	protected:
		/*������Ļ�ĳ�����Ϣ*/
		static int _SCREENY;
		static int _SCREENX;
		static int _SLENGTH;

		/*����������Բʱ�ݴ�Բ��Բ��λ�úͰ뾶����λ��*/
		static point _c_point;
		static point _r_point;

		static enum state _state;			//���ڱ��浱ǰ״̬�����ߣ��ƶ��㣬��
		static GLfloat** _pix_arr;			//��Ļ��������
		static bool _inited;				//��ʼ������־
		static point* _chosed_point;		//ָ��ѡ�еĵ�
		static std::vector<line> _circles;	//���ڱ��������߶�
		static menufunc _algorithm;			//���ڱ��滭Բ���㷨����

		/*�����������*/
		static inline float _calDistance (float x1, float y1, float x2, float y2) {
			return pow ((pow ((x1 - x2), 2) + pow ((y1 - y2), 2)), 0.5);
		};
		static inline float _calDistance (point a1, point a2) {
			return pow ((pow ((a1.x - a2.x), 2) + pow ((a1.y - a2.y), 2)), 0.5);
		};

		/*����������л���һ����*/
		static void _drawPixel (int x, int y, GLfloat color = 1.0f) {
			_pix_arr[x][y] = color;
			glutPostRedisplay ();
		}

		/*�������ؾ�����Ļ*/
		static void _disPlayScreen () {
			glClear (GL_COLOR_BUFFER_BIT);

			for (int x = 0; x < _SCREENX; x++) {
				for (int y = 0; y < _SCREENY; y++) {
					glColor3f (_pix_arr[x][y], _pix_arr[x][y], _pix_arr[x][y]);
					glRectf (x,y,x + 1,y + 1);
				}
			}
			//˫������ʹ�øú�����������������������
			glutSwapBuffers ();
		}

		/*�������߶λ��Ƶ�����������*/
		static void _drawCirclesToArr () {
			for (int i = 0; i < _SCREENX; i++) {
				for (int j = 0; j < _SCREENY; j++) {
					_pix_arr[i][j] = 0;
				}
			}

			for (line& i : _circles) {
				switch (_algorithm) {
				case TA::pannal::MID:
					drawCircleMidPoint(i.start.x, i.start.y, _calDistance (i.start, i.end));

					break;
				case TA::pannal::BER:
				default:
					drawCircleBresenham (i.start.x, i.start.y, _calDistance (i.start, i.end));
					break;
				}
				drawPixel (i.start.x, i.start.y);
			}
		}

		/*��ʱ������ص�����*/
		static void _timeCtr (int i) {
			_drawCirclesToArr ();
			_disPlayScreen ();
			glutTimerFunc (50, &_timeCtr, 1);
		}

		
		static point* _getClosetPoint (int xi, int yi) {

			point* temp = &_r_point;
			float tempdis = 999999999;

			for (line& i : _circles) {
				float temp1 = _calDistance (xi, yi, i.start.x, i.start.y);
				if (temp1 < tempdis) {
					tempdis = temp1;
					temp = &i.start;
					_state = MOVING_C_POINT;
				}

				//temp1 = _calDistance (xi, yi, i.end.x, i.end.y);
				temp1 = abs (_calDistance (i.start, i.end) - temp1);
				if (temp1 < tempdis) {
					tempdis = temp1;
					temp = &i.end;
					_state = MOVING_R_POINT;
				}

			}
			if (tempdis > 6.0) {
				dprintf ("δѡ�к��ʵĵ�\n");
				return nullptr;
			}
			dprintf ("ѡ���˵㣺(%d, %d)����λ�ã�%p\n", temp->x, temp->y,temp);
			return temp;
		}


		/*������갴������*/
		static void _mouseButton (int button, int state, int xi, int yi) {
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				//�����������ʼ����
				_state = ADDING_CIRCLE;
				_c_point.x = x;
				_c_point.y = y;
				_r_point.x = x;
				_r_point.y = y;
				_circles.emplace_back (line (_c_point, _r_point));
			} else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				if (_state == ADDING_CIRCLE) {
					//̧���������������
					_state = NO_ACTION;
					dprintf ("�����Ѿ���ӣ����(%d, %d)���յ�(%d, %d)\n", _c_point.x, _c_point.y, _r_point.x, _r_point.y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
				_chosed_point = _getClosetPoint (x, y);
				if (_chosed_point==nullptr) {
					_state = NO_ACTION;
					dprintf ("δѡ�к��ʵĵ�\n");
				} else {
					dprintf ("λ���ƶ���ʼ����ǰ��λ��(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
				if (_state == MOVING_C_POINT) {
					_state = NO_ACTION;
					dprintf ("λ���ƶ���������ǰ����λ��(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			}
		}
		/*��������ƶ�*/
		static void _mouseMove (int xi, int yi) {
			/*����м䰴��ʱ��������գ�������Ϊ�գ������޸�������ĩ��λ��*/
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (_state == ADDING_CIRCLE) {
				_r_point.x = x;
				_r_point.y = y;
				_circles[_circles.size () - 1].end = _r_point;
				dprintf ("��ǰ���(%d, %d)�� ��ǰ�յ�(%d, %d)\n", _c_point.x, _c_point.y, _r_point.x, _r_point.y);
				return;
			} else if (_state == MOVING_C_POINT) {
				dprintf ("��ǰ��λ��(%d, %d)�� ", _chosed_point->x, _chosed_point->y);
				int dx =x- _chosed_point->x;
				int dy =y- _chosed_point->y;
				_chosed_point->x = x;
				_chosed_point->y = y;
				(_chosed_point + 1)->x += dx;
				(_chosed_point + 1)->y += dy;
				dprintf ("��ǰ����λ��(%d, %d)\n", _chosed_point->x, _chosed_point->y);
			} else if (_state == MOVING_R_POINT) {
				dprintf ("��ǰ��λ��(%d, %d)�� ", _chosed_point->x, _chosed_point->y);
				_chosed_point->x = x;
				_chosed_point->y = y;
				dprintf ("��ǰ����λ��(%d, %d)\n", _chosed_point->x, _chosed_point->y);
			}
		}

		static void _menuCtr (int funci) {
			switch (funci) {
			case CLEAR:
				//�����������
				_state = NO_ACTION;
				_circles.clear ();
				dprintf ("������м�¼\n");
				break;
			case BER:
				_algorithm = BER;
				cout << "�л�Ϊ Bresenham �㷨\n";
				break;
			case MID:
				_algorithm = MID;
				cout << "�л�Ϊ Mid Point �㷨\n";

				break;
			case NOFUNC:
			default:
				break;
			}
		}
	public:
		pannal (int x = 100, int y = 100, int slengthi = 5) {
			if (_inited) {
				cout << "�Ѿ���ʼ����\n";
				return;
			}
			std::thread t (pannal::init, x, y,  slengthi);
			t.detach ();
			cout << "��ס�����ק���Բ\n"
				<<"��ס�м���ק�ƶ��˵�\n"
				<<"����Ҽ��������л���Բ�㷨\n";
		}

		static void init (int x, int y,int slengthi) {
			/*��ֹ�ظ���ʼ��*/
			if (_inited) {
				cout << "�Ѿ���ʼ����\n";
				return;
			}
			_inited = true;

			/*��ʼ�����ؾ���*/
			_pix_arr = new GLfloat * [x];
			for (int i = 0; i < x; i++) {
				_pix_arr[i] = new GLfloat[y];
			}
			for (int i = 0; i < x; i++) {
				for (int j = 0; j < y; j++) {
					_pix_arr[i][j] = 0;
				}
			}
			_SCREENY = y;
			_SCREENX = x;
			_SLENGTH = slengthi;

			/*��ʼ��gl*/
			int x1 = 0;
			char** x2 = 0;
			glutInit (&x1, x2);

			glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
			glutInitWindowPosition (700, 100);
			glutInitWindowSize (_SCREENX * _SLENGTH, _SCREENY * _SLENGTH);
			glutCreateWindow ("ͼ��ѧʵ�����");
			gluOrtho2D (0, _SCREENX, 0, _SCREENY);

			/*ע������Զ�����꺯��*/
			glutTimerFunc (50, &_timeCtr, 1);
			glutMouseFunc (_mouseButton);
			glutMotionFunc (_mouseMove);

			/*ע���Ҽ��˵���*/
			glutCreateMenu (_menuCtr);
			glutAddMenuEntry ("clear", CLEAR);
			glutAddMenuEntry ("use Bresenham", MID);
			glutAddMenuEntry ("use Mid Point", BER);
			glutAttachMenu (GLUT_RIGHT_BUTTON);

			/*��ʼ��ѭ��*/
			glutMainLoop ();
		}

		/*�������������һ����*/
		static void drawPixel (int x, int y, GLfloat color = 1.0f) {
			if (x >= _SCREENX || y > _SCREENY || x < 0 || y < 0) {
				return;
				//throw INDEX_OUT_OF_RANGE;
			}
			_drawPixel (x, y, color);
		}


		/*��ȡһЩ������Ϣ*/
		static int getX () { return _SCREENX; }
		static int getY () { return _SCREENY; }
		static GLfloat getPixel (int x, int y) {
			if (x < 0 || y < 0 || x >= _SCREENX || y >= _SCREENY) {
				return 0;
			} else {
				return _pix_arr[x][y];
			}
		}

	};

}