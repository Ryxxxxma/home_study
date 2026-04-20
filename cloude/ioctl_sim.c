#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DEV_MODE_NAME_SIZE 16

typedef struct {
    int baud_rate;
    int timeout_ms;
    char mode[DEV_MODE_NAME_SIZE];
} dev_config_t;

#define PERMISSION 0644 /* rw-r--r-- */

#define DEV_IOC_MAGIC 'D'
#define DEV_IOC_SET _IOW(DEV_IOC_MAGIC, 1, dev_config_t)
#define DEV_IOC_GET _IOR(DEV_IOC_MAGIC, 2, dev_config_t)

#define BOUND_115200 115200
#define TIMEOUT_500 500
#define MODE_UART "uart"

int main(void) {
    int fd;
    ssize_t ret = 0;
    off_t ret_lseek = 0;
    char *config_match = "OK";
    dev_config_t w_config;
    dev_config_t r_config;

    memset(&w_config, 0, sizeof(w_config));
    memset(&r_config, 0, sizeof(r_config));

    fd = open("test.txt", O_CREAT | O_RDWR | O_TRUNC, PERMISSION);
    if (fd == -1) {
        printf("ERR file not opened. errno[%d]\n", errno);
        return 0;
    }

    printf("DEV_IOC_SET command = 0x%08lX\n", DEV_IOC_SET);
    printf("DEV_IOC_GET command = 0x%08lX\n", DEV_IOC_GET);

    w_config = (dev_config_t){BOUND_115200, TIMEOUT_500, MODE_UART};

    /* ioctl(fd, DEV_IOC_SET, &w_config) */
    ret = write(fd, &w_config, sizeof(dev_config_t));
    if (ret == -1) {
        printf("ERR write failed. errno[%d]\n", errno);
        goto end;
    }
    printf("[SET] baud_rate=%d timeout_ms=%d mode=%s\n", w_config.baud_rate, w_config.timeout_ms, w_config.mode);

    /* ioctl(fd, DEV_IOC_GET, &r_config) */
    ret_lseek = lseek(fd, 0, SEEK_SET); /* ファイルポインタを先頭に戻す */
    if (ret_lseek == -1) {
        printf("ERR lseek failed. errno[%d]\n", errno);
        goto end;
    }

    ret = read(fd, &r_config, sizeof(w_config));
    if (ret == -1) {
        printf("ERR read failed. errno[%d]\n", errno);
        goto end;
    }
    printf("[SET] baud_rate=%d timeout_ms=%d mode=%s\n", r_config.baud_rate, r_config.timeout_ms, r_config.mode);

    /* DEV_IOC_SETで設定した内容をDEV_IOC_GETで取得しているか確認 */
    if ((memcmp(&w_config, &r_config, sizeof(w_config)) != 0)) {
        config_match = "NG";
    }
    printf("[verify] config match: %s\n", config_match);

end:
    close(fd);
    return 0;
}
