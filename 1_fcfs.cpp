#include <stdio.h>
#include <stdlib.h>

struct process {
	int num;
	int arrive;
	int cpu;
	int wait = 0;
};

int compare(const void* first, const void* second)
{
	struct process* pa = (struct process*)first;
	struct process* pb = (struct process*)second;

	if (pa->arrive > pb->arrive)
		return 1;
	else if (pa->arrive == pb->arrive)
	{
		if (pa->num < pb->num)	return pa->num < pb->num;
		else return pa->num > pb->num;
	}
	else //pa->arrive < pb->arrive
		return 0;
}

int main()
{
	FILE* out = fopen("fcfs.out", "wt");
	FILE* inp = fopen("fcfs.inp", "rt");

	int test;
	int waiting = 0;
	int i;

	fscanf(inp, "%d", &test); //프로세스 개수

	struct process p[50];
	int sum = 0;

	for (int k = 0; k < test; k++) {
		fscanf(inp, "%d", &p[k].num);
		fscanf(inp, "%d", &p[k].arrive);
		fscanf(inp, "%d", &p[k].cpu);
	}
	//정렬
	qsort(p, test, sizeof(process), compare);

	p[0].wait = p[0].arrive;

	for (int i = 1; i < test; i++)
	{
		sum = sum + p[i - 1].cpu;
		p[i].wait = sum - p[i].arrive; // wait = (그전프로세서 cpu사용시간 합 - p[i]도착시간)


	}//테스트케이스

	for (int k = 0; k < test; k++)
	{
		waiting = waiting + p[k].wait;
	}

	fprintf(out, "%d", waiting);

	fclose(inp);
	fclose(out);
	return 0;
}