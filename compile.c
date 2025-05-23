#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

const char *home = NULL;

const char* file_type(const char *a){
	if(a == NULL || a[0] == 0)
		return "";
	int i;
	for(i = strlen(a) - 1 ; i >= 0 ; i--)
		if(a[i] == '.')
			break;
	if(i == -1 || i == (int)strlen(a) - 1)
		return "";
	return a + i + 1;
}

char* file_name(const char *a){
	if(a == NULL || a[0] == 0)
		return NULL;
	int i;
	char *temp;
	for(i = strlen(a) - 1 ; i >= 0 ; i--)
		if(a[i] == '.')
			break;
	if(i == -1 || i == 0){
		temp = malloc(1);
		if(temp == NULL)
			return NULL;
		strcpy(temp, "");
		return temp;
	}
	temp = (char*)malloc(i + 1);
	if(temp == NULL)
		return NULL;
	strncpy(temp, a, i);
	temp[i] = 0;
	return temp;
} // Need to free()

char isDir(const char *a){
	struct stat st;
	if(stat(a, &st))
		return 0;
	if(S_ISDIR(st.st_mode))
		return 1;
	return 0;
}

char makefile(const char* type){
	if(home == NULL)
		return 0;
	char a, b;
	char base[strlen(home) + 50];
	snprintf(base, strlen(home) + 31, "%s/.zonda.ide/makefiles/makefile", home);
	int len = strlen(base) + strlen(type);
	char makefile_normal[len + 1];
	char makefile_project[len + 5];
	snprintf(makefile_normal, len + 1, "%s%s", base, type);
	snprintf(makefile_project, len + 5, "%s%s-prj", base, type);
	a = (access(makefile_normal, F_OK) == 0);
	b = (access(makefile_project, F_OK) == 0);
	return a + 2 * b;
}

char make(const char* type, const char* name){
	int n = (name == NULL) ? 0 : strlen(name);
	char base[] = "make -f ~/.zonda.ide/makefiles/makefile";
	char cmd[strlen(base) + strlen(type) + n + 2];
	if(n == 0)
		snprintf(cmd, strlen(base) + strlen(type) + 1, "%s%s", base, type);
	else
		snprintf(cmd, strlen(base) + strlen(type) + n + 2, "%s%s %s", base, type, name);
	return (system(cmd) == 0);
}

int main(int argc, char* argv[]){
	home = getenv("HOME");
	if(home == NULL){
		printf("Environment Variable \"HOME\" haven't been defined!\n");
		exit(1);
	}
	char ins;
	int i;
	if(argc == 3 && argv[1][0] == '-' && !strcmp(argv[2], "clean")){
		ins = makefile(argv[1]);
		char temp[strlen(argv[1]) + 5];
		snprintf(temp, strlen(argv[1]) + 5, "%s-prj", argv[1]);
		switch(ins){
		case 0:
			printf("~/.zonda.ide/makefiles/makefile%s doesn't exist...\n", argv[1]);
			printf("~/.zonda.ide/makefiles/makefile%s-prj doesn't exist...\n", argv[1]);
			return 1;
		case 1:
			make(argv[1], "clean");
			break;
		case 2:
			make(temp, "clean");
			break;
		case 3:
			make(argv[1], "clean");
			make(temp, "clean");
			break;
		}
	}
	else if(argc == 2 && argv[1][0] == '-'){
		if(strlen(argv[1]) > 4 && strncmp(argv[1] + strlen(argv[1]) - 4, "-prj", 4) == 0){
			printf("Invalid form...\n");
			return 1;
		}
		ins = makefile(argv[1]);
		char temp[strlen(argv[1]) + 5];
		snprintf(temp, strlen(argv[1]) + 5, "%s-prj", argv[1]);
		switch(ins){
		case 0:
			printf("~/.zonda.ide/makefiles/makefile%s doesn't exist...\n", argv[1]);
			printf("~/.zonda.ide/makefiles/makefile%s-prj doesn't exist...\n", argv[1]);
			return 1;
		case 1:
			make(argv[1], NULL);
			break;
		case 2:
			make(temp, NULL);
			break;
		case 3:
			make(argv[1], NULL);
			make(temp, NULL);
			break;
		}
		return 0;
	}
	else if(argc > 1 && argv[1][0] != '-'){
		char *type, *type_prj, *name, ins;
		for(i = 1 ; i < argc ; i++){
			name = file_name(argv[i]);
			if(strcmp(name, "") == 0){
				printf("Unable to compile %s...\n", argv[i]);
				continue;
			}
			if(name == NULL){
				printf("Failed to allocate memory...\n");
				return 1;
			} 
			type = malloc(strlen(file_type(argv[i])) + 2);
			if(type == NULL){
				printf("Failed to allocate memory...\n");
				free(name);
				return 1;
			}
			type_prj = malloc(strlen(file_type(argv[i])) + 6);
			if(type_prj == NULL){
				printf("Failed to allocate memory...\n");
				free(type);
				free(name);
				return 1;
			}
			snprintf(type, strlen(argv[i]) + 2, "-%s", file_type(argv[i]));
			snprintf(type_prj, strlen(argv[i]) + 6, "-%s-prj", file_type(argv[i]));
			ins = makefile(type);
			if(isDir(argv[i]) && ins < 2)
				printf("~/.zonda.ide/makefiles/makefile%s-prj doesn't exist...\n", type);
			else if(isDir(argv[i]))
				make(type_prj, name);
			else if(!(ins & 1))
				printf("~/.zonda.ide/makefiles/makefile%s doesn't exist...\n", type);
			else
				make(type, name);
			free(type);
			free(type_prj);
			free(name);
		}
	}
	else{
		printf("Invalid input!\n");
		return 1;
	}
	return 0;
}
