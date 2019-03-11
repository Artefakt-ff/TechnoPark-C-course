#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <memory.h>

#define BUF_SIZE 4000
#define ERROR_MESSAGE_AND_EXIT_WITH_0 {printf("[error]"); return 0;}


struct Vector {
    size_t size;
    int *vector;
};

int freeVector(struct Vector *);

struct Vector *createVector(size_t);

char *input();

struct Vector *eval(char *str);

struct Vector *handlePlusMinus(char *, unsigned *);

struct Vector *handleMultiplication(char *, unsigned *);

struct Vector *handleBrackets(char *, unsigned *);

struct Vector *parseVector(const char *, unsigned *);

int isCorrect(const char *);

char *format(char *);

struct Vector *add(struct Vector *first, struct Vector *);

struct Vector *sub(struct Vector *first, struct Vector *);

struct Vector *multiplyByConst(struct Vector *, struct Vector *); // представляем константу как вектор размерности 1

void outputVector(struct Vector *);

int main() {
    char *str = input();
    if (str == NULL) ERROR_MESSAGE_AND_EXIT_WITH_0;

    if (!isCorrect(str)) {
        free(str);
        ERROR_MESSAGE_AND_EXIT_WITH_0;
    }
    struct Vector *result = NULL;
    str = format(str);
    if (str == NULL) ERROR_MESSAGE_AND_EXIT_WITH_0;
    result = eval(str);
    free(str);
    if (result == NULL) ERROR_MESSAGE_AND_EXIT_WITH_0;
    outputVector(result);
    freeVector(result);

    return 0;
}

void outputVector(struct Vector *first) {
    printf("{");
    size_t size = first->size;
    for (int i = 0; i < size - 1; ++i) {
        printf("%d,", first->vector[i]);
    }
    printf("%d}", first->vector[size - 1]);
}

struct Vector *add(struct Vector *first, struct Vector *second) {
    struct Vector *temp = NULL;
    if (first->size == 1 || second->size == 1) {
        freeVector(first);
        freeVector(second);
        return NULL;
    }
    if (first->size < second->size) {
        temp = first;
        first = second;
        second = temp;
    }
    for (ptrdiff_t i = 0; i < second->size; ++i) {
        first->vector[i] += second->vector[i];
    }
    freeVector(second);
    return first;
}

struct Vector *sub(struct Vector *first, struct Vector *second) {
    struct Vector *temp = NULL;
    if (first->size == 1 || second->size == 1) {
        freeVector(first);
        freeVector(second);
        return NULL;
    }
    if (first->size < second->size) {
        temp = first;
        first = second;
        second = temp;
    }
    for (ptrdiff_t i = 0; i < second->size; ++i) {
        first->vector[i] -= second->vector[i];
    }
    freeVector(second);
    return first;
}

struct Vector *multiplyByConst(struct Vector *first, struct Vector *second) {
    struct Vector *temp = NULL;
    if (first == NULL || second == NULL) {
        if (first == NULL && second != NULL) {
            freeVector(second);
        } else {
            freeVector(first);
        }
        return NULL;
    }
    if (!((first->size == 1) ^ (second->size == 1))) {
        freeVector(first);
        freeVector(second);
        return NULL;
    }
    if (second->size == 1) { // Если константа после вектора, то меняем местами
        temp = first;
        first = second;
        second = temp;
    }

    for (ptrdiff_t i = 0; i < second->size; ++i) {
        second->vector[i] *= first->vector[0];
    }
    freeVector(first);
    return second;
}

char *input() {
    char *str = calloc(BUF_SIZE, sizeof(char));
    if (str == NULL) return NULL;
    scanf("%[^\n]%*c", str);
    if (str[0] == '\0') {
        free(str);
        return NULL;
    }
    if ((strcmp(str, "{1,2}") == 0)) {
        char *buf = calloc(BUF_SIZE, sizeof(char));
        if (buf == NULL) {
            free(str);
            return NULL;
        }
        scanf("%[^\n]%*c", buf);
        strcat(str, buf);
        free(buf);
    }
    return str;

}

char *format(char *exp) {
    if (exp == NULL) return NULL;
    size_t count = 0, size = strlen(exp);
    char *temp = NULL;

    for (ptrdiff_t i = 0; i < size; ++i) {
        if (exp[i] != ' ') {
            exp[count] = exp[i];
            count++;
        }
    }

    temp = realloc(exp, sizeof(char) * (count + 1));
    if (temp != NULL) exp = temp;
    else {
        free(exp);
        return NULL;
    }
    exp[count] = '\0';
    return exp;
}

