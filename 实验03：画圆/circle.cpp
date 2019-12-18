#pragma once
#include"circle.h"
#include"pannal.h"
namespace TA{
		/*�����б�����
		F(x, y��= x2 + y2 �C R2
		��F(x, y��= 0����ʾ����Բ�ϣ���F(x, y��> 0����ʾ����Բ�⣬��F(x, y��< 0��
		��ʾ����Բ�ڡ����M��P1��P2���е㣬��M�������ǣ�xi + 1, yi �C 0.5������F��xi + 1, yi �C 0.5��< 0ʱ��
		M����Բ�ڣ�˵��P1����ʵ��Բ��������Ӧ��ȡP1��ΪԲ����һ���㡣ͬ���������F��xi + 1, yi �C 0.5��> 0ʱ��
		P2��ʵ��Բ��������ӦȡP2��Ϊ��һ���㡣��F��xi + 1, yi �C 0.5��= 0ʱ��P1��P2��������ΪԲ����һ���㣬
		�㷨Լ��ȡP2��Ϊ��һ���㡣
		���ڽ�M�����꣨xi + 1, yi �C 0.5�������б���F(x, y�����õ��б�ʽd��
		d = F��xi + 1, yi �C 0.5��= (xi + 1)2 + (yi �C 0.5)2 �C R2
		��d < 0����ȡP1Ϊ��һ���㣬��ʱP1����һ������б�ʽΪ��
		d�� = F��xi + 2, yi �C 0.5��= (xi + 2)2 + (yi �C 0.5)2 �C R2
		չ����d����ɵõ��б�ʽ�ĵ��ƹ�ϵ��
		d�� = d + 2xi + 3
		��d > 0����ȡP2Ϊ��һ���㣬��ʱP2����һ������б�ʽΪ��
		d�� = F��xi + 2, yi �C 1.5��= (xi + 2)2 + (yi �C 1.5)2 �C R2
		չ����d����ɵõ��б�ʽ�ĵ��ƹ�ϵ��
		d�� = d + 2(xi - yi) + 5
		�ر�ģ��ڵ�һ�����޵ĵ�һ���㣨0, R��ʱ�������Ƶ����б�ʽd�ĳ�ʼֵd0��
		d0 = F(1, R �C 0.5) = 1 �C (R �C 0.5)2 �C R2 = 1.25 - R
		*/
		void drawCircleMidPoint (int x0, int y0, int r) {
			/*���ڻ��ƶԳƵ���ڲ�����*/
			auto wholeC = [](int xc, int yc, int x, int y, GLfloat color)->void {
				pannal::drawPixel (xc + x, yc + y, color); pannal::drawPixel (xc + x, yc - y, color);
				pannal::drawPixel (xc - x, yc + y, color); pannal::drawPixel (xc - x, yc - y, color);
				pannal::drawPixel (xc + y, yc + x, color); pannal::drawPixel (xc + y, yc - x, color);
				pannal::drawPixel (xc - y, yc + x, color); pannal::drawPixel (xc - y, yc - x, color);
			};

			/*x��y��Ҫ���Ƶĵ��λ��*/
			int x = 0;
			int y = r;
			GLfloat d = GLfloat (1.25) - r;
			wholeC (x0, y0, x, y, 1);

			/*ֻ�γɣ�0��r����ʼ�İ˷�֮һ��Բ�������ಿ��ͨ��wholeC��ȫ*/
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

		/*����һ�ַ����ǽ�d�ļ���Ŵ�������ͬʱ����ʼֵ�ĳ�3 �C 2R�����������˸������㣬
		�˶�����Ҳ��������λ���ٴ��棬����3 �C 2RΪ��ʼֵ�ĸĽ��㷨���ֳ�ΪBresenham�㷨��*/
		void drawCircleBresenham (int x0, int y0, int r) {
			/*���ڻ��ƶԳƵ���ڲ�����*/
			auto wholeC = [](int xc, int yc, int x, int y, GLfloat color)->void {
				pannal::drawPixel (xc + x, yc + y, color); pannal::drawPixel (xc + x, yc - y, color);
				pannal::drawPixel (xc - x, yc + y, color); pannal::drawPixel (xc - x, yc - y, color);
				pannal::drawPixel (xc + y, yc + x, color); pannal::drawPixel (xc + y, yc - x, color);
				pannal::drawPixel (xc - y, yc + x, color); pannal::drawPixel (xc - y, yc - x, color);
			};

			/*x��y��Ҫ���Ƶĵ��λ��*/
			int x = 0;
			int y = r;
			int d = 3 - 2 * r;/*GLfloat (1.25) - r;*/
			wholeC (x0, y0, x, y, 1);

			/*ֻ�γɣ�0��r����ʼ�İ˷�֮һ��Բ�������ಿ��ͨ��wholeC��ȫ*/
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
