#pragma comment(lib, "./glut32.lib")

#include<iostream>
#include<fstream>
#include "glut.h"
#include<GL/GLU.h>
#include <math.h>
#include<string>
#include<queue>
using namespace std;

//用于点线面的个数
int numOfVertex = 0;
int numOfFace = 0;
int numOfLine = 0;

//定义窗口大小
int WIDTH = 600;
int HEIGHT = 600;
float maxx = -1;
float maxy = -1;
float maxz = -1;
float minx = 1;
float miny = 1;
float minz = 1;

//定义摄像机位置和方向
GLfloat ShootPosition[] = { 0,0,0 };
GLfloat ShootDirect[] = { 0,0,0 };

//与实现旋转角度大小相关的参数，只需要两个就可以完成
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


//定义点、面、边等结构
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

//定义半边结构
struct halfedge {//半边结构
	halfedge* next;//下一条半边
	halfedge* opposite;//与其相反的半边
	int end;//所指的点
	bool visit;//是否被访问过
	he_face* face;//所属的面
	//定义构造函数并初始化
	halfedge () {
		next = nullptr;
		opposite = nullptr;
		end = -1;
		face = nullptr;
		visit = false;
	}
};

//定义半边结构中的点结构
struct vertex {//顶点
	//点坐标
	float x;
	float y;
	float z;

	//指向该点的半边
	halfedge* edge;

	//是否被访问过
	bool visit;

	//空构造函数，初始化
	vertex () {
		visit = false;
	}

	//构造函数
	vertex (float a, float b, float c) {
		x = a;
		y = b;
		z = c;
		edge = nullptr;
		visit = false;
	}
};

//定义正常的点的结构或者成为向量
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

//定义半边中面的结构
struct he_face {//半边面
	//包含的半边
	halfedge* edge;
	//是否被访问过
	bool visit;
	he_face () {
		edge = NULL;
		visit = false;
	}
};

//定义正常的面的结构
struct face {//面
	//构成该面的点的数量
	int numofv;
	//构成该面的点
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
		he = nullptr;
		next = nullptr;
		middle = -1;
	}
};

