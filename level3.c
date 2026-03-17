#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define BUFF_SIZE 16

struct ring_buffer {
    char buffer[BUFF_SIZE];
    int head;
    int tail;
};

void rb_init(struct ring_buffer *rb);
int rb_write(struct ring_buffer *rb, const char *data, int size);
int rb_read(struct ring_buffer *rb, char *buf, int size);

int main(void) {
    struct ring_buffer buff;
    size_t w_buff_size = 0;
    int ret = 0;
    const char w_buff[BUFF_SIZE] = "HELLO";
    char r_buff[BUFF_SIZE];

    rb_init(&buff);

    w_buff_size = strnlen(w_buff, BUFF_SIZE);
    ret = rb_write(&buff, w_buff, w_buff_size);
    if (ret < 0) {
       printf("date write failed\n");
       return -1;
    }

    ret = rb_read(&buff, r_buff, w_buff_size);
    if (ret < 0) {
       printf("date read failed\n");
       return -1;
    }

    printf("read result: %s\n", r_buff);

    return 0;
}

void rb_init(struct ring_buffer *rb) {
    memset(rb, 0, sizeof(*rb));
    return;
}

int rb_write(struct ring_buffer *rb, const char *data, int size) {
    int i = 0;

    if (size > BUFF_SIZE) {
        printf("write size over\n");
        return -1;
    }

    for (i = 0; i != size; i++) {
        if (rb->head == BUFF_SIZE) { /* rb->bufferの終端に達した際 */
            if (rb->buffer[0] == '\0') { /*  先頭に値が格納されていない場合は、処理を続行 */
                rb->head = 0;
            }
            else { /*  先頭に値が格納されている場合は、処理を終了 */
                printf("ring buffer is full\n");
                return -1;
            }
        }
        rb->buffer[rb->head] = data[i];
        rb->head++;
    }

    return 0;
}

int rb_read(struct ring_buffer *rb, char *buf, int size) {
    int i = 0;

    if (size > BUFF_SIZE) {
        printf("read size over\n");
        return -1;
    }

    for (i = 0; i != size; i++) {
        if (rb->tail == BUFF_SIZE) { /* rb->bufferの終端に達した際 */
            if (rb->buffer[0] != '\0') { /* 先頭に値が格納されている場合は、処理を続行 */
                rb->tail = 0;
            }
            else { /* 先頭に値が格納されていない場合は、処理を終了 */
                printf("ring buffer is empty\n");
                return -1;
            }
        }
        buf[i] = rb->buffer[rb->tail];
        rb->buffer[rb->tail] = '\0';
        rb->tail++;
    }

    buf[i] = '\0';
    return 0;
}
