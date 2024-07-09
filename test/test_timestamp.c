#include <stdio.h>
#include <string.h>

// clang test_timestamp.c -o test_timestamp

/**
 * This function generates a timestamp string in the format hh:mm:ss or mm:ss
 * from a float representing seconds.
 *
 * @param secs The total time in seconds.
 * @param res The resulting formatted timestamp string. It should be large
 * enough to hold the resulting string (128 bytes is assumed here).
 */
void generate_timestamp(float secs, char *res)
{
  int hours = (int)secs / 3600;
  secs = (int)secs % 3600;
  int mins = (int)secs / 60;
  secs = (int)secs % 60;

  if (hours > 0)
    snprintf(res, 128, "%d:%02d:%02.0f", hours, mins, secs); // Format hh:mm:ss
  else
    snprintf(res, 128, "%02d:%02.0f", mins, secs); // Format mm:ss
}

/**
 * Tests the generate_timestamp function with given input and expected output.
 *
 * @param input_secs The input time in seconds.
 * @param expected_output The expected formatted timestamp string.
 */
void test_generate_timestamp(float input_secs, const char *expected_output)
{
  char result[128];
  generate_timestamp(input_secs, result);
  if (strcmp(result, expected_output) == 0) {
    printf("Test passed for input %.2f seconds. Output: %s\n", input_secs,
           result);
  } else {
    printf("Test failed for input %.2f seconds. Expected: %s, but got: %s\n",
           input_secs, expected_output, result);
  }
}

int main()
{
  // Test cases
  test_generate_timestamp(3661.0f, "1:01:01");
  test_generate_timestamp(61.0f, "01:01");
  test_generate_timestamp(59.0f, "00:59");
  test_generate_timestamp(3600.0f, "1:00:00");
  test_generate_timestamp(0.0f, "00:00");
  test_generate_timestamp(125.0f, "02:05");
  test_generate_timestamp(3601.0f, "1:00:01");

  return 0;
}