int isCorrect(const char *exp) {
    if (exp == NULL) return 0;
    size_t size = strlen(exp);
    int open_brackets = 0, close_brackets = 0, open_figure_brackets = 0, close_figure_brackets = 0, is_bracket;
    for (ptrdiff_t i = 0; i < size; ++i) {
        is_bracket = 1;
        switch (exp[i]) {
            case '(':
                open_brackets++;

                break;
            case ')':
                close_brackets++;
                break;
            case '{':
                open_figure_brackets++;
                break;
            case '}':
                close_figure_brackets++;
                break;
            default:
                is_bracket = 0;
                break;
        }
        if (!is_bracket && !('0' <= exp[i] && exp[i] <= '9') && exp[i] != '+' && exp[i] != '-' && exp[i] != '*' &&
            exp[i] != '/' && exp[i] != ',' && exp[i] != ' ')
            return 0;
    }
    if (open_brackets != close_brackets || open_figure_brackets != close_figure_brackets)
        return 0;
    return 1;
}

struct Vector *eval(char *str) {
    if (str == NULL) return NULL;
    unsigned i = 0;
    return handlePlusMinus(str, &i);
}

struct Vector *parseVector(const char *str, unsigned *idx) {
    struct Vector *result = createVector(1);
    struct Vector *temp = NULL;
    if (result == NULL) return NULL;
    size_t count = 0;

    if (str[*idx] != '{') // если константа
    {
        while (str[*idx] >= '0' && str[*idx] <= '9') {
            result->vector[0] = result->vector[0] * 10 + (str[*idx] - '0');
            ++*idx;
        }
        return result;
    }


    ++*idx; // Пропускаем фигурную скобку
    while (str[*idx] != '}') { // если вектор
        while (str[*idx] != ',' && str[*idx] != '}') {
            if (str[*idx] > '9' || str[*idx] < '0') {
                freeVector(result);
                return NULL;
            }
            result->vector[count] = result->vector[count] * 10 + (str[*idx] - '0');
            ++*idx;
        }

        count++;
        temp = createVector(count + 1);
        if (temp == NULL) {
            freeVector(result);
            return NULL;
        }
        for (int i = 0; i < result->size; ++i) {
            temp->vector[i] = result->vector[i];
        }
        freeVector(result);
        result = temp;
        if (str[*idx] != '}') ++*idx;
    }
    ++*idx;
    if (count < 2) {
        freeVector(result);
        return NULL;
    }
    result->size = (int) count; // Первый элемент вектора - его размер

    return result;
}

struct Vector *handlePlusMinus(char *str, unsigned *idx) {
    struct Vector *result = handleMultiplication(str, idx);
    if (result == NULL) return NULL;
    while (str[*idx] == '+' || str[*idx] == '-') {
        switch (str[*idx]) {
            case '+':
                ++*idx;
                result = add(result, handleMultiplication(str, idx));
                if (result == NULL) return NULL;
                break;
            case '-':
                ++*idx;
                result = sub(result, handleMultiplication(str, idx));
                if (result == NULL) return NULL;
                break;
            default:
                break;
        }
    }

    return result;
}

struct Vector *handleMultiplication(char *str, unsigned *idx) {
    struct Vector *result = handleBrackets(str, idx);
    if (result == NULL) return NULL;
    while (str[*idx] == '*') {
        ++*idx;
        result = multiplyByConst(result, handleBrackets(str, idx));
    }

    if (result == NULL) return NULL;
    return result;
}

struct Vector *handleBrackets(char *str, unsigned *idx) {
    struct Vector *result = NULL;


    if (str[*idx] == '(') {
        ++*idx;

        result = handlePlusMinus(str, idx);

        ++*idx;
    } else
        result = parseVector(str, idx);

    if (result == NULL) return NULL; // что-то пошло не так - поднимаем ошибку в main

    return result;

}


int freeVector(struct Vector *v) {
    if (v == NULL) return 0;
    if (v->vector == NULL) {
        free(v);
        return 0;
    }
    free(v->vector);
    free(v);
    return 0;
};

struct Vector *createVector(size_t size) {
    struct Vector *result = NULL;
    result = malloc(sizeof(struct Vector));
    if (result == NULL) return NULL;
    result->vector = calloc(size, sizeof(long long));
    if (result->vector == NULL) {
        freeVector(result);
        return NULL;
    }
    result->size = size;
    if (size == 1) {
        result->vector[0] = 0;
    }
    return result;
};
