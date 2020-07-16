#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include<algorithm>
#pragma warning(disable:4996)
using namespace std;
int sum;


struct process {
	int num;
	int arrive;
	int cpu;
	int wait = 0;
	int stop = 0; //남은 cpu 값
	int flag = -1;
};

bool cmp1(const process& p1, const process& p2) {
	if (p1.arrive < p2.arrive) {
		return true;
	}
	else if (p1.arrive == p2.arrive) {
		return p1.num < p2.num;
	}
	else {
		return false;
	}
}

bool cmp2(const process& p1, const process& p2) { // burst_time정렬
	if (p1.arrive <= sum && p2.arrive <= sum) {
		if (p1.cpu == p2.cpu) {
			return p1.num < p2.num;
		}
		return p1.cpu < p2.cpu;
	}
	return p1.arrive < p2.arrive;
}



int main()
{
	FILE* out = fopen("srtf.out", "wt");
	FILE* inp = fopen("srtf.inp", "rt");

	int test;
	int waiting = 0;
	int i = 0, j = 0;
	int num, arrive, cpu;

	fscanf(inp, "%d", &test); //프로세스 개수
	vector<process> p;
	process process;


	for (int i = 0; i < test; i++) {
		fscanf(inp, "%d", &process.num);
		fscanf(inp, "%d", &process.arrive);
		fscanf(inp, "%d", &process.cpu);

		p.push_back(process);

	}

	int flag = 0; //extra 값 확인

	sort(p.begin(), p.end(), cmp1); //arrive 순으로 정렬

	for (int i = 0;i < test;i++) {

		/*
		if (sum - p[i].arrive < 0)
		{
			sum = p[i].cpu + p[i].arrive;
		}
		else
		{
			p[i].wait = sum - p[i].arrive;
			sum = sum + p[i].cpu;

		}
		*/


		if (p[i].cpu > p[i + 1].cpu) //그 다음 cpu가 더 작으면
		{
			p[i].stop = p[i + 1].arrive - p[i].arrive;
			sum = sum + p[i].stop;
			p[i].wait = sum - p[i + 1].arrive;
		}
		else
		{
			if (sum - p[i].arrive < 0)
			{
				sum = p[i].cpu + p[i].arrive;
			}
			else {
				sum = sum + p[i].cpu;
			}
		}



		waiting = waiting + p[i].wait;

	}



	fprintf(out, "%d", waiting);

	fclose(inp);
	fclose(out);
	return 0;
}


