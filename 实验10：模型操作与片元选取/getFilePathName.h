#pragma once
#include<Windows.h>
//#include<commdlg.h>
#include<string>

std::string getFilePathName () {
	for (;;) {
		OPENFILENAME ofn = { 0 };
		TCHAR strFileName[MAX_PATH] = { 0 };	//���ڽ����ļ���
		ofn.lStructSize = sizeof (OPENFILENAME);	//�ṹ���С
		ofn.hwndOwner = NULL;					//ӵ���Ŵ��ھ��
		ofn.lpstrFilter = TEXT (
			"All\0*.*\0"
			"jpg\0*.jpg\0"
			"bmp\0*.bmp\0"
			"\0"
		);	//���ù���
		ofn.nFilterIndex = 1;	//����������
		ofn.lpstrFile = strFileName;	//���շ��ص��ļ�����ע���һ���ַ���ҪΪNULL
		ofn.nMaxFile = sizeof (strFileName);	//����������
		ofn.lpstrInitialDir = NULL;			//��ʼĿ¼ΪĬ��
		ofn.lpstrTitle = TEXT ("��ѡ��һ���ļ�"); //���ڱ���
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY; //�ļ���Ŀ¼������ڣ�����ֻ��ѡ��
		//���ļ��Ի���
		if (GetOpenFileName (&ofn)) {
			std::string temp = strFileName;
			return temp;
		} else {
			MessageBox (NULL, TEXT ("��ѡ��һ�ļ�"), NULL, MB_ICONERROR);
		}
}
}