//定义自己的边结构
struct iedge {
	//起点
	int start;
	//在loop细分中的中间点
	int middle;
	//所含半边（两条）
	halfedge* he;
	iedge* next;
	iedge () {
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
	string filename = "block.off";
	//cin >> filename;
	infile.open ("off/" + filename);

	//读取“off”字符
	infile >> data;

	//读取点、面、边的数量
	infile >> numOfVertex;
	infile >> numOfFace;
	infile >> numOfLine;
	vertexs = new vertex[numOfVertex];
	faces = new face[numOfFace];
	int vnum = 0;
	int fnum = 0;

	//构建“点”集
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

	//构建“面”集
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
	while (temp != nullptr) {
		if (temp->he->end == end) {
			break;
		}
		temp = temp->next;
	}
	if (temp == nullptr) {
		return -1;
	} else {
		return temp->middle;
	}

}


//获取半边
halfedge* getHalfEdge (int start, int end, nedge** iedges) {
	nedge* temp = iedges[start];
	while (temp != nullptr) {
		if (temp->he->end == end) {
			break;
		}
		temp = temp->next;
	}
	if (temp == nullptr) {
		return nullptr;
	} else {
		return temp->he;
	}

}

//初始化半边结构中的半边，点和面
void initHalfedge () {//初始化半边结构
	hefaces = new he_face * [numOfFace];
	int numofhe = 0;
	for (int i = 0; i < numOfVertex; i++) {
		iedges[i] = nullptr;
	}
	for (int i = 0; i < numOfFace; i++) {
		int v1 = faces[i].vertexs[0];
		int v2 = faces[i].vertexs[1];
		int v3 = faces[i].vertexs[2];
		he_face* hf = new he_face ();
		halfedge* he1 = getHalfEdge (v1, v2, iedges);
		halfedge* he2 = getHalfEdge (v2, v3, iedges);
		halfedge* he3 = getHalfEdge (v3, v1, iedges);

		//这才是真正开始构造
		if (he1 == nullptr) {
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
			if (temp == nullptr) {
				iedges[v1] = tempi;
			} else {
				while (temp->next != nullptr) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
			temp = iedges[v2];
			tempi = new nedge ();
			tempi->start = v2;
			tempi->he = opposite1;
			if (temp == nullptr) {
				iedges[v2] = tempi;
			} else {
				while (temp->next != nullptr) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
		}
		if (he2 == nullptr) {
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
			if (temp == nullptr) {
				iedges[v2] = tempi;
			} else {
				while (temp->next != nullptr) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
			temp = iedges[v3];
			tempi = new nedge ();
			tempi->start = v3;
			tempi->he = opposite2;
			if (temp == nullptr) {
				iedges[v3] = tempi;
			} else {
				while (temp->next != nullptr) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
		}
		if (he3 == nullptr) {
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
			if (temp == nullptr) {
				iedges[v3] = tempi;
			} else {
				while (temp->next != nullptr) {
					temp = temp->next;
				}
				temp->next = tempi;
			}
			temp = iedges[v1];
			tempi = new nedge ();
			tempi->start = v1;
			tempi->he = opposite3;
			if (temp == nullptr) {
				iedges[v1] = tempi;
			} else {
				while (temp->next != nullptr) {
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
		if (vertexs[v1].edge == nullptr)
			vertexs[v1].edge = he1;
		if (vertexs[v2].edge == nullptr)
			vertexs[v2].edge = he2;
		if (vertexs[v3].edge == nullptr)
			vertexs[v3].edge = he3;
		hefaces[i] = hf;
	}

}

//显示3d模型
void drawModel () {//画出模型
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f (1.0, 1.0, 1.0);
	for (int i = 0; i < numOfFace; i++) {
		halfedge* e = hefaces[i]->edge;
		vertex v1 = vertexs[e->end];
		vertex v2 = vertexs[e->next->end];
		vertex v3 = vertexs[e->next->next->end];
		//绘制多边形
		glBegin (GL_POLYGON);
		//用来设置当前的法向量，这个法向量将被应用到紧接下来的glVertex*() 所定义的顶点上
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
	if (it == nullptr) {
		iedges[ie->start] = ie;
	} else {
		while (it->next != nullptr) {
			it = it->next;
		}
		it->next = ie;
	}
}

//更像是计算面的法向
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
		//细分后点、面和边的个数
	vertex* newvertexs = new vertex[numOfVertex + 1.5 * numOfFace];	//新添加顶点数组
	he_face** nfaces = new he_face * [4 * numOfFace];	//新添加面数组
	nedge** niedges = new nedge * [numOfVertex + 1.5 * numOfFace];	//新nedge数组

	int numoff = 0;	//新添加面数
	int numofv = 0;//新添加点数
	for (int i = 0; i < numOfVertex + 1.5 * numOfFace; i++) {
		niedges[i] = nullptr;
	}
	//更新原来顶点的坐标
	for (int i = 0; i < numOfVertex; i++) {
		int n = 0;
		halfedge* hedge = vertexs[i].edge;

		//统计该点（原顶点）周围有多少个点
		do {
			n++;
			hedge = hedge->opposite->next;
		} while (hedge != vertexs[i].edge);

		//计算β，即每个与之相邻的点的权重
		float beta = (5.0 / 8 - pow ((3.0 / 8 + cos (2 * PI / n) / 4), 2)) / n;
		float sumpx = 0;
		float sumpy = 0;
		float sumpz = 0;

		//计算所有与之相邻的点的坐标之和
		do {
			sumpx += vertexs[hedge->end].x;
			sumpy += vertexs[hedge->end].y;
			sumpz += vertexs[hedge->end].z;
			hedge = hedge->opposite->next;
		} while (hedge != vertexs[i].edge);

		//更新原点坐标
		float npx = (1 - n * beta) * vertexs[i].x + beta * sumpx;
		float npy = (1 - n * beta) * vertexs[i].y + beta * sumpy;
		float npz = (1 - n * beta) * vertexs[i].z + beta * sumpz;
		vertex nv = vertex ();
		nv.x = npx;
		nv.y = npy;
		nv.z = npz;
		nv.edge = vertexs[i].edge;
		newvertexs[i] = nv;
		numofv++;
	}

	//更新新加入的点的坐标
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

			//计算并添加新顶点
			if (middle == -1) {
				vertex newv = vertex ();
				newv.x = 3 * (vertexs[v0].x + vertexs[v1].x) / 8 + (vertexs[v2].x + vertexs[v3].x) / 8;
				newv.y = 3 * (vertexs[v0].y + vertexs[v1].y) / 8 + (vertexs[v2].y + vertexs[v3].y) / 8;
				newv.z = 3 * (vertexs[v0].z + vertexs[v1].z) / 8 + (vertexs[v2].z + vertexs[v3].z) / 8;
				newvertexs[numofv] = newv;

				//重构iedge的结构
				nedge* itemp = iedges[v0];
				while (itemp != nullptr) {
					if (itemp->he == fedge) {
						itemp->middle = numofv;
						break;
					}
					itemp = itemp->next;
				}
				itemp = iedges[v1];
				while (itemp != nullptr) {
					if (itemp->he == fedge->opposite) {
						itemp->middle = numofv;
						break;
					}
					itemp = itemp->next;
				}
				middle = numofv;
				numofv++;
			}

			//构建新的半边结构
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
			if (op1 != nullptr) {
				op1->opposite = newhe1;
			}
			newhe2->end = v1;
			nedge* newie2 = new nedge ();
			newie2->start = middle;
			newie2->he = newhe2;
			insertIedge (niedges, newie2);
			halfedge* op2 = getHalfEdge (v1, middle, niedges);
			newhe2->opposite = op2;
			if (op2 != nullptr) {
				op2->opposite = newhe2;
			}
			newvertexs[v0].edge = newhe1;
			newvertexs[middle].edge = newhe2;
			fedge = fedge->next;
		} while (fedge != hefaces[i]->edge);

		//构建新的面（周围的三个）
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

		//构建中间的那个三角形
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

		//构建中间三角形的iedge
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

	//更新一些参数，点、面、边的个数
	free (vertexs);
	vertexs = newvertexs;
	numOfVertex = numofv;
	free (hefaces);
	hefaces = nfaces;

	numOfFace = numoff;
	free (iedges);
	iedges = niedges;

	//重新计算法向并显示
	computeNormalVec ();
	drawModel ();
}
//设置光照
void setLightRes () {
	glEnable (GL_LIGHTING); // 启用光照
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };//光源位置
	GLfloat light_ambient[] = { 0.0, 1.0, 0.0, 1.0 };//rgb下的环境光颜色
	GLfloat light_diffuse[] = { 1.0, 1.0, 0.0, 1.0 };//漫反射光的颜色
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };//镜面光的颜色
	glLightfv (GL_LIGHT0, GL_POSITION, light_position);
	glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable (GL_LIGHT0); // 启用指定光源

	//glEnable(GL_LIGHTING);
	//GLfloat light0Position[] = { 0, 1, 0, 0 };//光源位置
	//GLfloat env_ambient[] = { 0.5,0.5,0.5 };//环境光颜色
	//GLfloat diffuseLight0[] = { 1,1,1 };//漫反射光的颜色
	//GLfloat specularlight0[] = { 1,1,1 };//镜面光的颜色
	//glLightfv(GL_LIGHT0, GL_POSITION, light0Position);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, env_ambient);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight0);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specularlight0);//设置镜面光成分
}


//重新计算中心，跟新摄像机位置和方向
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
void onMouseClick (int button, int state, int x, int y) {

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			px = x;
			py = y;
		}
	}
	glPolygonMode (GL_FRONT, GL_LINE);
	//if (button == GLUT_RIGHT_BUTTON) {
	//	if (state == GLUT_DOWN) {
	//		//if (displaystate == 1)
	//		//	glPolygonMode (GL_FRONT, GL_LINE);
	//		//else if (displaystate == 0) {
	//		//	glPolygonMode (GL_FRONT, GL_POINT);
	//		//} else {
	//		//	glPolygonMode (GL_FRONT, GL_FILL);
	//		//}
	//		//drawModel ();

	//		//displaystate = (displaystate + 1) % 3;
	//	}
	//}
	if ( button == GLUT_WHEEL_UP) {
		scale += 5;
	}
	if (button == GLUT_WHEEL_DOWN) {
		scale -= 5;
	}
	glutPostRedisplay ();

}

//旋转过程中视线摄像头位置和方向
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

//旋转
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

//定义放大、缩小和细分的键盘事件
void onKeyboardPress (unsigned char key, int mouseX, int mouseY) {
	switch (key) {
	case 'w':
	{
		scale += 0.05;
		break;
	}
	case 's':
	{
		if (scale > 0)
			scale -= 0.05;
		break;
	}
	case 'n':
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
			if (temp->opposite->face != nullptr && !temp->opposite->face->visit) {
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