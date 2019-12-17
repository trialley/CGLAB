#include<queue>
#include<stdio.h>
#include<string.h>
#include<algorithm>
#pragma warning(disable:4996)
using namespace std;

struct onestep {
	int x;
	int y;           //x,y������¼������ˮ��״̬.����Ԫ�ص�xy�������Ӧ
	int last_x;
	int last_y;          //last_x��last_y��¼·����Ҳ����ǰһ������ʱ����ˮ��״̬
	int op_type;           //��1��2��3��4��5��6������ˮ�Ĳ��� ֻ�а���״̬�����ֲ��� 
	//a���� 
	//b���� 
	//a���� 
	//b���� 
	//a��b
	//  a��
	//  b��
	//b��a
	//  a��
	//  b��
	int visited;         //���
	int num;         //��¼��ֹ������ڵ�λ�õ�ˮ���ٲ�
};



void bfs (int cup_a_capacity,int cup_b_capacity,int target_volume ) {
	int current_volume_x, current_volume_y;
	/*��ʼ��״̬��*/
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

	/*���У���¼�߹��Ĺ���·��*/
	queue<onestep>q;
	q.push (steps[0][0]);

	steps[0][0].visited = 1;//��������

	while (q.size ()) {//�����ѻ���·������
		onestep p = q.front ();//ȡ����
		q.pop ();

		current_volume_x = p.x;//��¼��ǰ������״̬���е�λ�ã���Ҳ��
		current_volume_y = p.y;
		if (current_volume_x == target_volume || current_volume_y == target_volume) {//�����ǰ�������������
			int path[10000];
			memset (path, 0, sizeof (path));
			printf ("%d\n", steps[current_volume_x][current_volume_y].num);//��ӡ����
			int sum = steps[current_volume_x][current_volume_y].num;

			onestep* current_step = &steps[current_volume_x][current_volume_y];//��¼��ǰ����Ĳ���
			for (int i = sum; i > 0; i--) {//��ĩβ��ʼ������
				path[i] = current_step->op_type;
				/*��ȡ��һ������Ĳ���*/
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
		/*���ڵ�ǰ״̬�����������������ֲ����Ͱ���״̬*/
		if (steps[cup_a_capacity][p.y].visited == 0) {    //����ǰa���Ӳ���״̬δ���ʹ������������a����¼����
			steps[cup_a_capacity][p.y].last_x = p.x;
			steps[cup_a_capacity][p.y].last_y = p.y;
			steps[cup_a_capacity][p.y].visited = 1;
			steps[cup_a_capacity][p.y].num = steps[p.x][p.y].num + 1;
			steps[cup_a_capacity][p.y].op_type = 1;
			q.push (steps[cup_a_capacity][p.y]);
		}
		if (steps[0][p.y].visited == 0) {  //������a����δ�����ʣ��򵹿ղ���¼����
			steps[0][p.y].last_x = p.x;
			steps[0][p.y].last_y = p.y;
			steps[0][p.y].visited = 1;
			steps[0][p.y].num = steps[p.x][p.y].num + 1;
			steps[0][p.y].op_type = 2;
			q.push (steps[0][p.y]);
		}
		if (steps[p.x][cup_b_capacity].visited == 0) { //����b������
			steps[p.x][cup_b_capacity].last_x = p.x;
			steps[p.x][cup_b_capacity].last_y = p.y;
			steps[p.x][cup_b_capacity].visited = 1;
			steps[p.x][cup_b_capacity].num = steps[p.x][p.y].num + 1;
			steps[p.x][cup_b_capacity].op_type = 3;
			q.push (steps[p.x][cup_b_capacity]);
		}
		if (steps[p.x][0].visited == 0) {//����b����
			steps[p.x][0].last_x = p.x;
			steps[p.x][0].last_y = p.y;
			steps[p.x][0].visited = 1;
			steps[p.x][0].num = steps[p.x][p.y].num + 1;
			steps[p.x][0].op_type = 4;
			q.push (steps[p.x][0]);
		}

		a_left = p.x + p.y - cup_b_capacity;

		if (a_left >= 0 && steps[a_left][cup_b_capacity].visited == 0) {   //a����b��a��ʣ�࣬b�����
			steps[a_left][cup_b_capacity].last_x = p.x;
			steps[a_left][cup_b_capacity].last_y = p.y;
			steps[a_left][cup_b_capacity].visited = 1;
			steps[a_left][cup_b_capacity].num = steps[p.x][p.y].num + 1;
			steps[a_left][cup_b_capacity].op_type = 5;
			q.push (steps[a_left][cup_b_capacity]);
		}
		if (a_left < 0 && steps[0][p.x + p.y].visited == 0) {    //a����b��a�գ�b�����
			steps[0][p.x + p.y].last_x = p.x;
			steps[0][p.x + p.y].last_y = p.y;
			steps[0][p.x + p.y].visited = 1;
			steps[0][p.x + p.y].num = steps[p.x][p.y].num + 1;
			steps[0][p.x + p.y].op_type = 5;
			q.push (steps[0][p.x + p.y]);
		}
		b_left = p.x + p.y - cup_a_capacity;
		if (b_left >= 0 && steps[cup_a_capacity][b_left].visited == 0) { //b����a��a����b�����
			steps[cup_a_capacity][b_left].last_x = p.x;
			steps[cup_a_capacity][b_left].last_y = p.y;
			steps[cup_a_capacity][b_left].visited = 1;
			steps[cup_a_capacity][b_left].num = steps[p.x][p.y].num + 1;
			steps[cup_a_capacity][b_left].op_type = 6;
			q.push (steps[cup_a_capacity][b_left]);
		}
		if (b_left < 0 && steps[p.x + p.y][0].visited == 0) {//b����a��b�գ�a�����
			steps[p.x + p.y][0].last_x = p.x;
			steps[p.x + p.y][0].last_y = p.y;
			steps[p.x + p.y][0].visited = 1;
			steps[p.x + p.y][0].num = steps[p.x][p.y].num + 1;
			steps[p.x + p.y][0].op_type = 6;
			q.push (steps[p.x + p.y][0]);
		}
	}

	printf ("�����������޷�������%d��ˮ\n", target_volume);
}

int main () {
	int cup_a_capacity, cup_b_capacity, target_volume;
	scanf ("%d %d %d", &cup_a_capacity, &cup_b_capacity, &target_volume);
	bfs ( cup_a_capacity, cup_b_capacity, target_volume);
	return 0;
}