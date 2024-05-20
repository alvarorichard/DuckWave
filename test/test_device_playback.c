#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../include/duckwave.h"

// Static declaration to ensure the lifetime of the expected configuration
static ma_device_config expected_config = {
    .playback.format = ma_format_f32,
    .playback.channels = 2,
    .sampleRate = 44100,
    .dataCallback = duckwave_default_playback_callback,
    .pUserData = NULL  // Set to NULL or an appropriate default
};

// Mock function for device config initialization
ma_device_config mock_device_config_init(ma_device_type device_type) {
    check_expected(device_type);
    return *(ma_device_config*)mock();  // Expecting a pointer to ma_device_config
}

void setup_tests() {
    // Set the function pointer to the mock function
    audioLib.device_config_init = mock_device_config_init;
}

void test_duckwave_init_device_playback(void **state) {
    (void)state;  // Unused parameter

    // Set expectations and return values for the mock function
    expect_value(mock_device_config_init, device_type, ma_device_type_playback);
    will_return(mock_device_config_init, &expected_config);

    DuckWaveSoundData dw_sdata;
    duckwave_init_device_playback(&dw_sdata);

    // Assertions to verify behavior
    assert_int_equal(dw_sdata.device_config.playback.format, expected_config.playback.format);
    assert_int_equal(dw_sdata.device_config.playback.channels, expected_config.playback.channels);
    assert_int_equal(dw_sdata.device_config.sampleRate, expected_config.sampleRate);
    assert_ptr_equal(dw_sdata.device_config.dataCallback, expected_config.dataCallback);
    assert_ptr_equal(dw_sdata.device_config.pUserData, expected_config.pUserData);
}

int main(void) {
    setup_tests();
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_duckwave_init_device_playback),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
