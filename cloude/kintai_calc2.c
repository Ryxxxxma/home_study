#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define EMPLOYEE_COUNT 7

typedef struct {
    char *name; /* 氏名 */
    int  work_days;  /* 出勤日数 */
    int  late_count; /* 遅刻回数 */
} Employee;

Employee *create_employee_list(int size);
int set_employee(Employee *e, const char *name, int work_days, int late_count);
void free_employee_list(Employee *list, int n);
double calc_late_rate(const Employee *e);
int print_report(const Employee list[], int n);
int find_worst(const Employee list[], int n, double *worst_rate);

int main(void) {

    int ret = 0;
    int i = 0;
    int worst_attendance_employee = 0;
    double worst_late_rate = 0.0;

    /* listに登録する社員のデータ */
    const char *names[EMPLOYEE_COUNT] = {"田中", "佐藤", "鈴木", "斎藤", "小川", "山田", "高橋"};
    int work_days[EMPLOYEE_COUNT] = {20, 20, 20, 20, 20, 20, 20};
    int late_counts[EMPLOYEE_COUNT] = {3, 4, 5, 6, 7, 0, 2};

    Employee *list = NULL;

    list = create_employee_list(EMPLOYEE_COUNT);
    if (list == NULL) {
        printf("[ERROR] [%s:%d] create_employee_list() failed\n", __func__, __LINE__);
        return 1;
    }

    for (i = 0; i < EMPLOYEE_COUNT; i++) {
        ret = set_employee(&list[i], names[i], work_days[i], late_counts[i]);
        if (ret < 0) {
            printf("[ERROR] [%s:%d] set_employee() failed on index %d\n", __func__, __LINE__, i);
            goto end;
        }
    }

    ret = print_report(list, EMPLOYEE_COUNT);
    if (ret < 0) {
        printf("[ERROR] [%s:%d] print_report() failed\n", __func__, __LINE__);
        goto end;
    }

    worst_attendance_employee = find_worst(list, EMPLOYEE_COUNT, &worst_late_rate);
    if (worst_attendance_employee < 0) {
        printf("[ERROR] [%s:%d] find_worst() failed\n", __func__, __LINE__);
        goto end;
    }

    printf("要注意社員: %s (遅刻率 %.1f%%)\n", list[worst_attendance_employee].name, worst_late_rate);

    free_employee_list(list, EMPLOYEE_COUNT);

    return 0;

end:
    free_employee_list(list, EMPLOYEE_COUNT);

    return 1;
}

Employee *create_employee_list(int size) {
    int i = 0;
    Employee *list = NULL;

    list = (Employee *)malloc(size * sizeof(Employee));
    if (list == NULL) {
        printf("[ERROR] [%s:%d] malloc() failed\n", __func__, __LINE__);
        return list;
    }

    for (i = 0; i < size; i++) {
        list[i].name = NULL; /* set_employeeでの再設定判定に備え、未初期化状態を排除 */
    }

    return list;
}

int set_employee(Employee *e, const char *name, int work_days, int late_count) {
    int ret = -1;
    size_t malloc_size = 0;

    if (e == NULL) {
        printf("[ERROR] [%s:%d] argument \"e\" is NULL\n", __func__, __LINE__);
        return ret;
    }
    if (name == NULL) {
        printf("[ERROR] [%s:%d] argument \"name\" is NULL\n", __func__, __LINE__);
        return ret;
    }
    if (name[0] == '\0') {
        printf("[ERROR] [%s:%d] argument \"name\" is empty string\n", __func__, __LINE__);
        return ret;
    }
    if (work_days <= 0) {
        printf("[ERROR] [%s:%d] argument \"work_days\" is %d\n", __func__, __LINE__, work_days);
        return ret;
    }
    if (late_count < 0) {
        printf("[ERROR] [%s:%d] argument \"late_count\" is %d\n", __func__, __LINE__, late_count);
        return ret;
    }
    if (e->name != NULL) {
        free(e->name); /* 既に何か割り当てられている場合は一度 free する（メモリリーク防止) */
        e->name = NULL;
    }

    malloc_size = strlen(name) + 1; /* nameの文字数 + ヌル文字のサイズを計算 */
    e->name = (char *)malloc(malloc_size);
    if (e->name == NULL) {
        printf("[ERROR] [%s:%d] malloc() failed\n", __func__, __LINE__);
        return ret;
    }

    ret = snprintf(e->name, malloc_size, "%s", name);
    if (ret < 0) {
        printf("[ERROR] [%s:%d] snprintf() failed\n", __func__, __LINE__);
        return ret;
    }
    e->work_days = work_days;
    e->late_count = late_count;

    ret = 0;
    return ret;
}

void free_employee_list(Employee *list, int n) {
    int i = 0;

    for (i = 0; i < n; i++) {
        if (list[i].name != NULL) {
            free(list[i].name);
            list[i].name = NULL;
        }
    }
    free(list);

    return;
}

double calc_late_rate(const Employee *e) {

    double late_rate = -1.0;

    if (e == NULL) {
        printf("[ERROR] [%s:%d] argument \"e\" is NULL\n", __func__, __LINE__);
        return late_rate;
    }
    if (e->work_days <= 0) {
        printf("[ERROR] [%s:%d] employee:%s has %d days of attendance\n", __func__, __LINE__, e->name, e->work_days);
        return late_rate;
    }

    late_rate = (double)e->late_count / (double)e->work_days;

    return late_rate;
}

int print_report(const Employee list[], int n) {

    int ret = -1;
    int i = 0;
    double late_rate = 0.0;

    if (list == NULL) {
        printf("[ERROR] [%s:%d] argument \"list\" is NULL\n", __func__, __LINE__);
        return ret;
    }

    printf("==== 勤怠レポート ====\n");

    for (i = 0; i < n; i++) {
        late_rate = calc_late_rate(&list[i]);
        if (late_rate < 0.0) {
            printf("[ERROR] [%s:%d] calc_late_rate() failed\n", __func__, __LINE__);
            return ret;
        }
        late_rate *= 100;  /* 割合をパーセント表示にするため 100 を掛ける */

        printf("%s     出勤:%d日  遅刻:%d回  遅刻率:%.1f%%\n", list[i].name, list[i].work_days, list[i].late_count, late_rate);
    }

    ret = 0;
    return ret;
}

int find_worst(const Employee list[], int n, double *worst_rate) {
    int i = 0;
    int worst_employee = -1;
    double late_rate = 0.0;

    *worst_rate = -1.0; /* 遅刻率は0.0以上のため、初回で必ず更新される */

    if (list == NULL) {
        printf("[ERROR] [%s:%d] argument \"list\" is NULL\n", __func__, __LINE__);
        return worst_employee;
    }

    for (i = 0; i < n; i++) {
        late_rate = calc_late_rate(&list[i]);
        if (late_rate < 0.0) {
            printf("[ERROR] [%s:%d] calc_late_rate() failed\n", __func__, __LINE__);
            return -1;
        }
        if (*worst_rate < late_rate) { /* 遅刻率が同率の場合、インデックスが最小の社員を返す */
            worst_employee = i;
            *worst_rate = late_rate;
        }
    }
    *worst_rate *= 100; /* 割合をパーセント表示にするため 100 を掛ける */

    return worst_employee;
}