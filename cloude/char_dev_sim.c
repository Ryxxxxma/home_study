#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MAX_STR_NUM 20
#define PERMISSION 0644 /* rw-r--r-- */

int main(void) {
    int fd;
    ssize_t ret = 0;
    size_t buf_size;
    char w_buf[MAX_STR_NUM];
    char r_buf[MAX_STR_NUM];

    memset(w_buf, 0, MAX_STR_NUM);
    memset(r_buf, 0, MAX_STR_NUM);

    fd = open("test.txt", O_CREAT | O_RDWR | O_TRUNC, PERMISSION);
    if (fd == -1) {
        printf("ERR file not opened. errno[%d]\n", errno);
        return 0;
    }

    printf("入力文字列を入力してください: ");
    if (fgets(w_buf, MAX_STR_NUM - 1, stdin) == NULL) {
        printf("ERR fgets failed.\n");
        goto end;
    }

    buf_size = strnlen(w_buf, MAX_STR_NUM);
    ret = write(fd, w_buf, buf_size);
    if (ret == -1) {
        printf("ERR write failed. errno[%d]\n", errno);
        goto end;
    }

    printf("[write] %ld bytes written\n", ret);

    /* ファイルポインタを先頭に戻す */
    ret = lseek(fd, 0, SEEK_SET);
    if (ret == -1) {
        printf("ERR lseek failed. errno[%d]\n", errno);
        goto end;
    }

    ret = read(fd, r_buf, MAX_STR_NUM - 1);
    if (ret == -1) {
        printf("ERR read failed. errno[%d]\n", errno);
        goto end;
    }
    r_buf[ret] = '\0';

    printf("[read] %ld bytes read\n", ret);
    printf("読み出し結果: %s\n", r_buf);

end:
    close(fd);
    return 0;
}
