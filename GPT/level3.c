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

    for (i = 0; i != size; i++) {
        if (rb->head == rb->tail) {
            printf("ring buffer is full\n");
            return -1;
        }
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head++) % BUFF_SIZE; /* headを1つ進め、末尾なら先頭に戻す（リング状にインクリメント） */
    }

    return i;
}

int rb_read(struct ring_buffer *rb, char *buf, int size) {
    int i = 0;

    for (i = 0; i != size; i++) {
        if (rb->tail == rb->head) {
            printf("ring buffer is empty\n");
            return -1;
        }
        buf[i] = rb->buffer[rb->tail];
        rb->tail = (rb->tail++) % BUFF_SIZE; /* tailを1つ進め、末尾なら先頭に戻す（リング状にインクリメント） */
    }

    buf[i] = '\0';
    return i;
}
