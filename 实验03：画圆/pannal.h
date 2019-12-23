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
		/*一系列自定义类型*/
		enum err { INDEX_OUT_OF_RANGE };
		typedef enum state { NO_ACTION, ADDING_CIRCLE, MOVING_C_POINT , MOVING_R_POINT} state;
		typedef enum menufunc {NOFUNC = 999 , CLEAR,MID,BER} menufunc;

	private:
		/*禁止复制*/
		pannal (const pannal&) {}
	protected:
		/*保存屏幕的长宽信息*/
		static int _SCREENY;
		static int _SCREENX;
		static int _SLENGTH;

		/*用于左键添加圆时暂存圆的圆心位置和半径结束位置*/
		static point _c_point;
		static point _r_point;

		static enum state _state;			//用于保存当前状态：画线，移动点，无
		static GLfloat** _pix_arr;			//屏幕像素阵列
		static bool _inited;				//初始化与否标志
		static point* _chosed_point;		//指向被选中的点
		static std::vector<line> _circles;	//用于保存所有线段
		static menufunc _algorithm;			//用于保存画圆的算法种类

		/*计算两点距离*/
		static inline float _calDistance (float x1, float y1, float x2, float y2) {
			return pow ((pow ((x1 - x2), 2) + pow ((y1 - y2), 2)), 0.5);
		};
		static inline float _calDistance (point a1, point a2) {
			return pow ((pow ((a1.x - a2.x), 2) + pow ((a1.y - a2.y), 2)), 0.5);
		};

		/*向矩形数组中绘制一个点*/
		static void _drawPixel (int x, int y, GLfloat color = 1.0f) {
			_pix_arr[x][y] = color;
			glutPostRedisplay ();
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

		/*将多条线段绘制到像素数组中*/
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

		/*计时器归零回调函数*/
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
				dprintf ("未选中合适的点\n");
				return nullptr;
			}
			dprintf ("选中了点：(%d, %d)，于位置：%p\n", temp->x, temp->y,temp);
			return temp;
		}


		/*监听鼠标按键动作*/
		static void _mouseButton (int button, int state, int xi, int yi) {
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				//按下左键，开始画线
				_state = ADDING_CIRCLE;
				_c_point.x = x;
				_c_point.y = y;
				_r_point.x = x;
				_r_point.y = y;
				_circles.emplace_back (line (_c_point, _r_point));
			} else if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				if (_state == ADDING_CIRCLE) {
					//抬起左键，结束画线
					_state = NO_ACTION;
					dprintf ("线条已经添加：起点(%d, %d)，终点(%d, %d)\n", _c_point.x, _c_point.y, _r_point.x, _r_point.y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
				_chosed_point = _getClosetPoint (x, y);
				if (_chosed_point==nullptr) {
					_state = NO_ACTION;
					dprintf ("未选中合适的点\n");
				} else {
					dprintf ("位置移动开始，当前点位置(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			} else if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
				if (_state == MOVING_C_POINT) {
					_state = NO_ACTION;
					dprintf ("位置移动结束，当前点新位置(%d, %d)\n", _chosed_point->x, _chosed_point->y);
				}
			}
		}
		/*监听鼠标移动*/
		static void _mouseMove (int xi, int yi) {
			/*鼠标中间按下时，数组清空；若数组为空，则不能修改新线条末端位置*/
			int x = xi / _SLENGTH - 1;
			int y = _SCREENY - yi / _SLENGTH;
			if (_state == ADDING_CIRCLE) {
				_r_point.x = x;
				_r_point.y = y;
				_circles[_circles.size () - 1].end = _r_point;
				dprintf ("当前起点(%d, %d)， 当前终点(%d, %d)\n", _c_point.x, _c_point.y, _r_point.x, _r_point.y);
				return;
			} else if (_state == MOVING_C_POINT) {
				dprintf ("当前点位置(%d, %d)， ", _chosed_point->x, _chosed_point->y);
				int dx =x- _chosed_point->x;
				int dy =y- _chosed_point->y;
				_chosed_point->x = x;
				_chosed_point->y = y;
				(_chosed_point + 1)->x += dx;
				(_chosed_point + 1)->y += dy;
				dprintf ("当前点新位置(%d, %d)\n", _chosed_point->x, _chosed_point->y);
			} else if (_state == MOVING_R_POINT) {
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
				_state = NO_ACTION;
				_circles.clear ();
				dprintf ("清空所有记录\n");
				break;
			case BER:
				_algorithm = BER;
				cout << "切换为 Bresenham 算法\n";
				break;
			case MID:
				_algorithm = MID;
				cout << "切换为 Mid Point 算法\n";

				break;
			case NOFUNC:
			default:
				break;
			}
		}
	public:
		pannal (int x = 100, int y = 100, int slengthi = 5) {
			if (_inited) {
				cout << "已经初始化！\n";
				return;
			}
			std::thread t (pannal::init, x, y,  slengthi);
			t.detach ();
			cout << "按住左键拖拽添加圆\n"
				<<"按住中间拖拽移动端点\n"
				<<"点击右键清屏或切换画圆算法\n";
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
			glutAddMenuEntry ("use Bresenham", MID);
			glutAddMenuEntry ("use Mid Point", BER);
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