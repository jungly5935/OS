#include <stdio.h>
#include <string.h>
#include <vector>
#include <iostream>
using namespace std;

#pragma warning(disable:4996)
FILE* out = fopen("banker.out", "wt");
FILE* inp = fopen("banker.inp", "rt");

vector< vector<int> > ready;
int n, m;
vector<int> ready_number; //����ť�� ���� ����
vector<int> ready_flag;
vector< vector<int> > need;
vector< vector<int> > allocation;

int sum[50] = { 0 };

void Request(
	int available[50], int request[50], int release[50], int process_num)
{
	int flag = 0;

	for (int i = 0; i < m; i++) {

		if (request[i] > need[process_num][i]) //�߸��� ��û
			return;

		if (available[i] < request[i]) //available�� request�� ���� ���Ѿ�
			flag = -1; //�ϳ��� ���� �ȵǸ� -1
	}


	for (int i = 0; i < m; i++)
	{
		if (flag == 0) // available�� ���� �Ǹ�
		{
			need[process_num][i] = need[process_num][i] - request[i];
			allocation[process_num][i] = allocation[process_num][i] + request[i];
			available[i] = available[i] - request[i];
		}
		else // available�� �������� ������ ����ť�� ����
		{
			ready[process_num][i] = request[i];
		}

	}
	if (flag == -1) {
		ready_number.push_back(process_num); //����ť�� ���� ����
		ready_flag.push_back(0); // 0���� flag���� �߰�
	}
}
void Release(
	int available[50], int request[50], int release[50], int process_num)
{
	for (int i = 0; i < m; i++)
	{
		available[i] = available[i] + release[i];
		need[process_num][i] = need[process_num][i] + release[i];
		allocation[process_num][i] = allocation[process_num][i] - release[i];
	}


	/* ��� ready �ȿ� ����ִ� ���μ������� �ڿ� �߿���
		���� available���� ���� �� �� �ִ� ���� �ִٸ� ���!
	*/
	for (int i = 0; i < ready_flag.size(); i++)
	{
		ready_flag[i] = 0;
	}

	for (int i = 0; i < ready_number.size(); i++) {
		for (int j = 0; j < m; j++) {
			if (available[j] < ready[ready_number[i]][j] || ready[ready_number[i]][j] > need[ready_number[i]][j])  //ù��° ����ť �������� �����Ǵ��� Ȯ�� �ؾ�...���Ⱑ����!!
			{
				ready_flag[i] = -1; //�ϳ��� ���� �ȵǸ� flag[i]�� -1
				break;
			}
		}
	}

	int index = 0;
	int a = -1;
	//flag[j]�� 0 �� �� ��, ����ť���� �պ��� ����
	while (!ready_number.empty()) {

		int flag_count = 0; // flag�� -1�� ����
		for (int i = 0; i < ready_flag.size(); i++)
		{
			if (ready_flag[i] == -1) flag_count++;
		}
		if (flag_count == ready_number.size()) break; // ��� -1�̸� ����� �ʿ�x
		//flag�� 0 �ΰ��� �ϳ��� �ִٸ� ���


		for (int i = 0; i < ready_number.size(); i++) {
			if (ready_flag[i] == 0) {
				a = ready_number[i]; //available - ready�� �����ϴ� �͸� a�� ����
				index = i;
				ready_flag[i] = -1; //�ߺ� ���ַ��� -1
				break;
			}
		}

		for (int i = 0; i < m; i++) {
			available[i] = available[i] - ready[a][i]; //flag[i]�� 0 �� �� �߿�����.
			need[a][i] = need[a][i] - ready[a][i];
			allocation[a][i] = allocation[a][i] + ready[a][i];
		}

		for (int i = 0; i < m; i++)
		{
			ready[a][i] = 0;// ����ť�� �ִ� flag[i] �� -1�� ��ҵ� 0���� �ʱ�ȭ
		}
		ready_number.erase(ready_number.begin() + index); // ��û ó���� ���μ����� ����ť �������� ����
		ready_flag.erase(ready_flag.begin() + index);
	}


}
int main()
{
	vector< vector<int> > max;
	int request[50];
	int release[50];
	int process_num;

	int resource[50];
	int available[50] = { 0 };

	fscanf(inp, "%d %d", &n, &m); //���μ��� ��, �ڿ� ����


	for (int i = 0; i < m; i++) {
		fscanf(inp, "%d", &resource[i]); //�� �ڿ� ����
	}

	for (int i = 0; i < m; i++)
	{
		vector<int> element(m);
		max.push_back(element);
		allocation.push_back(element);
		ready.push_back(element);
		need.push_back(element);
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			fscanf(inp, "%d", &max[i][j]);
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			fscanf(inp, "%d", &allocation[i][j]);
		}
	}

	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			sum[i] = sum[i] + allocation[j][i];
			need[j][i] = max[j][i] - allocation[j][i];
		}
		available[i] = resource[i] - sum[i];
	}

	while (1) {
		char text[10];
		fscanf(inp, "%s", &text);
		if (strcmp(text, "request") == 0)
		{
			fscanf(inp, "%d", &process_num); // ���μ��� i�� ��û, 0�̶��
			for (int i = 0; i < m; i++) {
				fscanf(inp, "%d", &request[i]);
			}
			Request(available, request, release, process_num);
		}
		else if (strcmp(text, "release") == 0)
		{
			fscanf(inp, "%d", &process_num); // ���μ��� i�� ��û, 0�̶��
			for (int i = 0; i < m; i++) {
				fscanf(inp, "%d", &release[i]);
			}
			Release(available, request, release, process_num);
		}
		else // quit
		{
			return 0;
		}

		for (int i = 0; i < m; i++) {
			fprintf(out, "%d ", available[i]);
		}
		fprintf(out, "\n");
	}

	fclose(out);
	fclose(inp);
	return 0;
}