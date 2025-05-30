# include <stdio.h>
#include <_printf.h>

// 계산기 프로그램

int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int divide(int a, int b) {
    return a / b;
}
int main() {
    int num1, num2;
    char op;

    while (1) {
        char line[100];
        printf("계산식을 넣으세요 (e.g., 5 + 3) (q to quit): ");
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        // 종료 명령 감지
        if (line[0] == 'q' && (line[1] == '\n' || line[1] == '\0')) {
            break;
        }
        if (sscanf(line, "%d %c %d", &num1, &op, &num2) != 3) {
            printf(" : 입력 오류!\n");
            continue;
        }

        switch (op) {
            case '+':
                printf(" : %d\n", add(num1, num2));
                break;
            case '-':
                printf(" : %d\n", subtract(num1, num2));
                break;
            case '*':
                printf(" : %d\n", multiply(num1, num2));
                break;
            case '/':
                if (num2 != 0) {
                    printf(" : %d\n", divide(num1, num2));
                } else {
                    printf(" : 0 !\n");
                }
                break;
            default:
                printf(" : !\n");
                break;
        }
    }

    return 0;
}
