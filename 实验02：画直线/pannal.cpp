#include"pannal.h"
namespace TA {
	/*对各种静态变量赋初值*/
	int pannal::_SLENGTH = 5;
	pannal::state pannal::_state= ON_ACTION;
	int pannal::_SCREENY = 0;
	int pannal::_SCREENX = 0;
	GLfloat** pannal::_pix_arr = 0;
	bool pannal::_inited = false;
	point pannal::_start_point = point ();
	point pannal::_end_point = point ();
	point* pannal::_chosed_point = nullptr;
	pannal::menufunc pannal::_algorithm= pannal::menufunc::DDA;

	std::vector<  line  > pannal::_lines = std::vector<  line  > ();
}