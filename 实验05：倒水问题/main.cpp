#include<queue>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#pragma warning(disable:4996)
using namespace std;

struct onestep {
	int x;
	int y;           //x,y用来记录杯子中水的状态.跟该元素的xy坐标相对应
	int last_x;
	int last_y;          //last_x，last_y记录路径，也就是前一步操作时杯中水的状态
	int op_type;           //用1，2，3，4，5，6，代表倒水的操作 只有八种状态和六种操作 
	//a倒空 
	//b倒空 
	//a倒满 
	//b倒满 
	//a入b
	//  a空
	//  b满
	//b入a
	//  a满
	//  b空
	int visited;         //标记
	int num;         //记录截止到这个节点位置倒水多少步
};



void bfs (int cup_a_capacity,int cup_b_capacity,int target_volume ) {
	int current_volume_x, current_volume_y;
	/*初始化状态表*/
	onestep** steps=new onestep*[cup_a_capacity+1];
	for (int i = 0; i <= cup_a_capacity;i++) {
		steps[i]= new onestep[cup_b_capacity+1];
	}

	for (int i = 0; i <= cup_a_capacity; i++) {
		for (int j = 0; j <= cup_b_capacity; j++) {
			steps[i][j].x = i;
			steps[i][j].y = j;
			steps[i][j].last_x = 0;
			steps[i][j].last_y = 0;
			steps[i][j].op_type = 0;
			steps[i][j].num = 0;
			steps[i][j].visited = 0;
		}
	}

	/*队列，记录走过的广搜路径*/
	queue<onestep>q;
	q.push (steps[0][0]);

	steps[0][0].visited = 1;//起点打个标记

	while (q.size ()) {//当深搜还有路径可走
		onestep p = q.front ();//取队首
		q.pop ();

		current_volume_x = p.x;//记录当前步骤在状态表中的位置，这也是
		current_volume_y = p.y;
		if (current_volume_x == target_volume || current_volume_y == target_volume) {//如果当前步骤就是所需结果
			int path[10000];
			memset (path, 0, sizeof (path));
			printf ("%d\n", steps[current_volume_x][current_volume_y].num);//打印步数
			int sum = steps[current_volume_x][current_volume_y].num;

			onestep* current_step = &steps[current_volume_x][current_volume_y];//记录当前步骤的操作
			for (int i = sum; i > 0; i--) {//从末尾开始整理步骤
				path[i] = current_step->op_type;
				/*获取上一个步骤的操作*/
				current_step = &steps[current_step->last_x][current_step->last_y];
			}
			for (int i = 1; i <= sum; i++) {
				if (path[i] == 1)      printf ("FILL(1)\n");
				else if (path[i] == 2) printf ("DROonestep(1)\n");
				else if (path[i] == 3) printf ("FILL(2)\n");
				else if (path[i] == 4) printf ("DROP(2)\n");
				else if (path[i] == 5) printf ("POUR(1,2)\n");
				else if (path[i] == 6) printf ("POUR(2,1)\n");
			}
			return;
		}

		int a_left;
		int b_left;
		/*对于当前状态，穷举其接下来的六种操作和八种状态*/
		if (steps[cup_a_capacity][p.y].visited == 0) {    //若当前a杯子不满状态未访问过，则加满杯子a并记录访问
			steps[cup_a_capacity][p.y].last_x = p.x;
			steps[cup_a_capacity][p.y].last_y = p.y;
			steps[cup_a_capacity][p.y].visited = 1;
			steps[cup_a_capacity][p.y].num = steps[p.x][p.y].num + 1;
			steps[cup_a_capacity][p.y].op_type = 1;
			q.push (steps[cup_a_capacity][p.y]);
		}
		if (steps[0][p.y].visited == 0) {  //若杯子a倒空未曾访问，则倒空并记录访问
			steps[0][p.y].last_x = p.x;
			steps[0][p.y].last_y = p.y;
			steps[0][p.y].visited = 1;
			steps[0][p.y].num = steps[p.x][p.y].num + 1;
			steps[0][p.y].op_type = 2;
			q.push (steps[0][p.y]);
		}
		if (steps[p.x][cup_b_capacity].visited == 0) { //杯子b加满，
			steps[p.x][cup_b_capacity].last_x = p.x;
			steps[p.x][cup_b_capacity].last_y = p.y;
			steps[p.x][cup_b_capacity].visited = 1;
			steps[p.x][cup_b_capacity].num = steps[p.x][p.y].num + 1;
			steps[p.x][cup_b_capacity].op_type = 3;
			q.push (steps[p.x][cup_b_capacity]);
		}
		if (steps[p.x][0].visited == 0) {//杯子b倒空
			steps[p.x][0].last_x = p.x;
			steps[p.x][0].last_y = p.y;
			steps[p.x][0].visited = 1;
			steps[p.x][0].num = steps[p.x][p.y].num + 1;
			steps[p.x][0].op_type = 4;
			q.push (steps[p.x][0]);
		}

		a_left = p.x + p.y - cup_b_capacity;

		if (a_left >= 0 && steps[a_left][cup_b_capacity].visited == 0) {   //a倒入b，a有剩余，b不清楚
			steps[a_left][cup_b_capacity].last_x = p.x;
			steps[a_left][cup_b_capacity].last_y = p.y;
			steps[a_left][cup_b_capacity].visited = 1;
			steps[a_left][cup_b_capacity].num = steps[p.x][p.y].num + 1;
			steps[a_left][cup_b_capacity].op_type = 5;
			q.push (steps[a_left][cup_b_capacity]);
		}
		if (a_left < 0 && steps[0][p.x + p.y].visited == 0) {    //a倒入b，a空，b不清楚
			steps[0][p.x + p.y].last_x = p.x;
			steps[0][p.x + p.y].last_y = p.y;
			steps[0][p.x + p.y].visited = 1;
			steps[0][p.x + p.y].num = steps[p.x][p.y].num + 1;
			steps[0][p.x + p.y].op_type = 5;
			q.push (steps[0][p.x + p.y]);
		}
		b_left = p.x + p.y - cup_a_capacity;
		if (b_left >= 0 && steps[cup_a_capacity][b_left].visited == 0) { //b倒入a，a满，b不清楚
			steps[cup_a_capacity][b_left].last_x = p.x;
			steps[cup_a_capacity][b_left].last_y = p.y;
			steps[cup_a_capacity][b_left].visited = 1;
			steps[cup_a_capacity][b_left].num = steps[p.x][p.y].num + 1;
			steps[cup_a_capacity][b_left].op_type = 6;
			q.push (steps[cup_a_capacity][b_left]);
		}
		if (b_left < 0 && steps[p.x + p.y][0].visited == 0) {//b倒入a，b空，a不清楚
			steps[p.x + p.y][0].last_x = p.x;
			steps[p.x + p.y][0].last_y = p.y;
			steps[p.x + p.y][0].visited = 1;
			steps[p.x + p.y][0].num = steps[p.x][p.y].num + 1;
			steps[p.x + p.y][0].op_type = 6;
			q.push (steps[p.x + p.y][0]);
		}
	}

	printf ("这两个杯子无法称量出%d的水\n", target_volume);
}

int main () {
	int cup_a_capacity, cup_b_capacity, target_volume;
	scanf ("%d %d %d", &cup_a_capacity, &cup_b_capacity, &target_volume);
	bfs ( cup_a_capacity, cup_b_capacity, target_volume);
	return 0;
}