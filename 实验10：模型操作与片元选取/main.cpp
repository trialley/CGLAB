#pragma warning(disable:4996)
#pragma comment(lib, "./glad.lib")
#pragma comment(lib, "./glfw3.lib")
//#pragma comment(lib, "./igl.lib")
//#pragma comment(lib, "./igl_opengl_glfw_imgui.lib")
//#pragma comment(lib, "./igl_opengl_glfw.lib")
//#pragma comment(lib, "./imgui.lib")
//#pragma comment(lib, "./igl_opengl.lib")
#include<glad/glad.h>
#include <igl/readOFF.h>
#include <igl/unproject_onto_mesh.h>
#include <igl/opengl/glfw/Viewer.h>
#include <iostream>
#include"getFilePathName.h"
int main (int argc, char* argv[]) {
	// Mesh with per-face color
	Eigen::MatrixXd V;//�������Ϣ
	Eigen::MatrixXi F;//��������Ϣ
	igl::readOFF ("../off/cube.off", V, F);// Load a mesh in OFF format

	Eigen::MatrixXd C = Eigen::MatrixXd::Constant (F.rows (), 3, 1);//���汻ѡ�еĵ� Eigen��ѧ��
	
	igl::opengl::glfw::Viewer viewer;
	int pfid = 0;
	viewer.callback_mouse_down =[&V, &F, &C,&pfid](igl::opengl::glfw::Viewer& viewer, int button, int)->bool {
		if (button == (int)igl::opengl::glfw::Viewer::MouseButton::Middle) {
			std::cout << "��ѡ������Ҫ�򿪵�offģ�ͣ�";
			std::string temp = getFilePathName ();
			std::cout << temp<<"\n";

			igl::readOFF (temp, V, F);// Load a mesh in OFF format
			C = Eigen::MatrixXd::Constant (F.rows (), 3, 1);//���汻ѡ�еĵ� Eigen��ѧ��
			
			viewer.data ().clear();
			viewer.data ().set_mesh (V, F);
			viewer.data ().set_colors (C);
			return true;
		} else if (button == (int)igl::opengl::glfw::Viewer::MouseButton::Left) {
			int fid;//��ѡ�е�ƬԪ��id
			Eigen::Vector3f bc;

			/*ȷ�����λ�ã���Ҫע��win��y�����������������෴��*/
			double x = viewer.current_mouse_x;
			double y = viewer.core ().viewport (3) - viewer.current_mouse_y;

			if (igl::unproject_onto_mesh (//������������Ƿ�ѡ��ƬԪ����ͼ��ģ�ͺ�ͶӰ����; viewport��OpenGL��ʽ���ӿڣ� 
				//fid����vid�ֱ��Ǳ�ʰȡ����Ͷ��㡣Ҳ����˵���ú��������������Ϣ�Ϳ��Լ������ѡ�еĶ������
				Eigen::Vector2f (x, y),//�������λ��
				viewer.core ().view,//ģ��
				viewer.core ().proj,//ͶӰ����
				viewer.core ().viewport,//OpenGL��ʽ���ӿ�
				V, F, fid, bc
			)
				) {
				// paint hit red
				std::cout << "ѡ����" << fid << "��ƬԪ\n";
				C.row (pfid >= C.size () ? 0 : pfid) << 1, 1, 1;//��pfid����ĳɰ�ɫ��Ҳ��ǰһ�����޸ĵ�main�ĳɺ�ɫ
				C.row (fid) << 1, 0, 0;//��fid����ĳɺ�ɫ
				pfid = fid;
				viewer.data ().set_colors (C);
				return true;
			}
			return false;
		}
		return false;

	};
	std::cout << "����϶���ת\n�����������ƬԪ\n������ת����\n���ֵ����л�ģ��\n�Ҽ��϶�ƽ��\n";

	//����ģ�ͣ�ÿ���л�ģ��Ҫ���¼���
	viewer.data ().clear ();
	viewer.data ().set_mesh (V, F);
	viewer.data ().set_colors (C);

	viewer.data ().show_lines = true;
	viewer.launch ();
}
