#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING_LEN 100
#define INPUT_COUNT_ONE 1
#define INPUT_COUNT_TWO 2

int main(void) {
    int ret = 0;
    int num_a = 0;
    int num_b = 0;
    int num_c = 0;
    int sum = 0;
    char str_s[MAX_STRING_LEN];

    memset(str_s, 0, MAX_STRING_LEN);

    ret = scanf("%d", &num_a);
    if (ret != INPUT_COUNT_ONE) {
        fprintf(stderr, "Invalid input: Please enter a valid number.\n");
        goto end;
    }
    ret = scanf("%d %d", &num_b, &num_c);
    if (ret != INPUT_COUNT_TWO) {
        fprintf(stderr, "Invalid input: Please enter a valid number.\n");
        goto end;
    }
    ret = scanf("%s", str_s);
    if (ret != INPUT_COUNT_ONE) {
        fprintf(stderr, "Invalid input: Please enter a valid string.\n");
        goto end;
    }

    sum = num_a + num_b + num_c;

    printf("%d %s\n", sum, str_s);

    return 0;

end:
    return 1;
}