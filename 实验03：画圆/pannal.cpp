#include"pannal.h"
namespace TA {
	/*�Ը��־�̬��������ֵ*/
	int pannal::_SLENGTH = 5;
	pannal::state pannal::_state= NO_ACTION;
	int pannal::_SCREENY = 0;
	int pannal::_SCREENX = 0;
	GLfloat** pannal::_pix_arr = 0;
	bool pannal::_inited = false;
	point pannal::_c_point = point ();
	point pannal::_r_point = point ();
	point* pannal::_chosed_point = nullptr;
	pannal::menufunc pannal::_algorithm= pannal::BER;			//���ڱ��滭Բ���㷨����

	std::vector<  line  > pannal::_circles = std::vector<  line  > ();
}