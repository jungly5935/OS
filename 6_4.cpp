#include <stdio.h>
#include <vector>
#pragma warning (disable:4996)
using namespace std;
#define MAX -99999
FILE* inp = fopen("page.inp", "rt");
FILE* out = fopen("page.out", "wt");


typedef struct insert {
	int PID;
	int page;
}insert;

typedef struct Memory {
	int PID;
	int page;
	int num;
	int cnt;
}Memory;

struct process {
	int pid = -1;
	int page = -1;
	int num;
	vector <int> flag;
};

vector<insert> Input;
vector<Memory> memory;
int frame;
int memory_size = 0, page_fault = 0, seq_Num = 0, pre, cnt = 0;


int SIZE = 0;
void FIFO()
{
	int FIFO_page_fault = 0;
	struct process memory[12345];
	int cnt[9999] = { 0 }; // cnt <= frame ����
	vector <int> flag; //2�����迭���� �ε��� = pid, �ȿ��� ��������
	struct process index[12345];
	vector <int>::iterator iter = flag.begin();
	int pid, page;
	for (int i = 0; i < 12345; i++) {

		pid = Input[i].PID;
		page = Input[i].page;

		if (pid == -1 || page == -1) break; //���� ����

		int next = -1;
		cnt[pid] += 1;

		for (int k = 0; k < index[pid].flag.size(); k++) {
			if (index[pid].flag[k] == page) //for������ index�ȿ� �ִ� page�� �Է¹��� page�� �������� break
			{
				next = 0;
				cnt[pid] -= 1;
				break;
			}
		}

		if (cnt[pid] > frame) //�����Ӻ��� ũ�� �޸𸮿� �ٷ� ���� x 
		{
			for (int j = 0; j < 12345; j++)
			{
				if (next == 0) break;
				//�ش� pid���� page ���� ���ϱ�

				if (memory[j].pid == pid) { //0������ Ȯ�� �� �ش��ϴ� pid�̸�
					if (next == 0)break;
					index[pid].flag.push_back(page); //������ �ε����� page�߰�

					for (int k = 0; k < 12345; k++) {

						if (memory[k].pid == pid && memory[k].page == index[pid].flag[0]) //0��° �� ���� page�� ������
						{
							memory[k].pid = pid;
							memory[k].page = page;
							next = 0;
							FIFO_page_fault++;
							break;
						}
						else // �ٸ��� 
							continue;
					}
					index[pid].flag.erase(index[pid].flag.begin()); //0��° �ε��� ����
				}

			}
		}

		else {//�޸𸮿� �� �Ҵ� �� �� �ִ� ���
			if (next == 0)continue;
			memory[i].pid = pid;
			memory[i].page = page;
			index[pid].pid = pid;
			index[pid].page = page;
			index[pid].flag.push_back(page); //pid��° �ε����� flag �ȿ� page ������� ����
			FIFO_page_fault++;
		}

	}
	fprintf(out, "FIFO: %d\n", FIFO_page_fault);
	for (int i = 0, j = 0; i < 12345; i++)
	{
		if (memory[i].pid == -1) continue;
		if (memory[i].pid != -1)SIZE++;
		fprintf(out, "%d %d %d\n", j, memory[i].pid, memory[i].page);
		j++;
	}

}
void LRU(int PID, int page) {


	int frame_cnt = 0;
	for (int i = 0; i < memory_size; i++) { //�޸� size��ŭ �Է¹��� pageȮ�ΰ� ���� ����
		if (memory[i].PID == PID && memory[i].page == page) {
			memory[i].num = seq_Num++;
			return;
		} // �޸��� PId, page�� �Է¹����Ͱ� �����ϱ� �ߴ�
		else if (memory[i].PID == PID && memory[i].page != page) {
			frame_cnt++;
			if (frame_cnt == frame) break;
		} // �޸��� PID�� �Է�PID������
	}

	//frame�������� ���� ���� ���
	if (frame_cnt == frame) {
		int replace = 0;
		int replace_seq = -MAX;
		for (int i = 0; i < memory_size; i++) {
			if (memory[i].PID == PID && replace_seq > memory[i].num) {
				replace_seq = memory[i].num;
				replace = i;
				if (replace_seq == MAX) break;
			}
		}
		if (replace_seq == MAX) {
			for (int i = replace; i < memory_size; i++) {
				if (memory[i].PID == PID && memory[i].num == MAX && memory[i].cnt < memory[replace].cnt) {
					replace = i; //��ü �� �ε���
				}
			}
		}
		memory[replace].PID = PID;
		memory[replace].page = page; //��ü �� �ε��� replace �� PID �� page ����
		memory[replace].num = seq_Num;
		//replace ó�� �� update
	}
	else { //�޸𸮿� �Ҵ� ������ ���

		memory[memory_size].PID = PID;
		memory[memory_size].page = page; //��� ���ϴ� ���� 
		memory[memory_size].num = seq_Num;
		memory_size++; //�޸𸮿� �Ҵ�� Input�� ũ��
	}
	seq_Num++;
	page_fault++;

}

