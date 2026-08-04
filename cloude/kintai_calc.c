#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 32
#define EMPLOYEE_COUNT 5

typedef struct {
    char name[MAX_NAME_LEN]; /* 氏名 */
    int  work_days;  /* 出勤日数 */
    int  late_count; /* 遅刻回数 */
} Employee;

double calc_late_rate(const Employee *e); /* 他の変数名と名前衝突を避けるためユニークな関数名に変更 */
int print_report(const Employee list[], int n); /* エラー処理を実装するため、int型に変更 */
int find_worst(const Employee list[], int n, double *worst_rate); /* 第3引数のポインタ経由で最大遅刻率の計算結果を格納する仕様に変更 */

int main(void) {

    int ret = 0;
    int worst_attendance_employee = 0;
    double worst_late_rate = 0.0;

    Employee list[EMPLOYEE_COUNT] = {
        {"田中", 20, 3},
        {"佐藤", 20, 4},
        {"鈴木", 20, 5},
        {"斎藤", 20, 6},
        {"小川", 20, 7}
    };

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

    return 0;

end:
    return 1;
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