#pragma once
#include"types.h"
#include<GL/glut.h>
namespace TA {
	//class pannal;
	void drawlineDDAr (int x1, int y1, int x2, int y2);
	void drawlineDDA (int x1, int y1, int x2, int y2);
	void drawlinecpr (int x1, int y1, int x2, int y2);
	void drawlinecp (int x1, int y1, int x2, int y2);

	void drawBresenham (int x1, int y11, int x2, int y2);

}
