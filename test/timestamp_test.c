#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../include/duckwave.h"


// para rodar o teste clang -o test timestamp_test.c ../src/duckwave.c -lm
 

void test_generate_timestamp() {
    char result[128];  // Buffer suficiente para armazenar a saída

    // Teste 1: Apenas segundos
    generate_timestamp(59, result);
    assert(strcmp(result, "00:59") == 0);

    // Teste 2: Minutos e segundos
    generate_timestamp(90, result);
    assert(strcmp(result, "01:30") == 0);

    // Teste 3: Horas, minutos e segundos
    generate_timestamp(3661, result);
    assert(strcmp(result, "1:01:01") == 0);

    // Teste 4: Entrada zero
    generate_timestamp(0, result);
    assert(strcmp(result, "00:00") == 0);

    // Teste 5: Uma hora completa
    generate_timestamp(3600, result);
    assert(strcmp(result, "1:00:00") == 0);

    // Teste 6: Valores maiores com mistura de horas, minutos e segundos
    generate_timestamp(5436, result);
    assert(strcmp(result, "1:30:36") == 0);

    // Teste 7: Valores grandes
    generate_timestamp(86399, result); // 23 horas, 59 minutos e 59 segundos
    assert(strcmp(result, "23:59:59") == 0);

    printf("Todos os testes passaram com sucesso!\n");
}

int main() {
    test_generate_timestamp();
    return 0;
}