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
				//����ʰȡ����
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

	//������δ�����п��ޣ�ֻ��Ϊ��ȷ��������Ժ�ʵ�ʻ�ͼ�õ���ͬһ��ͶӰ��
	glLoadIdentity (); // ���õ�ǰָ���ľ���Ϊ��λ����
	gluPerspective // ����͸��ͼ
	(53.0f, // ͸�ӽ�����Ϊ 90 ��
		(GLfloat)Width / (GLfloat)Height,// ���ڵĿ���߱�
		0.1f, // ��Ұ͸�����:����1.0f
		3000.0f // ��Ұ͸�����:ʼ��0.1fԶ��1000.0f
	);


	glPushMatrix ();

	glPushMatrix ();
	//����Ĵ�������ؼ��ĵط�����ע�⣺�ݲ��ԣ�gluPickMatrix()��˼�����ǽ�������һ��ʰȡ���ڡ�ͬʱ��  

	//����Ĵ�������ؼ��ĵط�����ע�⣺�ݲ��ԣ�gluPickMatrix()��˼�����ǽ�������һ��ʰȡ���ڡ�ͬʱ��Cpp����  
	//��Ҳ��һ��ͶӰ����任���������������ʹ�õڶ���LoadIdentity()��ô��ǰ���ʰȡ����ͻ�ʧЧ����  

	//��Ҳ��һ��ͶӰ����任���������������ʹ�õڶ���LoadIdentity()��ô��ǰ���ʰȡ����ͻ�ʧЧ����Cpp����  


	glLoadIdentity ();
	create 5x5 pixel picking region near cursor location * /
		gluPickMatrix ((GLdouble)mouse.x, (GLdouble)(viewport[3] - mouse.y),
			5.0, 5.0, viewport);

	glLoadIdentity ();
	/*  create 5x5 pixel picking region near cursor location */
	gluPickMatrix ((GLdouble)mouse.x, (GLdouble)(viewport[3] - mouse.y),
		5.0, 5.0, viewport); Cpp����
		//������ʹ�����Ǹ�PickMatrixʧЧ�����мǡ�������
		glLoadIdentity (); // ���õ�ǰָ���ľ���Ϊ��λ����   
	gluPerspective                           // ����͸��ͼ   
	(53.0f,                            // ͸�ӽ�����Ϊ 90 ��   
		(GLfloat)Width / (GLfloat)Height,   // ���ڵĿ���߱�   
		0.1f,                             // ��Ұ͸�����:����1.0f   
		3000.0f                           // ��Ұ͸�����:ʼ��0.1fԶ��1000.0f   
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
	Font->c3dtext ("�뿪��Ϸ", hFont, 0.25f);
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
	Font->c3dtext ("��ʼ��Ϸ", hFont, 0.25f);
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
	Font->c3dtext("�뿪��Ϸ",hFont,0.25f);
	/*glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// ǰ
		glTexCoord2f(1.0f, 0.0f); glVertex3f( Width/2,-Height/2,  -0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( Width/2,  Height/2,  -0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-Width/2,  Height/2,  -0.0f);
	glEnd();

 //   glColor3f(1.0,0.0,0.0);
//  glTranslatef(-2,1,-15);
	glColor3f(0.6f,0.2f,0.9f);
//  Font->c3dtext("��ʼ��Ϸ",hFont,0.25f);
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
	Font->c3dtext("�뿪��Ϸ",hFont,0.25f);

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// ǰ
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

//������ʹ�����Ǹ�PickMatrixʧЧ�����мǡ�������
   //glLoadIdentity();						// ���õ�ǰָ���ľ���Ϊ��λ����
//gluPerspective							// ����͸��ͼ
//(53.0f,							// ͸�ӽ�����Ϊ 90 ��
//(GLfloat)Width / (GLfloat)Height,	// ���ڵĿ���߱�
//0.1f,								// ��Ұ͸�����:����1.0f
//3000.0f							// ��Ұ͸�����:ʼ��0.1fԶ��1000.0f
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
	Font->c3dtext ("�뿪��Ϸ", hFont, 0.25f);
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
	Font->c3dtext ("��ʼ��Ϸ", hFont, 0.25f);
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
	Font->c3dtext("�뿪��Ϸ",hFont,0.25f);
	/*glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// ǰ
		glTexCoord2f(1.0f, 0.0f); glVertex3f( Width/2,-Height/2,  -0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( Width/2,  Height/2,  -0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-Width/2,  Height/2,  -0.0f);
	glEnd();
 //   glColor3f(1.0,0.0,0.0);
//	glTranslatef(-2,1,-15);
	glColor3f(0.6f,0.2f,0.9f);
//	Font->c3dtext("��ʼ��Ϸ",hFont,0.25f);
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
	Font->c3dtext("�뿪��Ϸ",hFont,0.25f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-Width/2, -Height/2,  -0.0f);// ǰ
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
