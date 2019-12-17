#pragma once
#include<Windows.h>
//#include<commdlg.h>
#include<string>
#include<algorithm>
using namespace std;

std::string TCHAR2STRING (TCHAR* str) {
	std::string strstr;
	try {
		int iLen = WideCharToMultiByte (CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

		char* chRtn = new char[iLen * sizeof (char)];

		WideCharToMultiByte (CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);

		strstr = chRtn;
	} catch (std::exception e) {
	}

	return strstr;
}
std::string getFilePathName () {
	for (;;) {
		OPENFILENAME ofn = { 0 };
		TCHAR strFileName[MAX_PATH] = { 0 };	//用于接收文件名
		ofn.lStructSize = sizeof (OPENFILENAME);	//结构体大小
		ofn.hwndOwner = NULL;					//拥有着窗口句柄
		ofn.lpstrFilter = TEXT (
			"All\0*.*\0"
			"jpg\0*.jpg\0"
			"bmp\0*.bmp\0"
			"\0"
		);	//设置过滤
		ofn.nFilterIndex = 1;	//过滤器索引
		ofn.lpstrFile = strFileName;	//接收返回的文件名，注意第一个字符需要为NULL
		ofn.nMaxFile = sizeof (strFileName);	//缓冲区长度
		ofn.lpstrInitialDir = NULL;			//初始目录为默认
		ofn.lpstrTitle = TEXT ("请选择一个文件"); //窗口标题
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //文件、目录必须存在，隐藏只读选项
		//打开文件对话框
		if (GetOpenFileName (&ofn)) {
			string temp(TCHAR2STRING(strFileName));
			//replace (temp.begin (), temp.end (), '\\\\', '/');
			std::cout << temp;
			return temp;
		} else {
			MessageBox (NULL, TEXT ("请选择一文件"), NULL, MB_ICONERROR);
		}
}
}

