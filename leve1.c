#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MAX_STR_NUM 18 + 1
#define PERMISSION 0644 /* rw-r--r-- */

int main(void) {
    int fd;
    int ret = 0;
    size_t buf_size;
    char buf[MAX_STR_NUM] = "Hello Driver World";
    
    fd = open("test.txt", O_CREAT | O_WRONLY, PERMISSION);
    if (fd == -1) {
        printf("ERR file not opened. errno[%d]\n", errno);
        return 1;
    }

    buf_size = strnlen(buf, MAX_STR_NUM);
    ret = write(fd, buf, buf_size);
    if (ret == -1) {
        printf("ERR write failed. errno[%d]\n", errno);
        return 1;
    }

    printf("write success\n");
    close(fd);
    return 0;
}
