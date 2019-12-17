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
	Eigen::MatrixXd V;//保存点信息
	Eigen::MatrixXi F;//保存面信息
	igl::readOFF ("../off/cube.off", V, F);// Load a mesh in OFF format

	Eigen::MatrixXd C = Eigen::MatrixXd::Constant (F.rows (), 3, 1);//保存被选中的点 Eigen数学库
	
	igl::opengl::glfw::Viewer viewer;
	int pfid = 0;
	viewer.callback_mouse_down =[&V, &F, &C,&pfid](igl::opengl::glfw::Viewer& viewer, int button, int)->bool {
		if (button == (int)igl::opengl::glfw::Viewer::MouseButton::Middle) {
			std::cout << "请选择您想要打开的off模型：";
			std::string temp = getFilePathName ();
			std::cout << temp<<"\n";

			igl::readOFF (temp, V, F);// Load a mesh in OFF format
			C = Eigen::MatrixXd::Constant (F.rows (), 3, 1);//保存被选中的点 Eigen数学库
			
			viewer.data ().clear();
			viewer.data ().set_mesh (V, F);
			viewer.data ().set_colors (C);
			return true;
		} else if (button == (int)igl::opengl::glfw::Viewer::MouseButton::Left) {
			int fid;//被选中的片元的id
			Eigen::Vector3f bc;

			/*确定鼠标位置，需要注意win下y轴与正常坐标轴是相反的*/
			double x = viewer.current_mouse_x;
			double y = viewer.core ().viewport (3) - viewer.current_mouse_y;

			if (igl::unproject_onto_mesh (//这个函数返回是否选中片元，视图，模型和投影矩阵; viewport是OpenGL格式的视口； 
				//fid并且vid分别是被拾取的面和顶点。也就是说，该函数仅靠传入的信息就可以计算出被选中的顶点和面
				Eigen::Vector2f (x, y),//鼠标点击的位置
				viewer.core ().view,//模型
				viewer.core ().proj,//投影矩阵
				viewer.core ().viewport,//OpenGL格式的视口
				V, F, fid, bc
			)
				) {
				// paint hit red
				std::cout << "选中了" << fid << "号片元\n";
				C.row (pfid >= C.size () ? 0 : pfid) << 1, 1, 1;//第pfid个面改成白色，也即前一个被修改的main改成红色
				C.row (fid) << 1, 0, 0;//第fid个面改成红色
				pfid = fid;
				viewer.data ().set_colors (C);
				return true;
			}
			return false;
		}
		return false;

	};
	std::cout << "左键拖动旋转\n左键单击高亮片元\n滚轮旋转缩放\n滚轮单击切换模型\n右键拖动平移\n";

	//加载模型，每次切换模型要重新加载
	viewer.data ().clear ();
	viewer.data ().set_mesh (V, F);
	viewer.data ().set_colors (C);

	viewer.data ().show_lines = true;
	viewer.launch ();
}
