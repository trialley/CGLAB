#pragma once
#include"circle.h"
#include"pannal.h"
namespace TA{
		/*构造判别函数：
		F(x, y）= x2 + y2 – R2
		当F(x, y）= 0，表示点在圆上，当F(x, y）> 0，表示点在圆外，当F(x, y）< 0，
		表示点在圆内。如果M是P1和P2的中点，则M的坐标是（xi + 1, yi – 0.5），当F（xi + 1, yi – 0.5）< 0时，
		M点在圆内，说明P1点离实际圆弧更近，应该取P1作为圆的下一个点。同理分析，当F（xi + 1, yi – 0.5）> 0时，
		P2离实际圆弧更近，应取P2作为下一个点。当F（xi + 1, yi – 0.5）= 0时，P1和P2都可以作为圆的下一个点，
		算法约定取P2作为下一个点。
		现在将M点坐标（xi + 1, yi – 0.5）带入判别函数F(x, y），得到判别式d：
		d = F（xi + 1, yi – 0.5）= (xi + 1)2 + (yi – 0.5)2 – R2
		若d < 0，则取P1为下一个点，此时P1的下一个点的判别式为：
		d’ = F（xi + 2, yi – 0.5）= (xi + 2)2 + (yi – 0.5)2 – R2
		展开后将d带入可得到判别式的递推关系：
		d’ = d + 2xi + 3
		若d > 0，则取P2为下一个点，此时P2的下一个点的判别式为：
		d’ = F（xi + 2, yi – 1.5）= (xi + 2)2 + (yi – 1.5)2 – R2
		展开后将d带入可得到判别式的递推关系：
		d’ = d + 2(xi - yi) + 5
		特别的，在第一个象限的第一个点（0, R）时，可以推倒出判别式d的初始值d0：
		d0 = F(1, R – 0.5) = 1 – (R – 0.5)2 – R2 = 1.25 - R
		*/
		void drawCircleMidPoint (int x0, int y0, int r) {
			/*用于绘制对称点的内部函数*/
			auto wholeC = [](int xc, int yc, int x, int y, GLfloat color)->void {
				pannal::drawPixel (xc + x, yc + y, color); pannal::drawPixel (xc + x, yc - y, color);
				pannal::drawPixel (xc - x, yc + y, color); pannal::drawPixel (xc - x, yc - y, color);
				pannal::drawPixel (xc + y, yc + x, color); pannal::drawPixel (xc + y, yc - x, color);
				pannal::drawPixel (xc - y, yc + x, color); pannal::drawPixel (xc - y, yc - x, color);
			};

			/*x，y是要绘制的点的位置*/
			int x = 0;
			int y = r;
			GLfloat d = GLfloat (1.25) - r;
			wholeC (x0, y0, x, y, 1);

			/*只形成（0，r）开始的八分之一个圆弧，其余部分通过wholeC补全*/
			while (x <= y) {
				if (d < 0) {
					d += 2 * x + 3;
				} else {
					d += 2 * (x - y) + 5;
					y--;
				}
				x++;
				wholeC (x0, y0, x, y, 1);
			}

		}

		/*还有一种方法是将d的计算放大两倍，同时将初始值改成3 – 2R，这样避免了浮点运算，
		乘二运算也可以用移位快速代替，采用3 – 2R为初始值的改进算法，又称为Bresenham算法：*/
		void drawCircleBresenham (int x0, int y0, int r) {
			/*用于绘制对称点的内部函数*/
			auto wholeC = [](int xc, int yc, int x, int y, GLfloat color)->void {
				pannal::drawPixel (xc + x, yc + y, color); pannal::drawPixel (xc + x, yc - y, color);
				pannal::drawPixel (xc - x, yc + y, color); pannal::drawPixel (xc - x, yc - y, color);
				pannal::drawPixel (xc + y, yc + x, color); pannal::drawPixel (xc + y, yc - x, color);
				pannal::drawPixel (xc - y, yc + x, color); pannal::drawPixel (xc - y, yc - x, color);
			};

			/*x，y是要绘制的点的位置*/
			int x = 0;
			int y = r;
			int d = 3 - 2 * r;/*GLfloat (1.25) - r;*/
			wholeC (x0, y0, x, y, 1);

			/*只形成（0，r）开始的八分之一个圆弧，其余部分通过wholeC补全*/
			while (x <= y) {
				if (d < 0) {
					d += (2 * x + 3) * 2;
				} else {
					d += (2 * (x - y) + 5) * 2;
					y--;
				}
				x++;
				wholeC (x0, y0, x, y, 1);
			}

		}


}
