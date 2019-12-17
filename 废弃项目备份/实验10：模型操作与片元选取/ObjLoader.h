#pragma once
#pragma comment(lib, "./glut32.lib")

#include <fstream>
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include "glut.h"
using namespace std;

//����λ��
//v 1.000000 -1.000000 -1.000000
struct V {
	double x;
	double y;
	double z;
};
//������������
//vt 0.748573 0.750412
struct Vt {
	double tU;
	double tV;
};
//���㷨����
//vn 0.000000 0.000000 - 1.000000
struct Vn {
	double nX;
	double nY;
	double nZ;
};
//��
//f 5/1/1 1/2/1 4/3/1
//����λ�� / �������� / ��������������������������ǰ����v, vt, vn��������� ע������Obj�������Ǵ�1��ʼ�ģ�������0
struct F {
	vector<int> v_index;
	vector<int> vt_index;
	vector<int> vn_index;
};

//���򶥵�
struct N {
	int vi;//��������
	set<int> v;//��������򶥵���������set��ֹ�ظ�
};

//���ڴ���ģ��
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
			cout << "�޷���obj�ļ�" << endl;
		}

		//���δ�����ļ��Ľ�β
		while (!f.eof ()) {
			getline (f, line);//�õ�obj�ļ���һ�У���Ϊһ���ַ���
			//������ʱ��ÿ�н����и�֮����ַ�������
			vector<string> temp;
			//β���ո�
			string blank = " ";
			//ÿ��char�ַ����кϲ��Ľ��
			string ans = "";

			//��ÿ��β��װ��һ�����գ���������и�
			line = line.append (blank);
			//��ÿһ�н��д����Կո��з�
			for (int i = 0; i < line.length (); i++) {
				char c = line[i];
				//�����Ϊ�գ��ͽ�char�ַ����кϲ�
				if (c != ' ') {
					ans += c;
				}
				//���Ϊ�գ���֮ǰ�ϲ��Ľ���ŵ�����֮��
				else {
					temp.push_back (ans);
					ans = "";
				}
			}


			//����Ƕ���Ļ�
			if (temp[0] == "v" && temp.size () == 4) {
				V v;
				v.x = atof (temp[1].c_str ());
				v.y = atof (temp[2].c_str ());
				v.z = atof (temp[3].c_str ());
				model.v.push_back (v);
			}
			//���������Ļ�
			else if (temp[0] == "vt" && temp.size () == 3) {
				Vt vt;
				vt.tU = atof (temp[1].c_str ());
				vt.tV = atof (temp[2].c_str ());
				model.vt.push_back (vt);
			}
			//����Ƿ������Ļ�
			else if (temp[0] == "vn" && temp.size () == 4) {
				Vn vn;
				vn.nX = atof (temp[1].c_str ());
				vn.nY = atof (temp[2].c_str ());
				vn.nZ = atof (temp[3].c_str ());
				model.vn.push_back (vn);
			}
			/*
			�������Ļ�������������������
			f�ĸ�ʽΪ
			1. ����λ�� / �������� / ������ *3��
			2. ����λ�� / �������� / ������ *4��
			3. ����λ�� / �������� *3��
			4. ����λ�� / �������� *4��
			*/
			else if (temp[0] == "f") {
				F f;
				for (int i = 1; i < temp.size (); i++) {
					string s = temp[i];
					s = s.append ("/");
					string ans = "";
					//������/��
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
						//��Ϊ����������obj�ļ����Ǵ�1��ʼ�ģ������Ǵ�ŵĶ���vector�Ǵ�0��ʼ�ģ����Ҫ��1
						int vIndex = atof (index[0].c_str ()) - 1;
						f.v_index.push_back (vIndex);
						int vtIndex = atof (index[1].c_str ()) - 1;
						f.vt_index.push_back (vtIndex);
					} else if (index.size () == 3) {
						//cout << index[1] << endl;
						//��Ϊ����������obj�ļ����Ǵ�1��ʼ�ģ������Ǵ�ŵĶ���vector�Ǵ�0��ʼ�ģ����Ҫ��1
						int vIndex = atof (index[0].c_str ()) - 1;
						f.v_index.push_back (vIndex);
						int vtIndex = atof (index[1].c_str ()) - 1;
						f.vt_index.push_back (vtIndex);
						int vnIndex = atof (index[2].c_str ()) - 1;
						f.vn_index.push_back (vnIndex);
					} else {
						cout << "��obj�ļ���f�ĸ�ʽ����" << endl;
					}

				}
				model.f.push_back (f);
			}
		}
		f.close ();

		//�õ�model����ά����ģ��,�󶥵��Լ�������Ч�ʵ�
		for (int i = 0; i < model.v.size (); i++) {
			//�ӵ�һ�����㿪ʼ����
			N n;
			n.vi = i;
			int f_size = model.f.size ();
			//�����е��浱�б��������򶥵㣬�������������򶥵�
			int count = 0;//��ֹѭ������
			for (int j = 0; j < f_size; j++) {
				if (count <= 6) {
					int v_index_size = model.f[j].v_index.size ();
					for (int k = 0; k < v_index_size; k++) {
						//��������к��иö��㣬��Ѹ����е����ඥ�㱣��
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
		cout << "��ȡ�ļ����" << endl;
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
			if (mode == GL_SELECT)glLoadName (i);//������1ѹ���ջ
			glColor3f (1, 1, 1);
			if (picked == i)
				glColor3f (1, 0, 0);


			glBegin (GL_POLYGON);//��ʼ����

			int n = model.f[i].v_index.size ();
			//cout << "size " << n << endl;
			//��ȡҪ������
			F f = model.f[i];
			//û������
			for (int j = 0; j < n; j++) {
				//��ȡ�����ϵĶ���
				//cout << "v_index" << f.v_index[j] << endl;
				V v = model.v[f.v_index[j]];
				//��ȡ�����ϵĶ���ķ�����
				Vn vn = model.vn[f.vn_index[j]];
				//���Ʒ�����
				glNormal3d (vn.nX, vn.nY, vn.nZ);
				//glNormal3d (0, 0, 0);
				//���ƶ���
				glVertex3d (v.x, v.y, v.z);
			}
			glEnd ();
		}

		glPopMatrix ();
	}

};
