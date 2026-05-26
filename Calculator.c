#include "calculator.h"
#include "ArrayStack.h"

int precedence(char op) {
    switch (op) {
    case '*':
    case '/':
        return 2;
    case '+':
    case '-':
        return 1;
    case '(':
        return 0;
    default:
        return 0;
    }
}

/* 현재 postfix 버퍼 출력 헬퍼 (버퍼 내 공백 포함이므로 그대로 출력) */
static void printPostfixBuf(char* buf, int len) {
    for (int k = 0; k < len; k++) {
        printf("%c", buf[k]);
    }
}

/* 현재 스택 내용 출력 헬퍼 (CHARACTER 스택) */
static void printCharStack(ArrayStack* s) {
    for (int k = 0; k <= s->top; k++) {
        if (k > 0) printf(" ");
        printf("%c", s->data[k].operator);
    }
}

/* 현재 스택 내용 출력 헬퍼 (INTEGER 스택) */
static void printIntStack(ArrayStack* s) {
    for (int k = 0; k <= s->top; k++) {
        if (k > 0) printf(" ");
        printf("%d", s->data[k].value);
    }
}

/* -------------------------------------------------------
 * infixToPostfix
 * 오류 수정 사항:
 *  1) infix[i] != NULL  → infix[i] != '\0'  (문자 종료 비교)
 *  2) infix[i] >= "0"   → infix[i] >= '0'   (문자열→문자 리터럴)
 *  3) precedence(prevOp) → precedence(prevOp.operator)  (union 필드)
 *  4) postfix[j] = popArrayStack(opStack)
 *       → postfix[j] = popArrayStack(opStack).operator  (타입 불일치)
 *  5) pushArrayStack(opStack, infix[i])
 *       → stackElement로 감싸서 push  (타입 불일치)
 *  6) do-while 조건 emptyArrayStack → !emptyArrayStack  (조건 반전)
 *  7) prevOp != '(' → prevOp.operator != '('  (union 필드)
 *  8) postfix[j] = NULL → postfix[j] = '\0'  (문자 종료)
 * ------------------------------------------------------- */
int infixToPostfix(char* infix, char* postfix) {
    int j = 0;
    /* [수정 1] postfix 버퍼 공간을 후에 공백 구분자 포함해 넉넉히 */
    char buf[200];
    int blen = 0;

    ArrayStack* opStack = createArrayStack(100, CHARACTER);

    printf("Transform:\n");

    for (int i = 0; infix[i] != '\0'; i++) {  /* [수정 1] NULL → '\0' */

        if (infix[i] == ' ') continue;  /* 공백 무시 */

        /* ── 숫자 ── */
        /* [수정 2] "0","9" → '0','9' (문자 리터럴) */
        if (infix[i] >= '0' && infix[i] <= '9') {
            if (blen > 0 && buf[blen - 1] != ' ') buf[blen++] = ' ';
            buf[blen++] = infix[i];

            /* 진행 상태 출력 */
            printf("- Postfix: ");
            printPostfixBuf(buf, blen);
            printf(" | Stack: ");
            printCharStack(opStack);
            printf("\n");
        }
        /* ── 연산자 ── */
        else if (infix[i] == '+' || infix[i] == '-' ||
                 infix[i] == '*' || infix[i] == '/') {

            /* [수정 3] precedence(prevOp) → precedence(prevOp.operator) */
            while (!emptyArrayStack(opStack)) {
                stackElement prevOp = peekArrayStack(opStack);
                if (precedence(infix[i]) <= precedence(prevOp.operator)) {
                    /* [수정 4] .operator 추가 */
                    stackElement popped = popArrayStack(opStack);
                    if (blen > 0 && buf[blen - 1] != ' ') buf[blen++] = ' ';
                    buf[blen++] = popped.operator;
                } else {
                    break;
                }
            }
            /* [수정 5] stackElement로 감싸서 push */
            stackElement elem;
            elem.operator = infix[i];
            pushArrayStack(opStack, elem);

            /* 진행 상태 출력 */
            printf("- Postfix: ");
            printPostfixBuf(buf, blen);
            printf(" | Stack: ");
            printCharStack(opStack);
            printf("\n");
        }
        /* ── '(' ── */
        else if (infix[i] == '(') {
            stackElement elem;
            elem.operator = infix[i];
            pushArrayStack(opStack, elem);

            printf("- Postfix: ");
            printPostfixBuf(buf, blen);
            printf(" | Stack: ");
            printCharStack(opStack);
            printf("\n");
        }
        /* ── ')' ── */
        else if (infix[i] == ')') {
            /* [수정 6] emptyArrayStack → !emptyArrayStack (조건 반전) */
            do {
                stackElement prevOp = popArrayStack(opStack);
                /* [수정 7] prevOp != '(' → prevOp.operator != '(' */
                if (prevOp.operator != '(') {
                    if (blen > 0 && buf[blen - 1] != ' ') buf[blen++] = ' ';
                    buf[blen++] = prevOp.operator;
                }
                if (prevOp.operator == '(') {
                    break;
                }
            } while (!emptyArrayStack(opStack));  /* [수정 6] */

            printf("- Postfix: ");
            printPostfixBuf(buf, blen);
            printf(" | Stack: ");
            printCharStack(opStack);
            printf("\n");
        }
    }

    /* 남은 연산자 모두 꺼내기 */
    while (!emptyArrayStack(opStack)) {
        stackElement popped = popArrayStack(opStack);
        if (blen > 0 && buf[blen - 1] != ' ') buf[blen++] = ' ';
        buf[blen++] = popped.operator;

        printf("- Postfix: ");
        printPostfixBuf(buf, blen);
        printf(" | Stack: ");
        printCharStack(opStack);
        printf("\n");
    }

    /* [수정 8] NULL → '\0' */
    buf[blen] = '\0';

    /* postfix 버퍼에 복사 */
    for (int k = 0; k <= blen; k++) postfix[k] = buf[k];

    printf("\n- Result: %s\n\n", postfix);
    return 0;
}

