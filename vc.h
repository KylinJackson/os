#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define		Run		1
#define		Ready	2
#define		Block	3

typedef struct process_list {
	struct process_list* previous;
	struct process_list* next;
	int					 Pid;
} PL;

typedef struct RESOURCE {
	int r[5];
} RESOURCE;

typedef struct PCB {
	struct PCB* previous;
	struct PCB* next;
	int			Pid;
	char*		name;
	RESOURCE	requested;
	RESOURCE	requiring;
	int			type;
	struct PCB*	parent;
	PL*			child;
	int			priority;
} PCB;

typedef struct STATE {
	PL *r[3];
	PL *b[3];
} STATE;

int			_Pid;
STATE		_stt;
RESOURCE	_res;
PCB*		_running;
PCB*		_begin;

PCB* findProcessByPid(int Pid);

void deletePLNode(PL **pQueue, PL* node);

void insertEnd(PL **pQueue, int Pid);

void scheduler();

void init();

void create(char *name, int priority);

PCB* findProcessByName(char *name);

void killChild(PL* list);

void killTree(PCB* process);

void destroy(char *name);

void request(int Rid, int cnt);

void release(int Rid, int cnt);

void timeOut();

void allocate();

void printProcess();

void printResource();

void printInformation(char *name);

// created by Yang tao.
PCB* findProcessByPid(int Pid) {
	PCB* process = _begin;

	while (process) {
		if (process->Pid == Pid) {
			return process;
		}
		else {
			process = process->next;
		}
	}
	return NULL;
}

// created by Zhao yiqi.
void deletePLNode(PL **pQueue, PL* node) {

	if (*pQueue == node) {
		if (node->next) {
			node->next->previous = NULL;
		}
		*pQueue = node->next;
		free(node);
		return;
	}

	if (node->next) node->next->previous = node->previous;
	if (node->previous) node->previous->next = node->next;

	free(node);
}

// created by Luo hao.
void insertEnd(PL **pQueue, int Pid) {
	PL *queue = *pQueue;
	if (!queue) {
		queue = (PL*)malloc(sizeof(PL));
		queue->next = NULL;
		queue->Pid = Pid;
		queue->previous = NULL;
		*pQueue = queue;
		return;
	}

	PL* nextNode = queue->next;

	while (nextNode) {
		queue = nextNode;
		nextNode = nextNode->next;
	}

	nextNode = (PL*)malloc(sizeof(PL));

	nextNode->next = NULL;
	nextNode->Pid = Pid;
	nextNode->previous = queue;
	queue->next = nextNode;
}

// created by Luo hao.
void scheduler() {
	if (!_running) {
		int i;
		for (i = 0; i < 3; i++) {
			if (_stt.r[i]) {
				PL* queue = _stt.r[i];

				_running = findProcessByPid(queue->Pid);
				_stt.r[i] = queue->next;
				if (_stt.r[i]) _stt.r[i]->previous = NULL;
				free(queue);
				break;
			}
		}
	}
	printf("(os) Process %s is running.\n", _running->name);
}

// created by Luo hao.
void init() {
	_Pid = 0;
	_begin = NULL;

	int i;
	for (i = 0; i < 3; i++) {
		_stt.r[i] = NULL;
		_stt.b[i] = NULL;
	}

	_running = NULL;
	for (i = 1; i < 5; i++) _res.r[i] = i;
}

// created by Luo hao.
void create(char *name, int priority) {
	PCB* process = (PCB*)malloc(sizeof(PCB));

	process->next = NULL;

	process->Pid = ++_Pid;

	process->name = name;

	int i;
	for (i = 1; i < 5; i++) {
		process->requested.r[i] = 0;
		process->requiring.r[i] = 0;
	}

	process->type = Ready;

	process->parent = _running;

	process->child = NULL;

	process->priority = priority;

	if (_running) {
		insertEnd(&_running->child, process->Pid);
	}

	insertEnd(&_stt.r[priority], process->Pid);

	if (!_begin) {
		_begin = process;
		process->previous = NULL;
	}
	else {
		PCB* tmp = _begin;
		while (tmp->next) tmp = tmp->next;
		tmp->next = process;
		process->previous = tmp;
	}

	scheduler();
}

// created by Yang tao.
PCB* findProcessByName(char *name) {
	PCB* process = _begin;

	while (process) {
		if (!strcmp(process->name, name)) {
			return process;
		}
		else {
			process = process->next;
		}
	}
	return NULL;
}

// created by Zhao yiqi
void killChild(PL* list) {
	if (list) {
		PCB* process = findProcessByPid(list->Pid);
		killTree(process);
		killChild(list->next);
		free(list);
	}
}

