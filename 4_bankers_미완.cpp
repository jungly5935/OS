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
vector<int> ready_number; //레디큐에 들어온 순서
vector<int> ready_flag;
vector< vector<int> > need;
vector< vector<int> > allocation;

int sum[50] = { 0 };

void Request(
	int available[50], int request[50], int release[50], int process_num)
{
	int flag = 0;

	for (int i = 0; i < m; i++) {

		if (request[i] > need[process_num][i]) //잘못된 요청
			return;

		if (available[i] < request[i]) //available이 request를 만족 시켜야
			flag = -1; //하나라도 만족 안되면 -1
	}


	for (int i = 0; i < m; i++)
	{
		if (flag == 0) // available이 충족 되면
		{
			need[process_num][i] = need[process_num][i] - request[i];
			allocation[process_num][i] = allocation[process_num][i] + request[i];
			available[i] = available[i] - request[i];
		}
		else // available이 충족되지 않으면 레디큐에 삽입
		{
			ready[process_num][i] = request[i];
		}

	}
	if (flag == -1) {
		ready_number.push_back(process_num); //레디큐에 들어온 순서
		ready_flag.push_back(0); // 0으로 flag벡터 추가
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


	/* 모든 ready 안에 들어있는 프로세스들의 자원 중에서
		현재 available으로 만족 할 수 있는 것이 있다면 사용!
	*/
	for (int i = 0; i < ready_flag.size(); i++)
	{
		ready_flag[i] = 0;
	}

	for (int i = 0; i < ready_number.size(); i++) {
		for (int j = 0; j < m; j++) {
			if (available[j] < ready[ready_number[i]][j] || ready[ready_number[i]][j] > need[ready_number[i]][j])  //첫번째 레디큐 순서부터 충족되는지 확인 해야...여기가문제!!
			{
				ready_flag[i] = -1; //하나라도 만족 안되면 flag[i]는 -1
				break;
			}
		}
	}

	int index = 0;
	int a = -1;
	//flag[j]가 0 인 것 중, 레디큐순서 앞부터 보기
	while (!ready_number.empty()) {

		int flag_count = 0; // flag가 -1인 개수
		for (int i = 0; i < ready_flag.size(); i++)
		{
			if (ready_flag[i] == -1) flag_count++;
		}
		if (flag_count == ready_number.size()) break; // 모두 -1이면 계산할 필요x
		//flag가 0 인것이 하나라도 있다면 계산


		for (int i = 0; i < ready_number.size(); i++) {
			if (ready_flag[i] == 0) {
				a = ready_number[i]; //available - ready을 만족하는 것만 a에 저장
				index = i;
				ready_flag[i] = -1; //중복 없애려고 -1
				break;
			}
		}

		for (int i = 0; i < m; i++) {
			available[i] = available[i] - ready[a][i]; //flag[i]가 0 인 것 중에서만.
			need[a][i] = need[a][i] - ready[a][i];
			allocation[a][i] = allocation[a][i] + ready[a][i];
		}

		for (int i = 0; i < m; i++)
		{
			ready[a][i] = 0;// 레디큐에 있는 flag[i] 가 -1인 요소들 0으로 초기화
		}
		ready_number.erase(ready_number.begin() + index); // 요청 처리한 프로세스는 레디큐 순서에서 제거
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

	fscanf(inp, "%d %d", &n, &m); //프로세스 수, 자원 종류


	for (int i = 0; i < m; i++) {
		fscanf(inp, "%d", &resource[i]); //총 자원 개수
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
			fscanf(inp, "%d", &process_num); // 프로세스 i가 요청, 0이라면
			for (int i = 0; i < m; i++) {
				fscanf(inp, "%d", &request[i]);
			}
			Request(available, request, release, process_num);
		}
		else if (strcmp(text, "release") == 0)
		{
			fscanf(inp, "%d", &process_num); // 프로세스 i가 요청, 0이라면
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