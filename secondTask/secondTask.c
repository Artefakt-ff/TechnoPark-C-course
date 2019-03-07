#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <memory.h>

#define BUF_SIZE 4000 // В задании не указано какой может быть входная строка - берем просто большое число
#define ERROR_MESSAGE_AND_EXIT_WITH_0 {printf("[error]"); return 0;}

char *input();

int *eval(char *str); // точка входа в вычислительную функцию

int *handlePlusMinus(char *, unsigned *); // обработка выражений, если встречен знаки плюс или минус

int *handleMultiplication(char *, unsigned *); // обработка выражений, если встречен знак умножения

int *handleBrackets(char *, unsigned *); // обработка выражений, если встречены круглые скобки

int *parseVector(const char *, unsigned *); // преобразование вектора из текстового вида в массив

int isCorrect(const char *); // проверка на скобки и допустимость символов

char *format(char *); // удаление пробельных символов

int *add(int *first, int *); // сложение векторов

int *sub(int *first, int *); // вычитание векторов

int *multiplyByConst(int *, int *); // представляем константу как вектор размерности 1

void outputVector(int *);

int main() {
    char *str = input();
    if (str == NULL) ERROR_MESSAGE_AND_EXIT_WITH_0;

    if (!isCorrect(str)) {
        free(str);
        ERROR_MESSAGE_AND_EXIT_WITH_0;
    }
    int *result;
    str = format(str);
    if (str == NULL) ERROR_MESSAGE_AND_EXIT_WITH_0;
    result = eval(str);
    free(str);
    if (result == NULL) ERROR_MESSAGE_AND_EXIT_WITH_0;
    outputVector(result);
    free(result);

    return 0;
}


void outputVector(int *first) {
    printf("{");
    int size = first[0] - 1;
    for (int i = 1; i < size; ++i) {
        printf("%d,", first[i]);
    }
    printf("%d}", first[size]);
}


int *add(int *first, int *second) {
    int *temp;
    if (first[0] == 2 || second[0] == 2) {  // если один из элементов - константа
        free(first);
        free(second);
        return NULL;
    }
    if (first[0] < second[0]) { // выбираем вектор с большей размерностью
        temp = first;
        first = second;
        second = temp;
    }
    for (ptrdiff_t i = 1; i < second[0]; ++i) {
        first[i] += second[i];
    }
    free(second);
    return first;
}

int *sub(int *first, int *second) {
    int *temp;
    if (first[0] == 2 || second[0] == 2) { // если один из элементов - константа
        free(first);
        free(second);
        return NULL;
    }
    if (first[0] < second[0]) {  // выбираем вектор с большей размерностью
        temp = first;
        first = second;
        second = temp;
    }
    for (ptrdiff_t i = 1; i < second[0]; ++i) {
        first[i] -= second[i];
    }
    free(second);
    return first;
}

int *multiplyByConst(int *first, int *second) {
    int *temp;
    if (first == NULL || second == NULL) { // проверяем выполнение контракта
        if (first == NULL && second != NULL) {
            free(second);
        } else {
            free(first);
        }
        return NULL;
    }
    if (!((first[0] == 2) ^ (second[0] == 2))) {  // если оба константы или оба вектора возвращает NULL
        free(first);
        free(second);
        return NULL;
    }
    if (second[0] == 2) { // Если константа после вектора, то меняем местами
        temp = first;
        first = second;
        second = temp;
    }

    for (ptrdiff_t i = 1; i < second[0]; ++i) {
        second[i] *= first[1];
    }
    free(first);
    return second;
}

char *input() {
    char *str = calloc(BUF_SIZE, sizeof(char));
    if (str == NULL) return NULL;
    scanf("%[^\n]%*c", str);
    if (str[0] == '\0'){
        free(str);
        return NULL;
    }
    if ((strcmp(str, "{1,2}") == 0)) {
        char *buf = calloc(BUF_SIZE, sizeof(char));
        if (buf == NULL) return NULL;
        scanf("%[^\n]%*c", buf);
        strcat(str, buf);
        free(buf);
    }
    return str;

}

