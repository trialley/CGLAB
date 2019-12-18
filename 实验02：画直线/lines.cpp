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

	//����	����ɭ��ķ;
	/*ͨ��Bresenham�㷨����ֱ��*/
	//Ӧ����㷺
	//����ÿ��d��k������d����������
	//Ϊ����ÿ�ζ�d�����������룬����e=d-0.5
	//Ϊ���ڸ�������ȣ����ö�e��d��k�ȳ���2dx���Խ����м���ת��Ϊ����
	void drawBresenham (int x1, int y11, int x2, int y2) {

		/*��ԭ���ĸ��ַ�ת��������*/
		auto drawpoint = [] (int x, int y, int k_kind, int d_kind)->void{
			if (k_kind == 0 && d_kind == 1) {//���»�����y��ת��x��Գ�
				y = -y;
			} else if (k_kind == 1 && d_kind == 1) {//x��x=y�����η�ת
				swap (x, y);
				y = -y;
			} else if (k_kind == 1 && d_kind == 0) {//���ԭ��б�ʶ��ͣ���x=y��ת
				swap (x, y);
			}
			pannal::drawPixel (x, y);
		};

		int k_kind = 0; //k_kind 1��ʾб�ʾ���ֵ����1 ���ͣ�0��ʾС��1 ƽ��
		int d_kind = 0; //d_kind������ʾdy����������,Ҳ�������˳��Ĭ�����ϻ�
		if (x1 > x2) {
			swap (x1, x2);
			swap (y11, y2);
		}
		int dx = abs (x2 - x1);
		int dy = abs (y2 - y11);
		if (y11 > y2) {//��������»��ģ�����ת��Ϊ���ϻ���x��Գ�
			y11 = -y11;
			y2 = -y2;
			d_kind = 1;
		}
		if (dy > dx) { //���б�ʾ���ֵ����1����ʹ��������С��1��x=y�Գ�
			swap (x1, y11);
			swap (x2, y2);
			swap (dx, dy);
			k_kind = 1;
		}
		float d = (dy + dy - dx);     //��dΪ������(��������d = dx*w*2���⸡������)
		float x = x1 + 0.0, y = y11 + 0.0;
		drawpoint ( (x),  (y), k_kind, d_kind); //������һ����
		while (x < x2) {             //��xΪ����
			if (d < 0) {
				d += 2 * dy ;
			} else {
				d += 2 * (dy - dx);
				y += 1;    //˵��Ӧ�û��������Ǹ�λ��
			}
			x = x + 1;
			drawpoint ( (x),  (y), k_kind, d_kind); //������һ����
		}
	}
}
