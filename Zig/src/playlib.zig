const c = @cImport({
    @cInclude("mpg123.h");
    @cInclude("ao/ao.h");
    @cInclude("stddef.h");
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

pub fn init_PlayMP3(mp3: *PlayMP3) void;
pub fn setMusic(mp3: *PlayMP3, track: [*c]u8) void;
pub fn play(mp3: *PlayMP3) void;
pub fn cleanup_PlayMP3(mp3: *PlayMP3) void;

