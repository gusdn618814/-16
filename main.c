#include "calculator.h"

int main() {
    char sample[] = "3+4*(5+6)*7";
    char postfix[200];

    printf("입력: %s\n\n", sample);

    infixToPostfix(sample, postfix);
    evalPostfix(postfix);

    return 0;
}
