#pragma once
#include"lines.h"
#include"pannal.h"
namespace TA{
	template<class T>
	void swap (T& a, T& b) {
		T c (std::move (a)); a = std::move (b); b = std::move (c);
	}
	void drawlineDDAr (int x1, int y1, int x2, int y2) {
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		int x;
		float y, k;
		k = float (y2 - y1) / float (x2 - x1);
		y = (float)y1;

		for (x = x1; x <= x2; x++) {
			pannal::drawPixel (int (y + 0.5), x, 1);
			y = y + k;
		}
	}

	void drawlineDDA (int x1, int y1, int x2, int y2) {
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		if (float (y2 - y1) / float (x2 - x1) > 1) {
			drawlineDDAr (y1, x1, y2, x2);
			return;
		}
		if (float (y2 - y1) / float (x2 - x1) < -1) {
			drawlineDDAr (y2, x2, y1, x1);
			return;
		}
		int x;
		float y, k;
		k = float (y2 - y1) / float (x2 - x1);
		y = (float)y1;
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		for (x = x1; x <= x2; x++) {
			pannal::drawPixel (x, int (y + 0.5), 1);
			y = y + k;
		}
	}

	void drawlinecpr (int x1, int y1, int x2, int y2) {
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		int d1, d2, d, x, y;
		d = 2 * (y1 - y2) + (x2 - x1);
		d1 = 2 * (y1 - y2);
		d2 = 2 * (y1 - y2 + x2 - x1);
		x = x1;
		y = y1;
		for (x = x1; x <= x2; x++) {
			pannal::drawPixel (y, x, 1);
			if (d < 0) {
				y++;
				d += d2;
			} else {
				d += d1;
			}
		}
	}
	void drawlinecpyr (int x1, int y1, int x2, int y2) {
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		int d1, d2, d, x, y;
		d = 2 * (y1 - y2) + (x2 - x1);
		d1 = 2 * (y1 - y2);
		d2 = 2 * (y1 - y2 + x2 - x1);
		x = x1;
		y = y1;
		for (x = x1; x <= x2; x++) {
			pannal::drawPixel (pannal::getX() - y, x, 1);
			if (d < 0) {
				y++;
				d += d2;
			} else {
				d += d1;
			}
		}
	}
	void drawlinecpy (int x1, int y1, int x2, int y2) {
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		float k = float (y2 - y1) / float (x2 - x1);
		if (k > 1) {
			drawlinecpyr (y1, x1, y2, x2);
			return;
		}
		if (k < -1) {
			drawlinecpyr (y1, x1, y2, x2);
			return;
		}
		int d1, d2, d, x, y;
		d = 2 * (y1 - y2) + (x2 - x1);
		d1 = 2 * (y1 - y2);
		d2 = 2 * (y1 - y2 + x2 - x1);
		x = x1;
		y = y1;
		for (x = x1; x <= x2; x++) {
			pannal::drawPixel (pannal::getX() - x, y, 1);
			if (d < 0) {
				y++;
				d += d2;
			} else {
				d += d1;
			}
		}
	}
	void drawlinecp (int x1, int y1, int x2, int y2) {
		if (x1 > x2) {
			swap (x1, x2);
			swap (y1, y2);
		}
		if (y1 > y2) {
			drawlinecpy (pannal::getX() - x1, y1, pannal::getX() - x2, y2);
			return;
		}
		float k = float (y2 - y1) / float (x2 - x1);
		if (k > 1) {
			drawlinecpr (y1, x1, y2, x2);
			return;
		}
		if (k < -1) {
			drawlinecpr (y1, x1, y2, x2);
			return;
		}
		int d1, d2, d, x, y;
		d = 2 * (y1 - y2) + (x2 - x1);
		d1 = 2 * (y1 - y2);
		d2 = 2 * (y1 - y2 + x2 - x1);
		x = x1;
		y = y1;
		for (x = x1; x <= x2; x++) {
			pannal::drawPixel (x, y, 1);
			if (d < 0) {
				y++;
				d += d2;
			} else {
				d += d1;
			}
		}
	}

	//网络	布兰森汉姆;
	/*通过Bresenham算法绘制直线*/
	//应用最广泛
	//采用每次d加k并更新d的增量计算
	//为免于每次对d进行四舍五入，采用e=d-0.5
	//为免于浮点运算等，次用对e，d，k等乘以2dx，以将所有计算转化为整数
	void drawBresenham (int x1, int y11, int x2, int y2) {

		auto drawpoint = [] (int x, int y, int k_kind, int d_kind)->void{
			if (k_kind == 0 && d_kind == 1) {
				y = -y;
			} else if (k_kind == 1 && d_kind == 1) {
				x = -x;
				swap (x, y);
			} else if (k_kind == 1 && d_kind == 0) {
				swap (x, y);
			}
			pannal::drawPixel (x, y);
		};
		/*
		 函数说明：bresenham算法部分
		 参数说明：与openGL已有的划线函数一样，要求用户提供的是点的起点（x1,y11）和终点(x2,y2)
		 为了便于观察，我们会绘制原像素下的直线。
		 这里的坐标要求是-1 ～ 1
		 */
		int k_kind = 0; //k_kind用来表示斜率的类型。0是0～1；1是1～无穷；2是0～-1；3是负无穷～-1
		int d_kind = 0; //d_kind用来表示dy正负的类型。
		if (x1 > x2) {
			swap (x1, x2);
			swap (y11, y2);
		}
		int dx = abs (x2 - x1), dy = abs (y2 - y11);
		if (y11 > y2) {//如果是向下的
			y11 = -y11;
			y2 = -y2;
			d_kind = 1;
		}
		if (dy > dx) { //斜率介于1～无穷的，将看作坐标系变换（这里将坐标变换）。
			swap (x1, y11);
			swap (x2, y2);
			swap (dx, dy);
			k_kind = 1;
		}
		float d = (dy + dy - dx);     //令d为决策量(这里利用d = dx*w*2避免浮点运算)
		float x = x1 + 0.0, y = y11 + 0.0;
		drawpoint ( (x),  (y), k_kind, d_kind); //绘制下一个点
		while (x < x2) {             //以x为步长
			if (d < 0) {
				d += 2 * dy ;
			} else {
				d += 2 * (dy - dx);
				y += 1;    //说明应该画在上面那个位置
			}
			x = x + 1;
			drawpoint ( (x),  (y), k_kind, d_kind); //绘制下一个点
		}
	}
}
