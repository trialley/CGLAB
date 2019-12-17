#include "glut.h"
#include<GL/GLU.h>
#include<GL/GL.h>
void pickRects () {
	/*	GetCursorPos(&mouse);
		ScreenToClient(hWnd,&mouse);
		GLuint pickBuffer [32];
		GLint nPicks,vpArray[4];
		glRenderMode(GL_SELECT);
			glSelectBuffer(32,pickBuffer);
			glInitNames();
			//glPushName(-1);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
				//glLoadIdentity();
				glGetIntegerv(GL_VIEWPORT,vpArray);
				gluPickMatrix(GLdouble(mouse.x),GLdouble(vpArray[3]-mouse.y),5.0,5.0,vpArray);
			//	glOrtho(0,1,0,1,0,1);
				//激活拾取区域。
			//	gluOrtho2D(0,0,Width,Height);

			rects(GL_SELECT);
			glMatrixMode(GL_PROJECTION);
			int j;
			glGetIntegerv(GL_NAME_STACK_DEPTH,&j);
			cout<<j<<endl;
			glPopMatrix();
			glFlush();
			nPicks=glRenderMode(GL_RENDER);
			processPicks(nPicks,pickBuffer);
			//processPicks(nPicks,pickBuffer);
			*/
	GetCursorPos (&mouse);
	ScreenToClient (hWnd, &mouse);
	GLuint selectBuf[32];
	GLint hits;
	GLint viewport[4];

	glGetIntegerv (GL_VIEWPORT, viewport);

	glSelectBuffer (32, selectBuf);
	(void)glRenderMode (GL_SELECT);
	glInitNames ();
	glPushName (0);
	glMatrixMode (GL_PROJECTION);

	//下面这段代码可有可无，只是为了确保点击测试和实际绘图用的是同一个投影阵
	glLoadIdentity (); // 重置当前指定的矩阵为单位矩阵
	gluPerspective // 设置透视图
	(53.0f, // 透视角设置为 90 度
		(GLfloat)Width / (GLfloat)Height,// 窗口的宽与高比
		0.1f, // 视野透视深度:近点1.0f
		3000.0f // 视野透视深度:始点0.1f远点1000.0f
	);


	glPushMatrix ();

	glPushMatrix ();
	//下面的代码是最关键的地方，，注意：据测试：gluPickMatrix()意思并不是仅仅绘制一个拾取窗口。同时，  

	//下面的代码是最关键的地方，，注意：据测试：gluPickMatrix()意思并不是仅仅绘制一个拾取窗口。同时，Cpp代码  
	//它也是一个投影矩阵变换！！～～～～如果使用第二个LoadIdentity()那么：前面的拾取矩阵就会失效！！  

	//它也是一个投影矩阵变换！！～～～～如果使用第二个LoadIdentity()那么：前面的拾取矩阵就会失效！！Cpp代码  


	glLoadIdentity ();
	create 5x5 pixel picking region near cursor location * /
		gluPickMatrix ((GLdouble)mouse.x, (GLdouble)(viewport[3] - mouse.y),
			5.0, 5.0, viewport);

	glLoadIdentity ();
	/*  create 5x5 pixel picking region near cursor location */
	gluPickMatrix ((GLdouble)mouse.x, (GLdouble)(viewport[3] - mouse.y),
		5.0, 5.0, viewport); Cpp代码
		//这个会促使上面那个PickMatrix失效！！切记～～～～
		glLoadIdentity (); // 重置当前指定的矩阵为单位矩阵   
	gluPerspective                           // 设置透视图   
	(53.0f,                            // 透视角设置为 90 度   
		(GLfloat)Width / (GLfloat)Height,   // 窗口的宽与高比   
		0.1f,                             // 视野透视深度:近点1.0f   
		3000.0f                           // 视野透视深度:始点0.1f远点1000.0f   
	); < / SPAN>
		rects (GL_SELECT);
	glPopMatrix ();
	glFlush ();
	hits = glRenderMode (GL_RENDER);
	processPicks (hits, selectBuf);
	int j;
	glGetIntegerv (GL_NAME_STACK_DEPTH, &j);
	int m;
}
void rects (GLenum mode) {
	if (mode == GL_SELECT)
		glLoadName (30);
	glPushMatrix ();
	glTranslatef (-2, -1, -15);
	glColor3f (0.6f, 0.2f, 0.9f);
	//  glRecti(0,0,100,100);   
	Font->c3dtext ("离开游戏", hFont, 0.25f);
	glPopMatrix ();
	/* glBegin(GL_QUADS);
	 glColor3f(1.0, 1.0, 0.0);
	 glVertex3i(2, 0, 0);
	 glVertex3i(2, 6, 0);
	 glVertex3i(6, 6, 0);
	 glVertex3i(6, 0, 0);
	 glEnd();
	 */
	if (mode == GL_SELECT)
		glLoadName (20);
	glPushMatrix ();
	glTranslatef (-2, 1, -15);
	glColor3f (0.6f, 0.2f, 0.9f);
	Font->c3dtext ("开始游戏", hFont, 0.25f);
	glPopMatrix ();
	/*glBegin(GL_QUADS);
 glColor3f(0.0, 1.0, 1.0);
 glVertex3i(3, 2, -1);
 glVertex3i(3, 8, -1);
 glVertex3i(8, 8, -1);
 glVertex3i(8, 2, -1);
 glEnd();
 */
 /*
 if (mode == GL_SELECT)
	glLoadName(3);
 glBegin(GL_QUADS);
 glColor3f(1.0, 0.0, 1.0);
 glVertex3i(0, 2, -2);
 glVertex3i(0, 7, -2);
 glVertex3i(5, 7, -2);
 glVertex3i(5, 2, -2);
 glEnd();
  /*
 if(mode==GL_SELECT)
  glPushName(1);
  glPushMatrix();
//  glTranslatef(0,0,-Height);
	glColor3f(0.0,1.0,0.0);
//  glPushName(20);
	glTranslatef(-2,-1,-15);
//  glColor3f(0.6f,0.2f,0.9f);
//  glRecti(0,0,100,100);
	Font->c3dtext("离开游戏",hFont,0.25f);
	/*glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// 前
		glTexCoord2f(1.0f, 0.0f); glVertex3f( Width/2,-Height/2,  -0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( Width/2,  Height/2,  -0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-Width/2,  Height/2,  -0.0f);
	glEnd();

 //   glColor3f(1.0,0.0,0.0);
//  glTranslatef(-2,1,-15);
	glColor3f(0.6f,0.2f,0.9f);
//  Font->c3dtext("开始游戏",hFont,0.25f);
	glPopMatrix();
   if(mode==GL_SELECT)
	glPushName(2);
	glPushMatrix();
   // glTranslatef(0,0,-Height);
//  glColor3f(0.0,1.0,0.0);
//  glPushName(20);
	glTranslatef(-2,-1,-15);
	glColor3f(0.6f,0.2f,0.9f);
//  glRecti(0,0,100,100);
	Font->c3dtext("离开游戏",hFont,0.25f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// 前
		glTexCoord2f(1.0f, 0.0f); glVertex3f( Width/2,-Height/2,  -0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( Width/2,  Height/2,  -0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-Width/2,  Height/2,  -0.0f);
	glEnd();

	glPopMatrix();
	*/
}
void processPicks (GLint hits, GLuint buffer[]) {
	rects (GL_RENDER);
	cout << hits << endl;
	unsigned int i, j;
	GLuint names, * ptr;
	printf ("hits = %d\n", hits);
	ptr = (GLuint*)buffer;
	for (i = 0; i < hits; i++) {
		names = *ptr;
		printf (" number of names for hit = %d\n", names); ptr++;
		printf ("  z1 is %g;", (float)*ptr / 0x7fffffff); ptr++;
		printf (" z2 is %g\n", (float)*ptr / 0x7fffffff); ptr++;
		printf ("   the name is ");
		for (j = 0; j < names; j++) {
			printf ("%d ", *ptr); ptr++;
		}
		printf ("\n");
	}
}

