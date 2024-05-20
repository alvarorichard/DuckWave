#include "../test/unity/unity.h"
#include "../src/duckwave.c"  // Inclui o arquivo que contém `generate_timestamp`
#include <stdio.h>

void setUp(void) {
    // Esta função é chamada antes de cada teste
    printf("Setting up...\n");
}

void tearDown(void) {
    // Esta função é chamada após cada teste
    // Vazia no momento
}

void test_generate_timestamp_seconds(void) {
    char result[128];
    generate_timestamp(59, result);
    TEST_ASSERT_EQUAL_STRING("00:59", result);
}

void test_generate_timestamp_minutes_seconds(void) {
    char result[128];
    generate_timestamp(90, result);
    TEST_ASSERT_EQUAL_STRING("01:30", result);
}

void test_generate_timestamp_hours(void) {
    char result[128];
    generate_timestamp(3661, result);
    TEST_ASSERT_EQUAL_STRING("1:01:01", result);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_generate_timestamp_seconds);
    RUN_TEST(test_generate_timestamp_minutes_seconds);
    RUN_TEST(test_generate_timestamp_hours);
    UNITY_END();
    return 0;
}
