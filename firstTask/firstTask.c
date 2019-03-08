// Homework1C.cpp: определяет точку входа для консольного приложения.
//

#define lettersOffset 54
#define ERROR_MSG_WITH_EXIT_CODE_0 {printf("[error]"); return 0;}

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

char *input(int *, int *);

int isInputCorrect(int, int, char *);

unsigned long long translateIntoDecimal(int P, char *S);

char *translateIntoQ(int Q, unsigned long long result, size_t *size);

void output(char *, const size_t *);


int main() {

    int Q = 0, P = 0;
    char *input_string = input(&P, &Q);
    if (input_string == NULL) ERROR_MSG_WITH_EXIT_CODE_0;

    if (!isInputCorrect(P, Q, input_string)) {
        free(input_string);
        ERROR_MSG_WITH_EXIT_CODE_0;
    }

    size_t final_string_size = 0;
    char *final_string = translateIntoQ(Q, translateIntoDecimal(P, input_string), &final_string_size);
    free(input_string);

    if (final_string == NULL) ERROR_MSG_WITH_EXIT_CODE_0;

    output(final_string, &final_string_size);
    free(final_string);
    return 0;
}

char *input(int *P, int *Q) {
    char buf[21] = {'\0'};
    scanf("%d%d%s", P, Q, buf);
    if (P == 0 || Q == 0 || buf[0] == '\0') return NULL;
    char *S = (char *) malloc(strlen(buf) + 1);
    if (S == NULL) return NULL;
    strcpy(S, buf);
    return S;
}

void output(char *fin_s, const size_t *size) {
    for (ptrdiff_t i = *size - 1; i >= 0; i--) {
        printf("%c", fin_s[i]);
    }
};

int isInputCorrect(int P, int Q, char *S) {
    int sym = P + lettersOffset; // max available symbol with actually P
    if (2 <= Q && Q < P && P <= 36) { // check tasks' conditions
        for (ptrdiff_t i = 0; i < strlen(S); ++i) {
            if (S[i] > 96 && S[i] < 123) S[i] -= 32; // transform uppercase letters to lowercase
            if ((S[i] > sym || S[i] < '0' ||
                 (S[i] > 57 && S[i] < 65))) // check if symbol not a number and not a lower case letter
                return 0;
        }
    } else {
        return 0;
    }
    return 1;
}


unsigned long long translateIntoDecimal(int P, char *S) {
    unsigned long long result = 0, current_power = 1;
    for (ptrdiff_t i = strlen(S) - 1; i >= 0; --i) {
        if (S[i] > 57) { // if letter
            result += (S[i] - lettersOffset - 1) * current_power;
        } else { // if digit
            result += (S[i] - '0') * current_power;
        }
        current_power *= P;
    }
    return result;
}

char *translateIntoQ(int Q, unsigned long long result, size_t *size) {
    char *s = (char *) malloc(sizeof(char));
    if (s == NULL) return NULL;
    unsigned long long temp; // i don't like long arithmetic, so lets spend memory
    size_t i = 0;
    char current;
    while (result > 0) {
        temp = result % Q;
        if (temp > 9) current = (char) (temp + lettersOffset);
        else current = (char) (temp + '0');
        s[i++] = current;
        s = (char *) realloc(s, i + 1);
        result = result / Q;
    }
    if (size == NULL) {
        free(s);
        return NULL;
    }
    *size = i;
    return s;
}