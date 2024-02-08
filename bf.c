#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <assert.h>
#include <fcntl.h>

#define	ARRAY_SIZE	1024

#define	eprintf(...)	fprintf(stderr, __VA_ARGS__)
#define eputs(s)	fputs(s, stderr)

static size_t get_file_size_from_fd(int fd)
{
	size_t len;
	int pos;

	/* get current position */
	pos = lseek(fd, 0L, SEEK_SET);
	len = lseek(fd, 0L, SEEK_END);
	/* restore to original position */
	lseek(fd, 0L, pos);

	return len;
}

static const char *find_matching_backward(const char *s, const char *start)
{
	int depth = 0;
	s--;

	do {
		if(*s == '[') {
			if(depth == 0)
				return s;
			else
				depth++;
		} else if(*s == ']')
			depth--;
	} while (s-- >= start);

	return NULL;
}

static const char *find_matching_forward(const char *s)
{
	const char *pos = s + 1;
	int depth = 0;

	do {
		if(*pos == ']') {
			if(depth == 0)
				return pos + 1;
			else
				depth++;
		} else if(*pos == '[')
			depth--;
	} while (*(pos++));

	return NULL;
}

int bf_mainloop(char *file, size_t file_size)
{
	const char *file_ptr;
	char *byte_array;
	char *byte_ptr;
	int ret = EXIT_SUCCESS;

	byte_array = calloc(1, ARRAY_SIZE);
	if(!byte_array) {
		perror("calloc");
		return EXIT_FAILURE;
	}

	byte_ptr = byte_array;
	file_ptr = file;

	while(file_ptr <= (file + file_size)) {
		switch(*file_ptr) {
		case '>':
			if(byte_ptr <= byte_array + ARRAY_SIZE)
				byte_ptr++;
			break;
		case '<':
			if(byte_ptr >= byte_array)
				byte_ptr--;
			break;
		case '+':
			(*byte_ptr)++;
			break;
		case '-':
			(*byte_ptr)--;
			break;
		case '.':
			putchar(*byte_ptr);
			break;
		/* I don't think we need input just yet */
		case ',': {
				int c = getchar();
				if(c == EOF) {
					eputs("Unexpected EOF encountered. Aborting...\n");
					ret = EXIT_FAILURE;
					goto out;
				}
			 	*byte_ptr = c;
			}
			break;
		case '[':
			if((*byte_ptr == 0)) {
				file_ptr = find_matching_forward(file_ptr);
				if(!file_ptr) {
					eputs("No matching ']' for '['. Aborting...\n");
					ret = EXIT_FAILURE;
					goto out;
				}
			}
			break;
		case ']':
			if((*byte_ptr != 0)) {
				file_ptr = find_matching_backward(file_ptr, file);
				if(!file_ptr) {
					eputs("No matching '[' for ']'. Aborting...\n");
					ret = EXIT_FAILURE;
					goto out;
				}
			}
			break;
		/* Brainf*** ignores all other characters... */
		default:
			break;
		}
		file_ptr++;
	}

out:
	free(byte_array);
	return ret;
}

static void usage(int code, char *name)
{
	printf("usage: %s FILE\n", name);
	exit(code);
}

int main(int argc, char **argv)
{
	size_t file_size;
	char *file;
	int fd;

	if(argc != 2)
		usage(EXIT_FAILURE, argv[0]);

	fd = open(argv[1], O_RDONLY);
	if(fd < 0) {
		eprintf("%s: %s: %s\n", argv[0], argv[1], strerror(errno));
		return EXIT_FAILURE;
	}

	file_size = get_file_size_from_fd(fd);

	file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(!file) {
		perror("mmap");
		return EXIT_FAILURE;
	}
	close(fd);

	assert(bf_mainloop(file, file_size) == EXIT_SUCCESS);
	munmap(file, file_size);

	return EXIT_SUCCESS;
}
