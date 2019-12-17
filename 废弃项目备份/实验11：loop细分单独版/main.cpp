#include<iostream>
#include<fstream>
#include <math.h>
#include<string>
#include<queue>

#include "glut.h"
#pragma comment(lib, "./glut32.lib")
using namespace std;
int numOfVertex = 0;
int numOfFace = 0;
int numOfLine = 0;
int WIDTH = 600;
int HEIGHT = 600;
float maxx = -1;
float maxy = -1;
float maxz = -1;
float minx = 1;
float miny = 1;
float minz = 1;
GLfloat ShootPosition[] = { 0,0,0 };
GLfloat ShootDirect[] = { 0,0,0 };
float scale = 1.0;
float px;
float py;
float theta1 = 0;
float theta2 = 0;
float radius = 0;
bool light1 = false;
bool light2 = false;
bool light3 = false;
int displaystate = 0;
float PI = 3.1415926;
struct vertex;
struct face;
struct halfedge;
struct he_face;
struct normalVec;
struct nedge;
vertex* vertexs;
face* faces;
he_face** hefaces;
normalVec* normalvectors;
nedge** iedges;


struct halfedge {//半边结构
	halfedge* next;
	halfedge* opposite;
	int end;
	bool visit;
	he_face* face;
	halfedge () {
		next = NULL;
		opposite = NULL;
		end = -1;
		face = NULL;
		visit = false;
	}
};
struct vertex {//顶点
	float x;
	float y;
	float z;
	halfedge* edge;//顶点指向边
	bool visit;
	vertex () {
		visit = false;
	}
	vertex (float a, float b, float c, halfedge* edgei = nullptr):edge(edgei){
		x = a;
		y = b;
		z = c;
		visit = false;
	}
};
struct normalVec {//法向量
	float x;
	float y;
	float z;
	normalVec () {

	}
	normalVec (float a, float b, float c) {
		x = a;
		y = b;
		z = c;
	}
};
struct he_face {//半边面
	halfedge* edge;
	bool visit;
	he_face () {
		edge = NULL;
		visit = false;
	}
};
struct face {//面
	int numofv;
	int* vertexs;
	face () {

	}
	face (int nv) {
		numofv = nv;
		vertexs = new int[nv];
	}
};
struct nedge {
	int start;
	int middle;
	halfedge* he;
	nedge* next;
	nedge () {
		start = -1;
		he = NULL;
		next = NULL;
		middle = -1;
	}
};
void readFile () {//读取文件
	char data[100];
	ifstream infile;
	//cout << "输入要读取的文件名 :" << endl;
	string filename = "../off/bunny.off";
	//cin >> filename;
	infile.open (filename);
	infile >> data;
	infile >> numOfVertex;
	infile >> numOfFace;
	infile >> numOfLine;
	vertexs = new vertex[numOfVertex];
	faces = new face[numOfFace];
	int vnum = 0;
	int fnum = 0;
	while (vnum < numOfVertex) {//读顶点信息
		float x;
		float y;
		float z;
		infile >> x;
		infile >> y;
		infile >> z;
		vertexs[vnum] = vertex (x, y, z);
		vnum++;
	}
	while (fnum < numOfFace)//读面信息
	{
		int numofv;
		infile >> numofv;
		face f = face (numofv);
		for (int i = 0; i < numofv; i++) {
			int v;
			infile >> v;
			f.vertexs[i] = v;
		}
		faces[fnum] = f;
		fnum++;
	}
	infile.close ();
}
int getMiddle (int start, int end, nedge** iedges) {
	nedge* temp = iedges[start];
	while (temp != NULL) {
		if (temp->he->end == end) {
			break;
		}
		temp = temp->next;
	}
	if (temp == NULL) {
		return -1;
	} else {
		return temp->middle;
	}

}
halfedge* getHalfEdge (int start, int end, nedge** iedges) {
	nedge* temp = iedges[start];
	while (temp != NULL) {
		if (temp->he->end == end) {
			break;
		}
		temp = temp->next;
	}
	if (temp == NULL) {
		return NULL;
	} else {
		return temp->he;
	}

}
void initHalfedge () {//初始化半边结构
	hefaces = new he_face * [numOfFace];
	int numofhe = 0;
	for (int i = 0; i < numOfVertex; i++) {
		iedges[i] = NULL;
	}
	for (int i = 0; i < numOfFace; i++) {
		int v1 = faces[i].vertexs[0];
		int v2 = faces[i].vertexs[1];
		int v3 = faces[i].vertexs[2];
		he_face* hf = new he_face ();
		halfedge* he1 = getHalfEdge (v1, v2, iedges);
		halfedge* he2 = getHalfEdge (v2, v3, iedges);
		halfedge* he3 = getHalfEdge (v3, v1, iedges);
		if (he1 == NULL) {
			he1 = new halfedge ();
			he1->end = v2;
			halfedge* opposite1 = new halfedge ();
			opposite1->end = v1;
			opposite1->opposite = he1;
			he1->opposite = opposite1;
			nedge* temp = iedges[v1];
			nedge* tempi = new nedge ();
			tempi->start = v1;
			tempi->he = he1;
			if (temp == NULL) {
				iedges[v1] = tempi;
			} else {
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
			temp = iedges[v2];
			tempi = new nedge ();
			tempi->start = v2;
			tempi->he = opposite1;
			if (temp == NULL) {
				iedges[v2] = tempi;
			} else {
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
		}
		if (he2 == NULL) {
			he2 = new halfedge ();
			he2->end = v3;
			halfedge* opposite2 = new halfedge ();
			opposite2->end = v2;
			opposite2->opposite = he2;
			he2->opposite = opposite2;
			nedge* temp = iedges[v2];
			nedge* tempi = new nedge ();
			tempi->start = v2;
			tempi->he = he2;
			if (temp == NULL) {
				iedges[v2] = tempi;
			} else {
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
			temp = iedges[v3];
			tempi = new nedge ();
			tempi->start = v3;
			tempi->he = opposite2;
			if (temp == NULL) {
				iedges[v3] = tempi;
			} else {
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
		}
		if (he3 == NULL) {
			he3 = new halfedge ();
			he3->end = v1;
			halfedge* opposite3 = new halfedge ();
			opposite3->end = v3;
			opposite3->opposite = he3;
			he3->opposite = opposite3;

			nedge* temp = iedges[v3];
			nedge* tempi = new nedge ();
			tempi->start = v3;
			tempi->he = he3;
			if (temp == NULL) {
				iedges[v3] = tempi;
			} else {
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
			temp = iedges[v1];
			tempi = new nedge ();
			tempi->start = v1;
			tempi->he = opposite3;
			if (temp == NULL) {
				iedges[v1] = tempi;
			} else {
				while (temp->next != NULL) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
		}

		he1->next = he2;
		he2->next = he3;
		he3->next = he1;

		hf->edge = he1;
		he1->face = hf;
		he2->face = hf;
		he3->face = hf;
		if (vertexs[v1].edge == NULL)
			vertexs[v1].edge = he1;
		if (vertexs[v2].edge == NULL)
			vertexs[v2].edge = he2;
		if (vertexs[v3].edge == NULL)
			vertexs[v3].edge = he3;
		hefaces[i] = hf;
	}

}
void drawModel () {//画出模型
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	for (int i = 0; i < numOfFace; i++) {
		halfedge* e = hefaces[i]->edge;
		vertex v1 = vertexs[e->end];
		vertex v2 = vertexs[e->next->end];
		vertex v3 = vertexs[e->next->next->end];
		glBegin (GL_POLYGON);
		glNormal3f (normalvectors[i].x, normalvectors[i].y, normalvectors[i].z);
		glVertex3f (v1.x, v1.y, v1.z);
		glVertex3f (v2.x, v2.y, v2.z);
		glVertex3f (v3.x, v3.y, v3.z);
		glEnd ();
	}
	glutSwapBuffers ();
	glFlush ();
}
void insertIedge (nedge** iedges, nedge* ie) {
	nedge* it = iedges[ie->start];
	if (it == NULL) {
		iedges[ie->start] = ie;
	} else {
		while (it->next != NULL) {
			it = it->next;
		}
		it->next = ie;
	}
}
void computeNormalVec () {
	normalvectors = new normalVec[numOfFace];
	for (int i = 0; i < numOfFace; i++) {
		halfedge* hef = hefaces[i]->edge;
		int v0 = hef->next->next->end;
		int v1 = hef->end;
		int v2 = hef->next->end;
		float p1[] = { vertexs[v1].x - vertexs[v0].x, vertexs[v1].y - vertexs[v0].y,vertexs[v1].z - vertexs[v0].z };
		float p2[] = { vertexs[v2].x - vertexs[v1].x,vertexs[v2].y - vertexs[v1].y,vertexs[v2].z - vertexs[v1].z };
		float nx = p1[1] * p2[2] - p1[2] * p2[1];
		float ny = p1[2] * p2[0] - p1[0] * p2[2];
		float nz = p1[0] * p2[1] - p1[1] * p2[0];
		float len = sqrt (pow (nx, 2) + pow (ny, 2) + pow (nz, 2));
		nx = nx / len;
		ny = ny / len;
		nz = nz / len;
		normalvectors[i] = normalVec (nx, ny, nz);
	}
}

void loopDivision () {//loop细分算法
	vertex* newvertexs = new vertex[numOfVertex + 1.5 * numOfFace];	//新添加顶点数组
	he_face** nfaces = new he_face * [4 * numOfFace];	//新添加面数组
	nedge** niedges = new nedge * [numOfVertex + 1.5 * numOfFace];	//新nedge数组
	int numoff = 0;	//新添加面数
	int numofv = 0;//新添加点数
	for (int i = 0; i < numOfVertex + 1.5 * numOfFace; i++) {
		niedges[i] = NULL;
	}

	//更新原来顶点的坐标
	for (int i = 0; i < numOfVertex; i++) {
		int n = 0;//n代表
		halfedge* hedge = vertexs[i].edge;//对于每个边
		do {
			n++;//统计该点有多少邻接点
			hedge = hedge->opposite->next;
		} while (hedge != vertexs[i].edge);


		float beta = (5.0 / 8 - pow ((3.0 / 8 + cos (2 * PI / n) / 4), 2)) / n;//计算beta

		float sumpx = 0;
		float sumpy = 0;
		float sumpz = 0;
		do {
			sumpx += vertexs[hedge->end].x;
			sumpy += vertexs[hedge->end].y;
			sumpz += vertexs[hedge->end].z;
			hedge = hedge->opposite->next;
		} while (hedge != vertexs[i].edge);

		//计算并赋值新顶点
		newvertexs[i] = vertex (
			(1 - n * beta) * vertexs[i].x + beta * sumpx,
			(1 - n * beta) * vertexs[i].y + beta * sumpy,
			(1 - n * beta) * vertexs[i].z + beta * sumpz,
			vertexs[i].edge
		);
		numofv++;
	}


	//对每个面设定新添加顶点的坐标
	for (int i = 0; i < numOfFace; i++) {
		halfedge* fedge = hefaces[i]->edge;
		do {//邻接点
			int v0 = fedge->next->next->end;
			int v1 = fedge->end;
			int v2 = fedge->opposite->next->end;
			int v3 = fedge->next->end;
			nedge* it = iedges[v0];
			int middle = -1;
			middle = getMiddle (v0, v1, iedges);
			if (middle == -1) {
				vertex newv = vertex ();
				newv.x = 3 * (vertexs[v0].x + vertexs[v1].x) / 8 + (vertexs[v2].x + vertexs[v3].x) / 8;
				newv.y = 3 * (vertexs[v0].y + vertexs[v1].y) / 8 + (vertexs[v2].y + vertexs[v3].y) / 8;
				newv.z = 3 * (vertexs[v0].z + vertexs[v1].z) / 8 + (vertexs[v2].z + vertexs[v3].z) / 8;
				newvertexs[numofv] = newv;

				nedge* itemp = iedges[v0];
				while (itemp != NULL) {
					if (itemp->he == fedge) {
						itemp->middle = numofv;
						break;
					}
					itemp = itemp->next;
				}
				itemp = iedges[v1];
				while (itemp != NULL) {
					if (itemp->he == fedge->opposite) {
						itemp->middle = numofv;
						break;
					}
					itemp = itemp->next;
				}
				middle = numofv;
				numofv++;
			}
			//设定新半边
			halfedge* newhe1 = new halfedge ();
			halfedge* newhe2 = new halfedge ();
			newhe1->end = getMiddle (v0, v1, iedges);
			nedge* newie1 = new nedge ();
			newie1->start = v0;
			newie1->he = newhe1;
			insertIedge (niedges, newie1);
			halfedge* op1 = getHalfEdge (middle, v0, niedges);
			newhe1->opposite = op1;
			if (op1 != NULL) {
				op1->opposite = newhe1;
			}
			newhe2->end = v1;
			nedge* newie2 = new nedge ();
			newie2->start = middle;
			newie2->he = newhe2;
			insertIedge (niedges, newie2);
			halfedge* op2 = getHalfEdge (v1, middle, niedges);
			newhe2->opposite = op2;
			if (op2 != NULL) {
				op2->opposite = newhe2;
			}
			newvertexs[v0].edge = newhe1;
			newvertexs[middle].edge = newhe2;
			fedge = fedge->next;
		} while (fedge != hefaces[i]->edge);

		//分割面
		do {
			int v0 = fedge->next->next->end;
			int v1 = fedge->end;
			int v3 = fedge->next->end;
			int middle1 = getMiddle (v0, v1, iedges);
			int middle2 = getMiddle (v3, v0, iedges);

			halfedge* middlehe = new halfedge ();
			middlehe->end = middle2;
			halfedge* fhe1 = getHalfEdge (v0, middle1, niedges);
			fhe1->next = middlehe;
			halfedge* fhe2 = getHalfEdge (middle2, v0, niedges);
			middlehe->next = fhe2;
			fhe2->next = fhe1;
			he_face* nface = new he_face ();
			nface->edge = fhe1;
			nfaces[numoff] = nface;
			numoff++;
			fhe1->face = nface;
			fhe2->face = nface;
			middlehe->face = nface;

			nedge* middleiedge = new nedge ();
			middleiedge->start = middle1;
			middleiedge->he = middlehe;
			insertIedge (niedges, middleiedge);
			fedge = fedge->next;
		} while (fedge != hefaces[i]->edge);

		//添加中间的面
		int middle1 = -1;
		int middle2 = -1;
		int middle3 = -1;
		middle1 = getMiddle (fedge->next->next->end, fedge->end, iedges);
		middle2 = getMiddle (fedge->end, fedge->next->end, iedges);
		middle3 = getMiddle (fedge->next->end, fedge->next->next->end, iedges);

		halfedge* middlefhe1 = new halfedge ();
		halfedge* middlefhe2 = new halfedge ();
		halfedge* middlefhe3 = new halfedge ();
		middlefhe1->end = middle1;
		middlefhe2->end = middle2;
		middlefhe3->end = middle3;
		middlefhe1->next = middlefhe2;
		middlefhe2->next = middlefhe3;
		middlefhe3->next = middlefhe1;
		halfedge* middleo1 = getHalfEdge (middle1, middle3, niedges);
		middleo1->opposite = middlefhe1;
		middlefhe1->opposite = middleo1;
		halfedge* middleo2 = getHalfEdge (middle2, middle1, niedges);
		middleo2->opposite = middlefhe2;
		middlefhe2->opposite = middleo2;
		halfedge* middleo3 = getHalfEdge (middle3, middle2, niedges);
		middleo3->opposite = middlefhe3;
		middlefhe3->opposite = middleo3;
		he_face* middlehf = new he_face ();
		middlehf->edge = middlefhe1;
		middlefhe1->face = middlehf;
		middlefhe2->face = middlehf;
		middlefhe3->face = middlehf;
		nfaces[numoff] = middlehf;
		numoff++;
		nedge* mie1 = new nedge ();
		mie1->start = middle3;
		mie1->he = middlefhe1;
		insertIedge (niedges, mie1);
		nedge* mie2 = new nedge ();
		mie2->start = middle1;
		mie2->he = middlefhe2;
		insertIedge (niedges, mie2);
		nedge* mie3 = new nedge ();
		mie3->start = middle2;
		mie3->he = middlefhe3;
		insertIedge (niedges, mie3);
	}

	//释放原来的老点、老边、老面
	free (vertexs);
	vertexs = newvertexs;
	numOfVertex = numofv;
	free (hefaces);
	hefaces = nfaces;

	numOfFace = numoff;
	free (iedges);
	iedges = niedges;

	computeNormalVec();
	drawModel ();
}

void setLightRes () {
	glEnable (GL_LIGHTING); //启用光源

	GLfloat light0Position[] = { 0, 1, 0, 0 };
	GLfloat env_ambient[] = { 0.5,0.5,0.5 };//指定环境光的RGBA强度,light0可以设置为环境光
	GLfloat diffuseLight0[] = { 1,1,1 };
	GLfloat specularlight0[] = { 1,1,1 };
	glLightfv (GL_LIGHT0, GL_POSITION, light0Position);
	glLightfv (GL_LIGHT0, GL_AMBIENT, env_ambient);//设置环境光成分
	glLightfv (GL_LIGHT0, GL_DIFFUSE, diffuseLight0);//设置漫射光成分
	glLightfv (GL_LIGHT0, GL_SPECULAR, specularlight0);//设置镜面光成分

	GLfloat light1Position[] = { -1, 0, 1, 1 };
	GLfloat light1Direction[] = { 1, 0, -1, 1 };
	GLfloat diffuseLight1[] = { 1,0,0 };
	GLfloat specularlight1[] = { 1,0,0 };
	glLightfv (GL_LIGHT1, GL_POSITION, light1Position);
	glLightfv (GL_LIGHT1, GL_SPOT_DIRECTION, light1Direction);
	glLightf (GL_LIGHT1, GL_SPOT_CUTOFF, 45.0);//设置光锥夹角
	glLightfv (GL_LIGHT1, GL_DIFFUSE, diffuseLight1);//设置漫射光成分
	glLightfv (GL_LIGHT1, GL_SPECULAR, specularlight1);//设置镜面光成分

	GLfloat light2Position[] = { 1, 0, 1, 1 };
	GLfloat light2Direction[] = { -1, 0, -1, 1 };
	GLfloat diffuseLight2[] = { 0,0,1 };
	GLfloat specularlight2[] = { 0,0,1 };
	glLightfv (GL_LIGHT2, GL_POSITION, light2Position);
	glLightfv (GL_LIGHT2, GL_SPOT_DIRECTION, light2Direction);
	glLightf (GL_LIGHT2, GL_SPOT_CUTOFF, 45.0);//设置光锥夹角
	glLightfv (GL_LIGHT2, GL_DIFFUSE, diffuseLight2);//设置漫射光成分
	glLightfv (GL_LIGHT2, GL_SPECULAR, specularlight2);//设置镜面光成分

	GLfloat light3Position[] = { 0, 0, 1, 1 };
	GLfloat light3Direction[] = { 0, 0, -1, 1 };
	GLfloat diffuseLight3[] = { 1,1,0 };
	GLfloat specularlight3[] = { 1,1,0 };
	glLightfv (GL_LIGHT3, GL_POSITION, light3Position);
	glLightfv (GL_LIGHT3, GL_SPOT_DIRECTION, light3Direction);
	glLightf (GL_LIGHT3, GL_SPOT_CUTOFF, 45.0);//设置光锥夹角
	glLightfv (GL_LIGHT3, GL_DIFFUSE, diffuseLight3);//设置漫射光成分
	glLightfv (GL_LIGHT3, GL_SPECULAR, specularlight3);//设置镜面光成分
}

void centralModel () {
	float centerx = 0;
	float centery = 0;
	float centerz = 0;
	for (int i = 0; i < numOfVertex; i++) {
		centerx += vertexs[i].x;
		centery += vertexs[i].y;
		centerz += vertexs[i].z;
	}
	centerx /= numOfVertex;
	centery /= numOfVertex;
	centerz /= numOfVertex;
	for (int i = 0; i < numOfVertex; i++) {
		vertexs[i].x -= centerx;
		vertexs[i].y -= centery;
		vertexs[i].z -= centerz;
		if (vertexs[i].x > maxx)
			maxx = vertexs[i].x;
		if (vertexs[i].y > maxy)
			maxy = vertexs[i].y;
		if (vertexs[i].z > maxz)
			maxz = vertexs[i].z;
		if (vertexs[i].x < minx)
			minx = vertexs[i].x;
		if (vertexs[i].y < miny)
			miny = vertexs[i].y;
		if (vertexs[i].z < minz)
			minz = vertexs[i].z;
	}
	float max;
	if (maxx > maxy&& maxx > maxz) {
		max = maxx;
	} else if (maxx > maxx&& maxy > maxz) {
		max = maxy;
	} else {
		max = maxz;
	}
	ShootPosition[2] = maxz;
	radius = max;
	ShootDirect[1] = 1;
}
void rotate (float t1, float t2) {
	float x = radius * cos (t2) * sin (t1);
	float y = radius * sin (t2);
	float z = radius * cos (t2) * cos (t1);
	float x2 = 1 * sin (t2) * -1 * sin (t1);
	float y2 = 1 * cos (t2);
	float z2 = 1 * sin (t2) * -1 * cos (t1);
	ShootDirect[0] = x2;
	ShootDirect[1] = y2;
	ShootDirect[2] = z2;
	ShootPosition[0] = x;
	ShootPosition[1] = y;
	ShootPosition[2] = z;
	gluLookAt (ShootPosition[0], ShootPosition[1], ShootPosition[2], 0.0, 0.0, 0.0, ShootDirect[0], ShootDirect[1], ShootDirect[2]);
	glutPostRedisplay ();
}
void onMouseClick (int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			px = x;
			py = y;
		}
	}

	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			if (displaystate == 1)
				glPolygonMode (GL_FRONT, GL_LINE);
			else if (displaystate == 0) {
				glPolygonMode (GL_FRONT, GL_POINT);
			} else {
				glPolygonMode (GL_FRONT, GL_FILL);
			}
			drawModel ();

			displaystate = (displaystate + 1) % 3;
		}
	}
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		
	}
	if (button == GLUT_WHEEL_DOWN) {
		scale -= 0.5;
	}
	if (button == GLUT_WHEEL_UP) {
		scale += 0.5;
	}

	glutPostRedisplay ();

}
void onMouseMove (int x, int y) {
	float dx = 20.0f / WIDTH;
	float dy = 20.0f / HEIGHT;

	if (x - px > 2) {
		theta1 -= 0.1;
		px = x;
		rotate (theta1, theta2);
	} else if (x - px < -2) {
		theta1 += 0.1;
		px = x;
		rotate (theta1, theta2);
	}

	if (y - py > 5) {
		theta2 += 0.1;
		py = y;
		rotate (theta1, theta2);
	}

	else if (y - py < -5) {
		theta2 -= 0.1;
		py = y;
		rotate (theta1, theta2);
	}

}
void onKeyboardPress (unsigned char key, int mouseX, int mouseY) {
	switch (key) {
	case '\r':
	{
		loopDivision ();

		break;
	}
	}
	glutPostRedisplay ();
}

void myDisplay () {
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glScalef (scale, scale, scale);
	setLightRes ();
	gluLookAt (ShootPosition[0], ShootPosition[1], ShootPosition[2], 0.0, 0.0, 0.0, ShootDirect[0], ShootDirect[1], ShootDirect[2]);
	glPushMatrix ();
	drawModel ();
	glPopMatrix ();
}

void reshape (int width, int height) {
	glViewport (0, 0, (GLsizei)width, (GLsizei)height);
	//设置投影参数

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	//正交投影
	float scale = 0.5;
	if (width <= height)
		glOrtho (-1.5 * scale, 1.5 * scale, -1.5 * scale * (GLfloat)height / (GLfloat)width, 1.5 * scale * (GLfloat)height / (GLfloat)width, -10.0 * scale, 10.0 * scale);
	else
		glOrtho (-1.5 * scale * (GLfloat)width / (GLfloat)height, 1.5 * scale * (GLfloat)width / (GLfloat)height, -1.5 * scale, 1.5 * scale, -10.0 * scale, 10.0 * scale);
	//设置模型参数--几何体参数
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();


}
void dtest () {
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	glBegin (GL_POLYGON);
	glVertex2f (0.0f, 0.0f);
	glVertex2f (0.5f, 0.5f);
	glVertex2f (0.2f, 0.75f);
	//glNormal3f(normalvectors[i].x, normalvectors[i].y, normalvectors[i].z);
	glEnd ();
	glutSwapBuffers ();
	glFlush ();
}
void visitAll (he_face* s) {
	queue<he_face*> q = queue<he_face*> ();
	q.push (s);
	while (!q.empty ()) {
		he_face* tempf = q.front ();
		q.pop ();
		tempf->visit = true;
		halfedge* temp = tempf->edge;
		do {
			temp->visit = true;
			vertexs[temp->end].visit = true;
			temp = temp->next;
		} while (temp != tempf->edge);
		temp = tempf->edge;
		do {
			if (temp->opposite->face != NULL && !temp->opposite->face->visit) {
				q.push (temp->opposite->face);
			}
			temp = temp->next;
		} while (temp != tempf->edge);

	}
}

int main (int argc, char* argv[]) {

	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition (100, 100);
	glutInitWindowSize (WIDTH, HEIGHT);
	glutCreateWindow ("网格细分");
	glEnable (GL_CULL_FACE);
	glEnable (GL_DEPTH_TEST);
	glShadeModel (GL_SMOOTH);
	readFile ();
	iedges = new nedge * [numOfVertex];
	initHalfedge ();
	cout << "加载完成" << endl;
	computeNormalVec ();
	centralModel ();
	glutKeyboardFunc (&onKeyboardPress);
	glutMotionFunc (&onMouseMove);
	glutMouseFunc (&onMouseClick);
	glEnable (GL_LIGHT0);
	glEnable (GL_NORMALIZE);
	glutDisplayFunc (&myDisplay);
	glutReshapeFunc (&reshape);
	glutSwapBuffers ();
	glutMainLoop ();
	return 0;
}