void OPT(int PID, int page) {
	int frame_cnt = 0;
	for (int i = 0; i < memory_size; i++) {
		if (memory[i].num != MAX) memory[i].num++;
	}

	for (int i = 0; i < memory_size; i++) { //�޸𸮻����ŭ page����
		if (memory[i].PID == PID && memory[i].page == page) {
			int replace_seq = 0;
			for (int j = pre + 1; j < Input.size(); j++) {
				memory[i].num = replace_seq;
				replace_seq--;
				if (Input[j].PID == PID && Input[j].page == page) {
					break;
				}
				if (j == Input.size() - 1) memory[i].num = MAX;
			}
			return;
		} // �޸��� PId, page�� �Է¹����Ͱ� �����ϱ� �ߴ�
		else if (memory[i].PID == PID && memory[i].page != page) {
			frame_cnt++;
			if (frame_cnt == frame) break;
		} // �޸��� PID�� �Է�PID������
	}

	if (frame_cnt == frame) { //frame cnt�� frame�ʰ��ؼ� �޸��Ҵ� �Ұ�
		int replace = 0, replace_seq = -MAX;
		for (int i = 0; i < memory_size; i++) {
			if (memory[i].PID == PID && replace_seq > memory[i].num) {
				replace_seq = memory[i].num, replace = i;
				if (replace_seq == MAX) break;
			}
		}
		if (replace_seq == MAX) {
			for (int i = replace; i < memory_size; i++) {
				if (memory[i].PID == PID && memory[i].num == MAX && memory[i].cnt < memory[replace].cnt) {
					replace = i;
				}
			}
		}
		memory[replace].PID = PID;
		memory[replace].page = page;
		int victim_Seq2 = 0;
		for (int i = pre + 1; i < Input.size(); i++) {
			memory[replace].num = victim_Seq2;
			victim_Seq2--;
			if (Input[i].PID == PID && Input[i].page == page) {
				break;
			}
			if (i == Input.size() - 1) memory[replace].num = MAX;
		}
		memory[replace].cnt = cnt++;

	}
	else { //�޸𸮿� �� �Ҵ� ������ ���

		memory[memory_size].PID = PID;
		memory[memory_size].page = page;
		int replace_seq = 0;
		for (int i = pre + 1; i < Input.size(); i++) {
			memory[memory_size].num = replace_seq;
			replace_seq--;
			if (Input[i].PID == PID && Input[i].page == page) {
				break;
			}
			if (i == Input.size() - 1) memory[memory_size].num = MAX;
		}
		memory[memory_size].cnt = cnt++;
		memory_size++;
	}
	seq_Num++;
	page_fault++;

}

void LRU_print()
{
	for (int i = 0; i < Input.size(); i++) LRU(Input[i].PID, Input[i].page);
	fprintf(out, "LRU: %d\n", page_fault - 1);
	for (int i = 0; i < SIZE; i++) //LRU���
	{
		fprintf(out, "%d %d %d\n", i, memory[i].PID, memory[i].page);
		memory[i].PID = 0, memory[i].page = 0, memory[i].num = 0; //��� �ʱ�ȭ
	}
	page_fault = 0;
	memory_size = 0;
	seq_Num = 0;

}

void OPT_print()
{
	for (int i = 0; i < Input.size(); i++) {
		pre = i, OPT(Input[i].PID, Input[i].page);
	}
	fprintf(out, "OPT: %d\n", page_fault - 1);
	for (int i = 0; i < SIZE; i++) //OPT���
	{
		fprintf(out, "%d %d %d\n", i, memory[i].PID, memory[i].page);

	}

}
int main()
{

	fscanf(inp, "%d", &frame);

	for (int i = 0;i < -MAX;i++)
	{
		struct insert x;
		fscanf(inp, "%d %d", &x.PID, &x.page);
		Input.push_back(x);
		if (x.PID == -1 && x.page == -1) break;
	}
	memory.resize(Input.size()); //memory�� Input������ ����
	//�Է¹ޱ�

	FIFO();
	LRU_print();
	OPT_print();

	fclose(out);
	fclose(inp);
	return 0;
}