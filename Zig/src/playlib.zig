const std = @import("std");

const c = @cImport({
    @cInclude("stddef.h");
    @cInclude("ao/ao.h");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("mpg123.h");
    @cInclude("string.h");
    @cInclude("termios.h");
    @cInclude("unistd.h");
    @cInclude("fcntl.h");
});

pub const PlayMP3 = struct {

    buffer_size: usize,
    done: usize,
    driver: c.int,
    err: c.int,
    channels: c.int,
    encoding: c.int,
    rate: c.long,
    track: [*c]u8,
    mh: *c.mpg123_handle,
    buffer: [*c]u8,
    format: c.ao_sample_format,
    dev: *c.ao_device,
    
};

