#pragma once
namespace TA {
	typedef struct point {
		int x;
		int y;
	}point;
	typedef struct line {
		point start;
		point end;
		line (int x1i, int y1i, int x2i, int y2i) {
			start.x = x1i;
			start.y = y1i;
			end.x = x2i;
			end.y = y2i;
		}
		line(point starti,point endi):start(starti), end(endi){}
	}line;
};