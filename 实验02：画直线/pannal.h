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
		/*一系列自定义类型*/
		enum err { INDEX_OUT_OF_RANGE };
		typedef enum state { ON_ACTION, ADDING_LINE, MOVING_POINT } state;
		typedef enum menufunc { CLEAR, DDA, BR, CP, NOFUNC = 999 } menufunc;

	private:
		/*禁止复制*/
		pannal (const pannal&) {}
	protected:
		/*保存屏幕的长宽信息*/
		static int _SCREENY;
		static int _SCREENX;
		static int _SLENGTH;

		/*用于左键添加点时暂存线的起始位置*/
		static point _start_point;
		static point _end_point;

		static enum state _state;			//用于保存当前状态：画线，移动点，无
		static GLfloat** _pix_arr;			//屏幕像素阵列
		static bool _inited;				//初始化与否标志
		static point* _chosed_point;		//指向被选中的点
		static std::vector<line> _lines;	//用于保存所有线段
		static menufunc _algorithm;			//用于保存画直线的算法种类

		/*计算两点距离*/
		static inline float _calDistance (float x1, float y1, float x2, float y2) {
			return pow ((pow ((x1 - x2), 2) + pow ((y1 - y2), 2)), 0.5);
		};

		/*向矩形数组中绘制一个点*/
		static void _drawPixel (int x, int y, GLfloat color = 1.0f) {
			_pix_arr[x][y] = color;
			glutPostRedisplay ();
		}

		/*将多条线段绘制到像素数组中*/
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


		/*绘制像素矩阵到屏幕*/
		static void _disPlayScreen () {
			glClear (GL_COLOR_BUFFER_BIT);

			for (int x = 0; x < _SCREENX; x++) {
				for (int y = 0; y < _SCREENY; y++) {
					glColor3f (_pix_arr[x][y], _pix_arr[x][y], _pix_arr[x][y]);
					glRectf (x,y,x + 1,y + 1);
				}
			}
			//双缓冲下使用该函数交换两个缓冲区的内容
			glutSwapBuffers ();
		}

		/*计时器归零回调函数*/
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
			//dprintf ("选中了点：(%d, %d)，于位置：%p\n", temp->x, temp->y,temp);
			return temp;
		}


		/*监听鼠标按键动作*/
		static void _mouseButton (int button, int state, int xi, int yi) {
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				//按下左键，开始画线
				_state = ADDING_LINE;
				_start_point.x = x;
				_start_point.y = y;
				_lines.emplace_back (line (_start_point, _start_point));
			} else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				if (_state == ADDING_LINE) {
					//抬起左键，结束画线
					_state = ON_ACTION;
					dprintf ("线条已经添加：起点(%d, %d)，终点(%d, %d)\n", _start_point.x, _start_point.y, _end_point.x, _end_point.y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
				_chosed_point = _getClosetPoint (x, y);
				if (_calDistance (_chosed_point->x, _chosed_point->y, x, y) > 6.0) {
					_state = ON_ACTION;
					dprintf ("未选中合适的点\n");
				} else {
					_state = MOVING_POINT;
					dprintf ("位置移动开始，当前点位置(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
				if (_state == MOVING_POINT) {
					_state = ON_ACTION;
					dprintf ("位置移动结束，当前点新位置(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			}
		}
		/*监听鼠标移动*/
		static void _mouseMove (int xi, int yi) {
			/*鼠标中间按下时，数组清空；若数组为空，则不能修改新线条末端位置*/
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (_state == ADDING_LINE) {
				_end_point.x = x;
				_end_point.y = y;
				_lines[_lines.size () - 1].end = _end_point;
				dprintf ("当前起点(%d, %d)， 当前终点(%d, %d)\n", _start_point.x, _start_point.y, _end_point.x, _end_point.y);
				return;
			} else if (_state == MOVING_POINT) {
				dprintf ("当前点位置(%d, %d)， ", _chosed_point->x, _chosed_point->y);
				_chosed_point->x = x;
				_chosed_point->y = y;
				dprintf ("当前点新位置(%d, %d)\n", _chosed_point->x, _chosed_point->y);
			}
		}

		static void _menuCtr (int funci) {
			switch (funci) {
			case CLEAR:
				//清空所有线条
				_state = ON_ACTION;
				_lines.clear ();
				dprintf ("清空所有线条\n");
				break;

			case DDA:
			case CP:
			case BR:
				_algorithm = (menufunc)funci;
				dprintf ("已切换划线算法\n");
				break;

			case NOFUNC:
			default:
				break;
			}
		}
	public:
		pannal (int x = 200, int y = 200, int slengthi = 3) {
			if (_inited) {
				cout << "已经初始化！\n";
				return;
			}
			std::thread t (pannal::init, x, y,  slengthi);
			t.detach ();
			cout << "按住左键拖拽添加线段\n"
				<<"按住中间拖拽移动端点\n"
				<<"点击右键清屏或切换画线算法\n";
		}

		static void init (int x, int y,int slengthi) {
			/*禁止重复初始化*/
			if (_inited) {
				cout << "已经初始化！\n";
				return;
			}
			_inited = true;

			/*初始化像素矩阵*/
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

			/*初始化gl*/
			int x1 = 0;
			char** x2 = 0;
			glutInit (&x1, x2);

			glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
			glutInitWindowPosition (700, 100);
			glutInitWindowSize (_SCREENX * _SLENGTH, _SCREENY * _SLENGTH);
			glutCreateWindow ("图形学实验面板");
			gluOrtho2D (0, _SCREENX, 0, _SCREENY);

			/*注册各种自定义鼠标函数*/
			glutTimerFunc (50, &_timeCtr, 1);
			glutMouseFunc (_mouseButton);
			glutMotionFunc (_mouseMove);

			/*注册右键菜单栏*/
			glutCreateMenu (_menuCtr);
			glutAddMenuEntry ("clear", CLEAR);
			glutAddMenuEntry ("---------", 999);
			glutAddMenuEntry ("use CP", CP);
			glutAddMenuEntry ("use DE", BR);
			glutAddMenuEntry ("use DDA", DDA);
			glutAttachMenu (GLUT_RIGHT_BUTTON);

			/*开始主循环*/
			glutMainLoop ();
		}

		/*向像素数组绘制一个点*/
		static void drawPixel (int x, int y, GLfloat color = 1.0f) {
			if (x >= _SCREENX || y > _SCREENY || x < 0 || y < 0) {
				return;
				//throw INDEX_OUT_OF_RANGE;
			}
			_drawPixel (x, y, color);
		}


		/*获取一些基本信息*/
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