/* -------------------------------------------------------
 * evalPostfix
 * 오류 수정 사항:
 *  1) postfix[i] != NULL → postfix[i] != '\0'
 *  2) pushArrayStack(valueStack, (int)(...))
 *       → stackElement로 감싸서 push
 *  3) int i1 = popArrayStack(...)
 *       → stackElement로 받아 .value 사용
 *  4) pushArrayStack(valueStack, i1*i2)
 *       → stackElement로 감싸서 push
 *  5) return popArrayStack(valueStack)
 *       → .value 반환
 * ------------------------------------------------------- */
int evalPostfix(char* postfix) {
    ArrayStack* valueStack = createArrayStack(100, INTEGER);
    int i = 0;

    printf("Eval:\n");
    printf("- Postfix: %s\n", postfix);

    while (postfix[i] != '\0') {  /* [수정 1] */

        if (postfix[i] == ' ') { i++; continue; }

        if (postfix[i] >= '0' && postfix[i] <= '9') {
            /* [수정 2] stackElement로 감싸서 push */
            stackElement elem;
            elem.value = (int)(postfix[i] - '0');
            pushArrayStack(valueStack, elem);

            printf("- Current: %c | Stack: ", postfix[i]);
            printIntStack(valueStack);
            printf("\n");
        }
        else if (postfix[i] == '*' || postfix[i] == '/' ||
                 postfix[i] == '+' || postfix[i] == '-') {

            /* [수정 3] stackElement로 pop 후 .value 사용 */
            int i1 = popArrayStack(valueStack).value;
            int i2 = popArrayStack(valueStack).value;
            int result = 0;

            if      (postfix[i] == '*') result = i2 * i1;
            else if (postfix[i] == '/') result = i2 / i1;
            else if (postfix[i] == '+') result = i2 + i1;
            else if (postfix[i] == '-') result = i2 - i1;

            /* [수정 4] stackElement로 감싸서 push */
            stackElement elem;
            elem.value = result;
            pushArrayStack(valueStack, elem);

            printf("- Current: %c | Stack: ", postfix[i]);
            printIntStack(valueStack);
            printf("\n");
        }
        i++;
    }

    /* [수정 5] .value 반환 */
    int finalResult = popArrayStack(valueStack).value;
    printf("\n- Result: %d\n", finalResult);
    return finalResult;
}