//这个会促使上面那个PickMatrix失效！！切记～～～～
   //glLoadIdentity();						// 重置当前指定的矩阵为单位矩阵
//gluPerspective							// 设置透视图
//(53.0f,							// 透视角设置为 90 度
//(GLfloat)Width / (GLfloat)Height,	// 窗口的宽与高比
//0.1f,								// 视野透视深度:近点1.0f
//3000.0f							// 视野透视深度:始点0.1f远点1000.0f
//);
//rects (GL_SELECT);
//glPopMatrix ();
//glFlush ();
//hits = glRenderMode (GL_RENDER);
//processPicks (hits, selectBuf);
//int j;
//glGetIntegerv (GL_NAME_STACK_DEPTH, &j);
//int m;
//}
void rects (GLenum mode) {
	if (mode == GL_SELECT)
		glLoadName (30);
	glPushMatrix ();
	glTranslatef (-2, -1, -15);
	glColor3f (0.6f, 0.2f, 0.9f);
	//	glRecti(0,0,100,100);
	Font->c3dtext ("离开游戏", hFont, 0.25f);
	glPopMatrix ();
	/* glBegin(GL_QUADS);
	 glColor3f(1.0, 1.0, 0.0);
	 glVertex3i(2, 0, 0);
	 glVertex3i(2, 6, 0);
	 glVertex3i(6, 6, 0);
	 glVertex3i(6, 0, 0);
	 glEnd();
	 */
	if (mode == GL_SELECT)
		glLoadName (20);
	glPushMatrix ();
	glTranslatef (-2, 1, -15);
	glColor3f (0.6f, 0.2f, 0.9f);
	Font->c3dtext ("开始游戏", hFont, 0.25f);
	glPopMatrix ();
	/*glBegin(GL_QUADS);
 glColor3f(0.0, 1.0, 1.0);
 glVertex3i(3, 2, -1);
 glVertex3i(3, 8, -1);
 glVertex3i(8, 8, -1);
 glVertex3i(8, 2, -1);
 glEnd();
 */
 /*
 if (mode == GL_SELECT)
	glLoadName(3);
 glBegin(GL_QUADS);
 glColor3f(1.0, 0.0, 1.0);
 glVertex3i(0, 2, -2);
 glVertex3i(0, 7, -2);
 glVertex3i(5, 7, -2);
 glVertex3i(5, 2, -2);
 glEnd();
  /*
 if(mode==GL_SELECT)
  glPushName(1);
  glPushMatrix();
//	glTranslatef(0,0,-Height);
	glColor3f(0.0,1.0,0.0);
//	glPushName(20);
	glTranslatef(-2,-1,-15);
//	glColor3f(0.6f,0.2f,0.9f);
//	glRecti(0,0,100,100);
	Font->c3dtext("离开游戏",hFont,0.25f);
	/*glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// 前
		glTexCoord2f(1.0f, 0.0f); glVertex3f( Width/2,-Height/2,  -0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( Width/2,  Height/2,  -0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-Width/2,  Height/2,  -0.0f);
	glEnd();
 //   glColor3f(1.0,0.0,0.0);
//	glTranslatef(-2,1,-15);
	glColor3f(0.6f,0.2f,0.9f);
//	Font->c3dtext("开始游戏",hFont,0.25f);
	glPopMatrix();
   if(mode==GL_SELECT)
	glPushName(2);
	glPushMatrix();
   // glTranslatef(0,0,-Height);
//	glColor3f(0.0,1.0,0.0);
//	glPushName(20);
	glTranslatef(-2,-1,-15);
	glColor3f(0.6f,0.2f,0.9f);
//	glRecti(0,0,100,100);
	Font->c3dtext("离开游戏",hFont,0.25f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// 前
		glTexCoord2f(1.0f, 0.0f); glVertex3f( Width/2,-Height/2,  -0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( Width/2,  Height/2,  -0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-Width/2,  Height/2,  -0.0f);
	glEnd();
	glPopMatrix();
	*/
}
void processPicks (GLint hits, GLuint buffer[]) {
	rects (GL_RENDER);
	cout << hits << endl;
	unsigned int i, j;
	GLuint names, * ptr;
	printf ("hits = %d\n", hits);
	ptr = (GLuint*)buffer;
	for (i = 0; i < hits; i++) {
		names = *ptr;
		printf (" number of names for hit = %d\n", names); ptr++;
		printf ("  z1 is %g;", (float)*ptr / 0x7fffffff); ptr++;
		printf (" z2 is %g\n", (float)*ptr / 0x7fffffff); ptr++;
		printf ("   the name is ");
		for (j = 0; j < names; j++) {
			printf ("%d ", *ptr); ptr++;
		}
		printf ("\n");
	}
}
