#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_STR_NUM 18 + 1
#define PERMISSION 0644 /* rw-r--r-- */
#define OFFSET_ZERO 0

int main(void) {
    int fd;
    int ret = 0;
    size_t buf_size;
    char w_buf[MAX_STR_NUM] = "Hello Driver World";
    char r_buf[MAX_STR_NUM];

    memset(r_buf, 0, MAX_STR_NUM);

    fd = open("test.txt", O_CREAT | O_RDWR, PERMISSION);
    if (fd == -1) {
        printf("ERR file not opened. errno[%d]\n", errno);
        return 0;
    }

    buf_size = strnlen(w_buf, MAX_STR_NUM);
    ret = write(fd, w_buf, buf_size);
    if (ret == -1) {
        printf("ERR write failed. errno[%d]\n", errno);
        goto end;
    }

    printf("write success\n");

    ret = lseek(fd, OFFSET_ZERO, SEEK_SET);
    if (ret == -1) {
        printf("ERR lseek failed. errno[%d]\n", errno);
        goto end;
    }

    ret = read(fd, r_buf, MAX_STR_NUM);
    if (ret == -1) {
        printf("ERR read failed. errno[%d]\n", errno);
        goto end;
    }
    r_buf[MAX_STR_NUM] = '\0';

    printf("read result: %s\n", r_buf);

end:
    close(fd);
    return 0;
}
