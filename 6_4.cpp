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
	int cnt[9999] = { 0 }; // cnt <= frame 개수
	vector <int> flag; //2차원배열으로 인덱스 = pid, 안에는 순서저장
	struct process index[12345];
	vector <int>::iterator iter = flag.begin();
	int pid, page;
	for (int i = 0; i < 12345; i++) {

		pid = Input[i].PID;
		page = Input[i].page;

		if (pid == -1 || page == -1) break; //종료 조건

		int next = -1;
		cnt[pid] += 1;

		for (int k = 0; k < index[pid].flag.size(); k++) {
			if (index[pid].flag[k] == page) //for문으로 index안에 있는 page와 입력받은 page가 같은경우는 break
			{
				next = 0;
				cnt[pid] -= 1;
				break;
			}
		}

		if (cnt[pid] > frame) //프레임보다 크면 메모리에 바로 저장 x 
		{
			for (int j = 0; j < 12345; j++)
			{
				if (next == 0) break;
				//해당 pid에서 page 순서 정하기

				if (memory[j].pid == pid) { //0번부터 확인 후 해당하는 pid이면
					if (next == 0)break;
					index[pid].flag.push_back(page); //마지막 인덱스에 page추가

					for (int k = 0; k < 12345; k++) {

						if (memory[k].pid == pid && memory[k].page == index[pid].flag[0]) //0번째 인 곳과 page가 같을때
						{
							memory[k].pid = pid;
							memory[k].page = page;
							next = 0;
							FIFO_page_fault++;
							break;
						}
						else // 다르면 
							continue;
					}
					index[pid].flag.erase(index[pid].flag.begin()); //0번째 인덱스 제거
				}

			}
		}

		else {//메모리에 더 할당 할 수 있는 경우
			if (next == 0)continue;
			memory[i].pid = pid;
			memory[i].page = page;
			index[pid].pid = pid;
			index[pid].page = page;
			index[pid].flag.push_back(page); //pid번째 인덱스의 flag 안에 page 순서대로 저장
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
	for (int i = 0; i < memory_size; i++) { //메모리 size만큼 입력받은 page확인과 순서 결정
		if (memory[i].PID == PID && memory[i].page == page) {
			memory[i].num = seq_Num++;
			return;
		} // 메모리의 PId, page가 입력받은것과 같으니까 중단
		else if (memory[i].PID == PID && memory[i].page != page) {
			frame_cnt++;
			if (frame_cnt == frame) break;
		} // 메모리의 PID와 입력PID같을때
	}

	//frame공간보다 많이 쓰는 경우
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
					replace = i; //대체 할 인덱스
				}
			}
		}
		memory[replace].PID = PID;
		memory[replace].page = page; //대체 할 인덱스 replace 에 PID 와 page 삽입
		memory[replace].num = seq_Num;
		//replace 처리 후 update
	}
	else { //메모리에 할당 가능한 경우

		memory[memory_size].PID = PID;
		memory[memory_size].page = page; //희생 당하는 지점 
		memory[memory_size].num = seq_Num;
		memory_size++; //메모리에 할당된 Input의 크기
	}
	seq_Num++;
	page_fault++;

}

void OPT(int PID, int page) {
	int frame_cnt = 0;
	for (int i = 0; i < memory_size; i++) {
		if (memory[i].num != MAX) memory[i].num++;
	}

	for (int i = 0; i < memory_size; i++) { //메모리사이즈만큼 page순서
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
		} // 메모리의 PId, page가 입력받은것과 같으니까 중단
		else if (memory[i].PID == PID && memory[i].page != page) {
			frame_cnt++;
			if (frame_cnt == frame) break;
		} // 메모리의 PID와 입력PID같을때
	}

	if (frame_cnt == frame) { //frame cnt가 frame초과해서 메모리할당 불가
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
	else { //메모리에 더 할당 가능한 경우

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
	for (int i = 0; i < SIZE; i++) //LRU출력
	{
		fprintf(out, "%d %d %d\n", i, memory[i].PID, memory[i].page);
		memory[i].PID = 0, memory[i].page = 0, memory[i].num = 0; //모두 초기화
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
	for (int i = 0; i < SIZE; i++) //OPT출력
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
	memory.resize(Input.size()); //memory에 Input사이즈 복사
	//입력받기

	FIFO();
	LRU_print();
	OPT_print();

	fclose(out);
	fclose(inp);
	return 0;
}