char *format(char *exp) {
    if (exp == NULL) return NULL;
    size_t count = 0, size = strlen(exp);
    char *temp;

    for (ptrdiff_t i = 0; i < size; ++i) { // копируем все непробельные символы
        if (exp[i] != ' ') {
            exp[count] = exp[i];
            count++;
        }
    }

    temp = realloc(exp, sizeof(char) * (count + 1)); // перевыделяем память под получившуюся строку
    if (temp != NULL) exp = temp;
    else return NULL;
    exp[count] = '\0';
    return exp;
}

int isCorrect(const char *exp) {
    if (exp == NULL) return 0;
    size_t size = strlen(exp);
    int open_brackets = 0, close_brackets = 0, open_figure_brackets = 0, close_figure_brackets = 0, is_bracket;
    for (ptrdiff_t i = 0; i < size; ++i) { // считаем скобки
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
        if (!is_bracket && !('0' <= exp[i] && exp[i] <= '9') && exp[i] != '+' && exp[i] != '-' && exp[i] != '*' && // проверяем на корректность введенных символов
            exp[i] != '/' && exp[i] != ',' && exp[i] != ' ')
            return 0;
    }
    if (open_brackets != close_brackets || open_figure_brackets != close_figure_brackets) // проверяем на корректность скобок
        return 0;
    return 1;
}

int *eval(char *str) {
    if (str == NULL) return NULL;
    unsigned i = 0; // курсор по выражению
    return handlePlusMinus(str, &i);
}

int *parseVector(const char *str, unsigned *idx) {
    int *result = malloc(2 * sizeof(long long));
    int *temp;
    if (result == NULL) return NULL;
    result[1] = 0;
    int sign[2] = {2, 1};
    size_t count = 1;
    if (str[*idx] == '-') { // если есть знак минус, то позже умножаем на константу "-1"
        sign[1] = -1;
        ++*idx;
    }

    if (str[*idx] != '{') // если константа
    {

        while (str[*idx] >= '0' && str[*idx] <= '9') {
            result[1] = result[1] * 10 + (str[*idx] - '0');
            ++*idx;
        }
        result[0] = 2;
        return result;
    }


    ++*idx; // Пропускаем фигурную скобку

    while (str[*idx] != '}') { // если вектор
        while (str[*idx] != ',' && str[*idx] != '}') {
            if (str[*idx] > '9' || str[*idx] < '0'){
                free(result);
                return NULL;
            }
            result[count] = result[count] * 10;
            result[count] += (str[*idx] - '0');
            ++*idx;
        }

        count++;
        temp = realloc(result, sizeof(long long) * (count + 1));
        if (temp != NULL) result = temp;
        else return NULL;

        result[count] = 0;
        if (str[*idx] != '}') ++*idx;
    }
    ++*idx;
    result[0] = (int) count; // Первый элемент вектора - его размер
    if (count == 1) { // если вектор размера 1 - возвращаем NULL
        free(result);
        return NULL;
    }
    if (sign[1] == -1) {
        return multiplyByConst(sign, result);
    }
    return result;
}

int *handlePlusMinus(char *str, unsigned *idx) {
    int *result = handleMultiplication(str, idx);
    if (result == NULL) return NULL; // проверяем выполнение контракта
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

int *handleMultiplication(char *str, unsigned *idx) {
    int *result = handleBrackets(str, idx);
    if (result == NULL) return NULL; // проверяем выполнение контракта
    while (str[*idx] == '*') {
        ++*idx;
        result = multiplyByConst(result, handleBrackets(str, idx));
    }

    if (result == NULL) return NULL;
    return result;
}

int *handleBrackets(char *str, unsigned *idx) {
    int *result;
    int sign[2] = {2, 1};

    if (str[*idx] == '-') {
        sign[1] = -1;
        ++*idx;
    }

    if (str[*idx] == '(') {
        ++*idx;

        result = handlePlusMinus(str, idx);

        ++*idx;
    } else
        result = parseVector(str, idx);

    if (result == NULL) return NULL;

    if (sign[1] == -1) return multiplyByConst(sign, result);
    return result;

}

