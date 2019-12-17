//#include<iostream>
//#include<cstdio>
//#include<cstring>
//#include<algorithm>
//using namespace std;
//int x, y, z, ans = 1e8, map[105][105];
//void dfs (int a, int b, int c) {
//
//	if (map[a][b] && map[a][b] <= c) //剪枝1：同种情况只考虑次数最少的一种。 
//		return;
//	map[a][b] = c;
//	if (c >= ans)  //剪枝 2 ：若步数 > ans，则无法更新答案。 
//		return;
//	if (a == z || b == z)
//		ans = c;  // 由于c > ans 的情况被剪枝，不用写 ans = min（ans，c）； 
//	dfs (x, b, c + 1); // a 装满 
//	dfs (a, y, c + 1); // b 装满 
//	dfs (0, b, c + 1); // a 倒净 
//	dfs (a, 0, c + 1); // b 倒净 
//	int t = a + b;
//	if (t > x)   // b 倒入 a 中但 a 溢出，则 b中还有剩余。 
//		dfs (x, t - x, c + 1);
//	else
//		dfs (t, 0, c + 1);// b 全部倒入 a 
//	if (t > y)   // a 倒入 b 中但 b 溢出，则 a中还有剩余。 
//		dfs (t - y, y, c + 1);
//	else
//		dfs (0, t, c + 1); // a全部倒入b； 
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