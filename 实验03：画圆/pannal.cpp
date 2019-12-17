#include"pannal.h"
namespace TA {
	/*对各种静态变量赋初值*/
	int pannal::_SLENGTH = 5;
	pannal::state pannal::_state= NO_ACTION;
	int pannal::_SCREENY = 0;
	int pannal::_SCREENX = 0;
	GLfloat** pannal::_pix_arr = 0;
	bool pannal::_inited = false;
	point pannal::_c_point = point ();
	point pannal::_r_point = point ();
	point* pannal::_chosed_point = nullptr;
	pannal::menufunc pannal::_algorithm= pannal::BER;			//用于保存画圆的算法种类

	std::vector<  line  > pannal::_circles = std::vector<  line  > ();
}