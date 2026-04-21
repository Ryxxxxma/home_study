#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

#define RING_BUF_SIZE 8
#define RING_BUF_MAX_WRITE 16
#define MAX_SLEEP_COUNT 1000 /* 1s = 1ms * 1000 */

typedef struct {
    int             buf[RING_BUF_SIZE]; /* データ本体     */
    int             head;               /* 読み出し位置   */
    int             tail;               /* 書き込み位置   */
    int             count;              /* 現在の格納数   */
    pthread_mutex_t lock;               /* 排他制御       */
} ring_buf_t;

static int ret_ok = 0;
static int ret_err = -1;

void* write_thread_func(void* arg);
void* read_thread_func(void* arg);

int main(void) {
    int mtx_ret = -1;
    int w_ret = -1;
    int r_ret = -1;
    pthread_t w_thread;
    pthread_t r_thread;
    void* r_thread_ret = NULL;
    void* w_thread_ret = NULL;
    ring_buf_t ring_buf;

    memset(&w_thread, 0, sizeof(pthread_t));
    memset(&r_thread, 0, sizeof(pthread_t));
    memset(&ring_buf, 0, sizeof(ring_buf_t));

    mtx_ret = pthread_mutex_init(&ring_buf.lock, NULL);
    if (mtx_ret != 0) {
        printf("ERR mutex init failed. ret[%d]\n", mtx_ret);
        goto end;
    }

    w_ret = pthread_create(&w_thread, NULL, write_thread_func, (void*)&ring_buf);
    if (w_ret != 0) {
        printf("ERR write pthread create failed. ret[%d]\n", w_ret);
        goto end;
    }

    r_ret = pthread_create(&r_thread, NULL, read_thread_func, (void*)&ring_buf);
    if (r_ret != 0) {
        printf("ERR read pthread create failed. ret[%d]\n", r_ret);
        goto end;
    }

end:
    if (w_ret == 0) {
        pthread_join(w_thread, &w_thread_ret);
    }
    if (r_ret == 0) {
        pthread_join(r_thread, &r_thread_ret);
    }

    if (mtx_ret == 0) {
        pthread_mutex_destroy(&ring_buf.lock);
    }

    if ((w_ret == 0) &&
        (r_ret == 0) &&
        (*(int *)w_thread_ret == 0) &&
        (*(int *)r_thread_ret == 0)) {
        printf("[main]   all done.\n");
    }

    return 0;
}

void* write_thread_func(void* arg) {
    int i = 0;
    int sleep_count = 0;
    struct timespec ts;
    ring_buf_t* w_ring_buf = (ring_buf_t*)arg;

    memset(&ts, 0, sizeof(struct timespec));
    ts.tv_nsec = 1000000; /* 1ms = 1000,000ns */

    for (i = 0; i < RING_BUF_MAX_WRITE; i++) {
        sleep_count = 0;

        pthread_mutex_lock(&w_ring_buf->lock);
        while (w_ring_buf->count == RING_BUF_SIZE) { /* bufが満杯なら1ms待機 */
            pthread_mutex_unlock(&w_ring_buf->lock);
            nanosleep(&ts, NULL);
            sleep_count++;
            if (sleep_count == MAX_SLEEP_COUNT) { /* sleep_countがMAX_SLEEP_COUNT超えた場合、処理を終了 */
                printf("ERR write sleep max count over.\n");
                return (void *)&ret_err;
            }
            pthread_mutex_lock(&w_ring_buf->lock);
        }

        w_ring_buf->buf[w_ring_buf->tail] = i;
        printf("[writer] write: %d\n", i);
        w_ring_buf->tail = (w_ring_buf->tail + 1) % RING_BUF_SIZE;  /* tailを1つ進め、末尾なら先頭に戻す（リング状にインクリメント） */
        w_ring_buf->count++;
        pthread_mutex_unlock(&w_ring_buf->lock);
    }

    return (void *)&ret_ok;
}

void* read_thread_func(void* arg) {
    int i = 0;
    int sleep_count = 0;
    int r_num = 0;
    struct timespec ts;
    ring_buf_t* r_ring_buf = (ring_buf_t*)arg;

    memset(&ts, 0, sizeof(struct timespec));
    ts.tv_nsec = 1000000; /* 1ms = 1000,000ns */

    for (i = 0; i < RING_BUF_MAX_WRITE; i++) {
        sleep_count = 0;

        pthread_mutex_lock(&r_ring_buf->lock);
        while (r_ring_buf->count == 0) { /* bufが空なら1ms待機 */
            pthread_mutex_unlock(&r_ring_buf->lock);
            nanosleep(&ts, NULL);
            sleep_count++;
            if (sleep_count == MAX_SLEEP_COUNT) { /* sleep_countがMAX_SLEEP_COUNT超えた場合、処理を終了 */
                printf("ERR read sleep max count over.\n");
                return (void *)&ret_err;
            }
            pthread_mutex_lock(&r_ring_buf->lock);
        }

        r_num = r_ring_buf->buf[r_ring_buf->head];
        printf("[reader] read: %d\n", r_num);
        r_ring_buf->head = (r_ring_buf->head + 1) % RING_BUF_SIZE;  /* headを1つ進め、末尾なら先頭に戻す（リング状にインクリメント） */
        r_ring_buf->count--;
        pthread_mutex_unlock(&r_ring_buf->lock);
    }

    return (void *)&ret_ok;
}