const std = @import("std");
const c = @import("c.zig");



pub const PlayMP3 = struct {
    buffer_size: usize,
    done: usize,
    driver: i32,
    channels: i32,
    encoding: i32,
    rate: i32,
    track: [*c]u8,
    mh: ?*c.mpg123_handle_struct,
    buffer: [*c]u8,
    format: c.ao_sample_format,
    dev: *c.ao_device,
    track_len: usize,
};
const playlib = struct {
    PlayMP3: extern struct {
        format: *c.struct_ao_sample_format,
        dev_info: c.struct_ao_option,
        dev: *c.struct_ao_device,
        track: [*c]u8,
        track_len: usize,
        driver: i32,
        channels: i32,
        encoding: i32,
        rate: i32,
        buffer_size: usize,
        done: usize,
        mh: ?*c.struct_mpg123_handle_struct,
        buffer: [*c]u8,
        ptr : *c.u8,
    },
};