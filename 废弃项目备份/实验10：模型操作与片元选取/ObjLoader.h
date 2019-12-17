#pragma once
#pragma comment(lib, "./glut32.lib")

#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "glut.h"
using namespace std;

//顶点位置
//v 1.000000 -1.000000 -1.000000
struct V {
	double x;
	double y;
	double z;
};
//顶点纹理坐标
//vt 0.748573 0.750412
struct Vt {
	double tU;
	double tV;
};
//顶点法向量
//vn 0.000000 0.000000 - 1.000000
struct Vn {
	double nX;
	double nY;
	double nZ;
};
//面
//f 5/1/1 1/2/1 4/3/1
//顶点位置 / 纹理坐标 / 法向量的索引，这里索引的是前面用v, vt, vn定义的数据 注意这里Obj的索引是从1开始的，而不是0
struct F {
	vector<int> v_index;
	vector<int> vt_index;
	vector<int> vn_index;
};

//邻域顶点
struct N {
	int vi;//顶点索引
	set<int> v;//顶点的邻域顶点索引，用set防止重复
};

//用于储存模型
class Model {
public:
	vector<V> v;
	vector<Vt> vt;
	vector<Vn> vn;
	vector<F> f;
	vector<N> n;
	void clear () {
		v.clear ();
		vt.clear ();
		vn.clear ();
		f.clear ();
		n.clear ();
		cout << "model cleared!\n";
	}
};

class ObjLoader {
public:
	ObjLoader (){}
	~ObjLoader (){}

	Model model;

	void ReadModel (string filename) {
		model.clear ();

		picked = -1;
		scale_x = 1;
		scale_y = 1;
		scale_z = 1;
		rotate_x = 0;
		rotate_y = 0;
		rotate_z = 0;
		trans_x = 0;
		trans_y = 0;
		trans_z = 0;

		string line;
		fstream f;
		f.open (filename, ios::in);

		if (!f.is_open ()) {
			cout << "无法打开obj文件" << endl;
		}

		//如果未到达文件的结尾
		while (!f.eof ()) {
			getline (f, line);//拿到obj文件中一行，作为一个字符串
			//用于临时存每行进行切割之后的字符的数组
			vector<string> temp;
			//尾部空格
			string blank = " ";
			//每个char字符进行合并的结果
			string ans = "";

			//给每行尾部装上一个隔空，方便进行切割
			line = line.append (blank);
			//对每一行进行处理，以空格切分
			for (int i = 0; i < line.length (); i++) {
				char c = line[i];
				//如果不为空，就将char字符进行合并
				if (c != ' ') {
					ans += c;
				}
				//如果为空，把之前合并的结果放到向量之中
				else {
					temp.push_back (ans);
					ans = "";
				}
			}


			//如果是顶点的话
			if (temp[0] == "v" && temp.size () == 4) {
				V v;
				v.x = atof (temp[1].c_str ());
				v.y = atof (temp[2].c_str ());
				v.z = atof (temp[3].c_str ());
				model.v.push_back (v);
			}
			//如果是纹理的话
			else if (temp[0] == "vt" && temp.size () == 3) {
				Vt vt;
				vt.tU = atof (temp[1].c_str ());
				vt.tV = atof (temp[2].c_str ());
				model.vt.push_back (vt);
			}
			//如果是法向量的话
			else if (temp[0] == "vn" && temp.size () == 4) {
				Vn vn;
				vn.nX = atof (temp[1].c_str ());
				vn.nY = atof (temp[2].c_str ());
				vn.nZ = atof (temp[3].c_str ());
				model.vn.push_back (vn);
			}
			/*
			如果是面的话，存放三个顶点的索引
			f的格式为
			1. 顶点位置 / 纹理坐标 / 法向量 *3组
			2. 顶点位置 / 纹理坐标 / 法向量 *4组
			3. 顶点位置 / 纹理坐标 *3组
			4. 顶点位置 / 纹理坐标 *4组
			*/
			else if (temp[0] == "f") {
				F f;
				for (int i = 1; i < temp.size (); i++) {
					string s = temp[i];
					s = s.append ("/");
					string ans = "";
					//跳过‘/’
					vector<string> index;
					for (int j = 0; j < s.length (); j++) {
						char ch = s[j];
						if (ch != '/') {
							ans += ch;
						} else {
							index.push_back (ans);
							ans = "";
						}
					}
					//cout << index.size() << endl;
					if (index.size () == 2) {
						//cout << index[1] << endl;
						//因为顶点索引在obj文件中是从1开始的，而我们存放的顶点vector是从0开始的，因此要减1
						int vIndex = atof (index[0].c_str ()) - 1;
						f.v_index.push_back (vIndex);
						int vtIndex = atof (index[1].c_str ()) - 1;
						f.vt_index.push_back (vtIndex);
					} else if (index.size () == 3) {
						//cout << index[1] << endl;
						//因为顶点索引在obj文件中是从1开始的，而我们存放的顶点vector是从0开始的，因此要减1
						int vIndex = atof (index[0].c_str ()) - 1;
						f.v_index.push_back (vIndex);
						int vtIndex = atof (index[1].c_str ()) - 1;
						f.vt_index.push_back (vtIndex);
						int vnIndex = atof (index[2].c_str ()) - 1;
						f.vn_index.push_back (vnIndex);
					} else {
						cout << "该obj文件中f的格式错误！" << endl;
					}

				}
				model.f.push_back (f);
			}
		}
		f.close ();

		//得到model的三维网格模型,求顶点以及其邻域，效率低
		for (int i = 0; i < model.v.size (); i++) {
			//从第一个顶点开始遍历
			N n;
			n.vi = i;
			int f_size = model.f.size ();
			//在所有的面当中遍历其邻域顶点，估计有六个邻域顶点
			int count = 0;//防止循环过多
			for (int j = 0; j < f_size; j++) {
				if (count <= 6) {
					int v_index_size = model.f[j].v_index.size ();
					for (int k = 0; k < v_index_size; k++) {
						//如果该面中含有该顶点，则把该面中的其余顶点保存
						if (model.f[j].v_index[k] == i) {
							for (int l = 0; l < v_index_size; l++) {
								if (l != k) {
									n.v.insert (model.f[j].v_index[l]);
									//n.vi_v.insert(v);
									count++;
								}
							}
							break;
						}
					}
				}
			}
			model.n.push_back (n);
		}
		cout << "读取文件完成" << endl;
		cout << model.v.size() << " " << model.vt.size() << " " << model.vn.size() << endl;
	}

