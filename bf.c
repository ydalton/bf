#include <stdio.h>
#include <stdlib.h>

#define ARR_LEN 1024
#define FILE_MAX 1024

#define DIE_IF(cond, s)							\
		if(cond) {								\
				perror(s);						\
				return -1;						\
		}										\

int main(int argc, char **argv) {
		char arr[ARR_LEN];
		char c, *ptr, *code;
		unsigned int instr_ptr, loop_num, i, code_len;
		FILE *fp;

		if(argc != 2) {
				printf("usage: %s FILE\n", argv[0]);
				return -1;
		}

		fp = fopen(argv[1], "r");
		DIE_IF(!fp, "fopen");

		code = (char *) malloc(FILE_MAX);
		DIE_IF(!code, "malloc");

		for(i = 0; i < FILE_MAX; i++) {
				c = getc(fp);
				if(c == EOF)
						break;
				code[i] = c;
		}
		fclose(fp);

		code_len = i;
		/* set the pointer to the beginning of the array */
		ptr = arr;
		/* zero out the entire array, without using memset */
		for(i = 0; i < ARR_LEN; i++)
				arr[i] = 0;
		/* start from the beginning */
		instr_ptr = 0;
		/* set loop counter to 0 */
		loop_num = 0;
		/* loop over each character in the loop */
		while(instr_ptr < code_len) {
				c = code[instr_ptr];
				switch(c) {
						/* ignoring the newline or space... */
				case ' ':
				case '\n':
						continue;
				case '+':
						(*ptr)++;
						break;
				case '>':
						ptr++;
						break;
				case '<':
						ptr--;
						break;
				case '-':
						(*ptr)--;
						break;
				case '.':
						putchar(*ptr);
						break;
				case ',': {
						char tmp;
						tmp = getchar();
						*ptr = tmp;
				}
						break;
				case '[':
						loop_num++;
						break;
				case ']':

						if(*ptr != 0) {
								while(1) {
										instr_ptr--;
										if(code[instr_ptr] == '[')
												break;
								}
								break;
						}
						loop_num--;
						break;
				}
#if 0
				/* print where the pointer is */
				printf("%s", code);
				for(i = 0; i < instr_ptr; i++)
						putchar(' ');
				printf("^\n");
#endif
				instr_ptr++;
		}
		free(code);
		if(loop_num != 0) {
				puts("Something has gone wrong");
				return -1;
		}
		return 0;
}