// created by Zhao yiqi
void killTree(PCB* process) {
	if (process) {
		killChild(process->child);
		int i;
		for (i = 0; i < 4; i++) {
			_res.r[i] = _res.r[i] + process->requested.r[i];
		}

		allocate();

		if (_running->Pid == process->Pid) {
			_running = NULL;
		}
		else {
			PL* result;
			int i;
			for (i = 0; i < 3; i++) {
				result = _stt.r[i];

				while (result) {
					if (result->Pid == process->Pid) {
						deletePLNode(&_stt.r[i], result);
						break;
					}
					else {
						result = result->next;
					}
				}

				result = _stt.b[i];

				while (result) {
					if (result->Pid == process->Pid) {
						deletePLNode(&_stt.b[i], result);
						break;
					}
					else {
						result = result->next;
					}
				}
			}
		}

		if (!process->previous && !process->next) {
			_begin = NULL;
		}
		else if (!process->previous) {
			process->next->previous = process->previous;
		}
		else if (!process->next) {
			process->previous->next = process->next;
		}
		else {
			process->next->previous = process->previous;
			process->previous->next = process->next;
		}

		free(process);
	}
}

// created by Zhao yiqi
void allocate() {
	int i;
	for (i = 0; i < 3; i++) {
		PL* queue = _stt.b[i];
		PL* tmp = NULL;

		int allocate = 0;
		while (queue) {
			PCB* process = findProcessByPid(queue->Pid);

			int j;
			for (j = 1; j < 5; j++) {
				if (process->requiring.r[j] <= _res.r[j] && process->requiring.r[j] != 0) {
					allocate = 1;
					process->requested.r[j] = process->requested.r[j] + process->requiring.r[j];
					_res.r[j] = _res.r[j] - process->requiring.r[j];
					process->requiring.r[j] = 0;

					tmp = queue->next;

					deletePLNode(&_stt.b[process->priority], queue);
					insertEnd(&_stt.r[process->priority], process->Pid);
					break;
				}
			}
			if (allocate && j != 5) {
				queue = tmp;
			}
			else {
				queue = queue->next;
			}
		}
	}
}

// created by Zhao yiqi
void destroy(char *name) {
	PCB* process = findProcessByName(name);
	if (process->parent) {
		PL* result = process->parent->child;

		while (result) {
			if (result->Pid = process->Pid) {
				deletePLNode(&process->parent->child, result);
				break;
			}
			else {
				result = result->next;
			}
		}
	}

	killTree(process);

	scheduler();
}

// created by Zhao yiqi
void request(int Rid, int cnt) {
	if (cnt <= _res.r[Rid]) {
		_running->requested.r[Rid] = _running->requested.r[Rid] + cnt;
		_res.r[Rid] = _res.r[Rid] - cnt;
	}
	else {
		_running->requiring.r[Rid] = _running->requiring.r[Rid] + cnt;
		insertEnd(&_stt.b[_running->priority], _running->Pid);
		_running = NULL;
	}
	scheduler();
}

// created by Zhao yiqi
void release(int Rid, int cnt) {
	_running->requested.r[Rid] = _running->requested.r[Rid] - cnt;
	_res.r[Rid] = _res.r[Rid] + cnt;

	allocate();

	scheduler();
}

// created by Luo hao
void timeOut() {
	insertEnd(&_stt.r[_running->priority], _running->Pid);

	_running->type = Ready;
	_running = NULL;

	scheduler();
}

// created by Yang tao.
void printProcess() {
	PCB* head = _begin;
	if (!head) {
		printf("(os) No Process...");
	}
	else {
		while (1) {
			if (!head) {
				break;
			}
			else {
				printf("     Pid:\t%d\n", head->Pid);
				printf("     Name:\t%s\n", head->name);
				printf("     Type:\t");
				switch (head->type) {
				case Run:
					printf("Run\n");
					break;
				case Ready:
					printf("Ready\n");
					break;
				case Block:
					printf("Block\n");
					break;
				}
			}
			head = head->next;
		}
	}
}

// created by Yang tao.
void printResource() {
	PCB* head = _begin;
	int cnt = 0;

	struct term {
		int r;
		char *name;
		int num;
	} table[10];

	int i;
	for (i = 0; i < 10; i++) {
		table[i].r = 0;
	}

	if (!head) {
		printf("(os) No process...\n");
		return;
	}

	while (head) {
		int i;
		for (i = 1; i < 5; i++) {
			if (head->requested.r[i]) {
				struct term tmp;
				tmp.r = i;
				tmp.name = head->name;
				tmp.num = head->requested.r[i];
				table[cnt++] = tmp;
			}
		}
		head = head->next;
	}
	printf("     Resource\tProcess\t\tcount\n");

	int j;
	for (i = 1; i < 5; i++) {
		for (j = 0; j < 10; j++) {
			if (table[j].r == i) {
				printf("     R%d\t\t%s\t\t%d\n", table[j].r, table[j].name, table[j].num);
			}
		}
	}
}

// created by Yang tao.
void printInformation(char *name) {
	PCB *process = findProcessByName(name);
	printf("     Pid:\t%d\n", process->Pid);
	printf("     Name:\t%s\n", process->name);
	printf("     Type:\t");
	switch (process->type) {
	case Run:
		printf("Run\n");
		break;
	case Ready:
		printf("Ready\n");
		break;
	case Block:
		printf("Block\n");
		break;
	}
}
