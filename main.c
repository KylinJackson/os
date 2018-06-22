#include <stdio.h>
#include "vc.h"

int main()
{
	while (1) {
		printf("(os) ");
		char input[20];
		gets(input);
		if (input[0] == '\0') continue;

		char *scanner = NULL;
		scanner = strtok(input, " ");

		if (!strcmp(scanner, "cr")) {
			scanner = strtok(NULL, " ");
			char *processName = (char*)malloc(sizeof(strlen(scanner)) + 1);
			strcpy(processName, scanner);
			processName[strlen(scanner)] = '\0';
			scanner = strtok(NULL, " ");
			create(processName, atoi(scanner));
		}
		else if (!strcmp(scanner, "de")) {
			scanner = strtok(NULL, " ");
			char *processName = (char*)malloc(sizeof(strlen(scanner)) + 1);
			strcpy(processName, scanner);
			processName[strlen(scanner)] = '\0';
			destroy(processName);
		}
		else if (!strcmp(scanner, "init")) {
			init();
		}
		else if (!strcmp(scanner, "req")) {
			char *res = strtok(NULL, " ");
			char *cnt = strtok(NULL, " ");
			request(atoi(++res), atoi(cnt));
		}
		else if (!strcmp(scanner, "rel")) {
			char *res = strtok(NULL, " ");
			char *cnt = strtok(NULL, " ");
			release(atoi(++res), atoi(cnt));
		}
		else if (!strcmp(scanner, "lp")) {
			printProcess();
		}
		else if (!strcmp(scanner, "lr")) {
			printResource();
		}
		else if (!strcmp(scanner, "p")) {
			printInformation(strtok(NULL, " "));
		}
		else if (!strcmp(scanner, "to")) {
			timeOut();
		}
		else {
			printf("Illegal instruct\n");
		}

	}
	return 0;
}