//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#pragma once
#include<GL/glut.h>
#include<math.h>
#include<cstdlib>
#include<thread>
#include<vector>
#include<iostream>
#include"types.h"
#include"lines.h"
//#include"taglut.h"

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
		typedef enum state { ON_ACTION, ADDING_LINE, MOVING_POINT } state;
		typedef enum menufunc { CLEAR, DDA, BR, CP, NOFUNC = 999 } menufunc;

	private:
		/*��ֹ����*/
		pannal (const pannal&) {}
	protected:
		/*������Ļ�ĳ�����Ϣ*/
		static int _SCREENY;
		static int _SCREENX;
		static int _SLENGTH;

		/*���������ӵ�ʱ�ݴ��ߵ���ʼλ��*/
		static point _start_point;
		static point _end_point;

		static enum state _state;			//���ڱ��浱ǰ״̬�����ߣ��ƶ��㣬��
		static GLfloat** _pix_arr;			//��Ļ��������
		static bool _inited;				//��ʼ������־
		static point* _chosed_point;		//ָ��ѡ�еĵ�
		static std::vector<line> _lines;	//���ڱ��������߶�
		static menufunc _algorithm;			//���ڱ��滭ֱ�ߵ��㷨����

		/*�����������*/
		static inline float _calDistance (float x1, float y1, float x2, float y2) {
			return pow ((pow ((x1 - x2), 2) + pow ((y1 - y2), 2)), 0.5);
		};

		/*����������л���һ����*/
		static void _drawPixel (int x, int y, GLfloat color = 1.0f) {
			_pix_arr[x][y] = color;
			glutPostRedisplay ();
		}

		/*�������߶λ��Ƶ�����������*/
		static void _drawLinesToArr () {
			for (int i = 0; i < _SCREENX; i++) {
				for (int j = 0; j < _SCREENY; j++) {
					_pix_arr[i][j] = 0;
				}
			}

			for (line i : _lines) {
				int x1 = i.start.x;
				int y1 = i.start.y;
				int x2 = i.end.x;
				int y2 = i.end.y;
				switch (_algorithm) {
				case TA::pannal::DDA:
					drawlineDDA (x1,y1,x2,y2);
					break;
				case TA::pannal::BR:
					//drawlinebr (x1,y1,x2,y2);
					drawBresenham (x1, y1, x2, y2);
					break;
				case TA::pannal::CP:
					drawlinecp (x1,y1,x2,y2);
					break;
				default:
					break;
				}
			}
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

		/*��ʱ������ص�����*/
		static void _timeCtr (int i) {
			_drawLinesToArr ();
			_disPlayScreen ();
			glutTimerFunc (50, &_timeCtr, 1);
		}

		
		static point* _getClosetPoint (int xi, int yi) {

			point* temp = &_end_point;
			float tempdis = 999999999;

			for (line& i : _lines) {
				float temp1 = _calDistance (xi, yi, i.start.x, i.start.y);
				if (temp1 < tempdis) {
					tempdis = temp1;
					temp = &i.start;
				}

				temp1 = _calDistance (xi, yi, i.end.x, i.end.y);
				if (temp1 < tempdis) {
					tempdis = temp1;
					temp = &i.end;
				}
			}
			//dprintf ("ѡ���˵㣺(%d, %d)����λ�ã�%p\n", temp->x, temp->y,temp);
			return temp;
		}


		/*������갴������*/
		static void _mouseButton (int button, int state, int xi, int yi) {
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				//�����������ʼ����
				_state = ADDING_LINE;
				_start_point.x = x;
				_start_point.y = y;
				_lines.emplace_back (line (_start_point, _start_point));
			} else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				if (_state == ADDING_LINE) {
					//̧���������������
					_state = ON_ACTION;
					dprintf ("�����Ѿ���ӣ����(%d, %d)���յ�(%d, %d)\n", _start_point.x, _start_point.y, _end_point.x, _end_point.y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
				_chosed_point = _getClosetPoint (x, y);
				if (_calDistance (_chosed_point->x, _chosed_point->y, x, y) > 6.0) {
					_state = ON_ACTION;
					dprintf ("δѡ�к��ʵĵ�\n");
				} else {
					_state = MOVING_POINT;
					dprintf ("λ���ƶ���ʼ����ǰ��λ��(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
				if (_state == MOVING_POINT) {
					_state = ON_ACTION;
					dprintf ("λ���ƶ���������ǰ����λ��(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			}
		}
		/*��������ƶ�*/
		static void _mouseMove (int xi, int yi) {
			/*����м䰴��ʱ��������գ�������Ϊ�գ������޸�������ĩ��λ��*/
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (_state == ADDING_LINE) {
				_end_point.x = x;
				_end_point.y = y;
				_lines[_lines.size () - 1].end = _end_point;
				dprintf ("��ǰ���(%d, %d)�� ��ǰ�յ�(%d, %d)\n", _start_point.x, _start_point.y, _end_point.x, _end_point.y);
				return;
			} else if (_state == MOVING_POINT) {
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
				_state = ON_ACTION;
				_lines.clear ();
				dprintf ("�����������\n");
				break;

			case DDA:
			case CP:
			case BR:
				_algorithm = (menufunc)funci;
				dprintf ("���л������㷨\n");
				break;

			case NOFUNC:
			default:
				break;
			}
		}
	public:
		pannal (int x = 200, int y = 200, int slengthi = 3) {
			if (_inited) {
				cout << "�Ѿ���ʼ����\n";
				return;
			}
			std::thread t (pannal::init, x, y,  slengthi);
			t.detach ();
			cout << "��ס�����ק����߶�\n"
				<<"��ס�м���ק�ƶ��˵�\n"
				<<"����Ҽ��������л������㷨\n";
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
			glutAddMenuEntry ("---------", 999);
			glutAddMenuEntry ("use CP", CP);
			glutAddMenuEntry ("use DE", BR);
			glutAddMenuEntry ("use DDA", DDA);
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