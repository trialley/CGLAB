//#include<iostream>
//#include<cstdio>
//#include<cstring>
//#include<algorithm>
//using namespace std;
//int x, y, z, ans = 1e8, map[105][105];
//void dfs (int a, int b, int c) {
//
//	if (map[a][b] && map[a][b] <= c) //��֦1��ͬ�����ֻ���Ǵ������ٵ�һ�֡� 
//		return;
//	map[a][b] = c;
//	if (c >= ans)  //��֦ 2 �������� > ans�����޷����´𰸡� 
//		return;
//	if (a == z || b == z)
//		ans = c;  // ����c > ans ���������֦������д ans = min��ans��c���� 
//	dfs (x, b, c + 1); // a װ�� 
//	dfs (a, y, c + 1); // b װ�� 
//	dfs (0, b, c + 1); // a ���� 
//	dfs (a, 0, c + 1); // b ���� 
//	int t = a + b;
//	if (t > x)   // b ���� a �е� a ������� b�л���ʣ�ࡣ 
//		dfs (x, t - x, c + 1);
//	else
//		dfs (t, 0, c + 1);// b ȫ������ a 
//	if (t > y)   // a ���� b �е� b ������� a�л���ʣ�ࡣ 
//		dfs (t - y, y, c + 1);
//	else
//		dfs (0, t, c + 1); // aȫ������b�� 
//}
//int main () {
//#pragma warning(disable:4996)
//	scanf ("%d%d%d", &x, &y, &z);
//	if (z > x&& z > y)
//		puts ("impossible");
//	else {
//		dfs (0, 0, 0);
//		if (ans == 1e8)
//			puts ("impossible");
//		else
//			printf ("%d", ans);
//	}
//}