	float rotate_x;
	float rotate_y;
	float rotate_z;

	float scale_x;
	float scale_y;
	float scale_z;

	float trans_x;
	float trans_y;
	float trans_z;

	int picked;
	void setScale (float scale_xi, float  scale_yi, float  scale_zi) {
		scale_x = scale_xi;
		scale_y = scale_yi;
		scale_z = scale_zi;
	}	void scaleCal (float scale_xi, float  scale_yi, float  scale_zi) {
		scale_x += scale_xi;
		scale_y += scale_yi;
		scale_z += scale_zi;
	}
	void setRotate (float rotate_xi, float  rotate_yi, float  rotate_zi) {
		rotate_x = rotate_xi;
		rotate_y = rotate_yi;
		rotate_z = rotate_zi;
	}
	void rotateCal (float rotate_xi, float  rotate_yi, float  rotate_zi) {
		rotate_x += rotate_xi;
		rotate_y += rotate_yi;
		rotate_z += rotate_zi;
	}
	void setTrans(float trans_xi, float  trans_yi, float  trans_zi) {
		trans_x = trans_xi;
		trans_y = trans_yi;
		trans_z = trans_zi;
	}
	
	void DrawModel (GLenum mode, bool* ok) {

		//glPushMatrix ();
		//glMatrixMode (GL_MODELVIEW);


		//glScalef (scale_x, scale_y, scale_z);
		//glTranslatef (trans_x, trans_y, trans_z);
		//glRotatef (rotate_x, 1, 0, 0);
		//glRotatef (rotate_y, 0, 1, 0);
		//glRotatef (rotate_z, 0, 0, 1);


		//glLineWidth (4);
		for (int i = 0; i < model.f.size (); i++) {
			if (mode == GL_SELECT)glLoadName (i);//将名字1压入堆栈
			glColor3f (1, 1, 1);
			if (picked == i)
				glColor3f (1, 0, 0);


			glBegin (GL_POLYGON);//开始绘制

			int n = model.f[i].v_index.size ();
			//cout << "size " << n << endl;
			//获取要画的面
			F f = model.f[i];
			//没有纹理
			for (int j = 0; j < n; j++) {
				//获取该面上的顶点
				//cout << "v_index" << f.v_index[j] << endl;
				V v = model.v[f.v_index[j]];
				//获取该面上的顶点的法向量
				Vn vn = model.vn[f.vn_index[j]];
				//绘制法向量
				glNormal3d (vn.nX, vn.nY, vn.nZ);
				//glNormal3d (0, 0, 0);
				//绘制顶点
				glVertex3d (v.x, v.y, v.z);
			}
			glEnd ();
		}

		glPopMatrix ();
	}

};
