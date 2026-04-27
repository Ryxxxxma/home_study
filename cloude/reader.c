#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <semaphore.h>

#define SHM_NAME        "/dev_shm"
#define SHM_MSG_MAX     64
#define SHM_MSG_NUM     8
#define SHM_PERMISSION  0644 /* rw-r--r-- */

typedef struct {
    char     msg[SHM_MSG_NUM][SHM_MSG_MAX]; /* メッセージバッファ */
    int      count;                         /* 書き込み済み件数   */
    sem_t    sem_written;                   /* 書き込み完了通知   */
    sem_t    sem_read;                      /* 読み出し完了通知   */
} shm_data_t;

int main(void) {
    int i = 0;
    int shm_fd = -1;
    shm_data_t *shm_ptr = MAP_FAILED;

    shm_fd = shm_open(SHM_NAME, O_RDWR, SHM_PERMISSION);
    if (shm_fd == -1) {
        printf("ERR shm open failed. errno[%d]\n", errno);
        goto end;
    }

    /* ftruncate()はwriter側で実施済みのため、reader側では不要 */

    shm_ptr = (shm_data_t *)mmap(NULL, sizeof(shm_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        printf("ERR mmap failed. errno[%d]\n", errno);
        goto end;
    }

    for (i = 0; i < SHM_MSG_NUM; i++) {
        sem_wait(&shm_ptr->sem_written); /* writerがデータを書き込むまで待機 */
        printf("[reader] read: %s\n", shm_ptr->msg[i]);
        sem_post(&shm_ptr->sem_read);
    }

    printf("[reader] all done.\n");

end:
    if (shm_ptr != MAP_FAILED) {
        munmap(shm_ptr, sizeof(shm_data_t));
    }

    if (shm_fd != -1) {
        close(shm_fd);
    }

    return 0;
}