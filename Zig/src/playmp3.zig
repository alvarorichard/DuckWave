const std = @import("std");
const playlib = @import("playlib.zig");

const c = @cImport({
    @cInclude("ao/ao.h");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
    @cInclude("mpg123.h");
    @cInclude("string.h");
    @cInclude("termios.h");
    @cInclude("unistd.h");
    @cInclude("fcntl.h");
});

pub fn isSpacePressed() bool {
    var oldt: c.termios = undefined;
    var newt: c.termios = undefined;
    var ch: c.int = undefined;
    var oldf: c.int = undefined;

    c.tcgetattr(c.STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(c.ICANON | c.ECHO);
    c.tcsetattr(c.STDIN_FILENO, c.TCSANOW, &newt);
    oldf = c.fcntl(c.STDIN_FILENO, c.F_GETFL, 0);
    c.fcntl(c.STDIN_FILENO, c.F_SETFL, oldf | c.O_NONBLOCK);

    ch = c.getchar();
    c.tcsetattr(c.STDIN_FILENO, c.TCSANOW, &oldt);
    c.fcntl(c.STDIN_FILENO, c.F_SETFL, oldf);

    return ch == ' ';
}


pub fn play(mp3: *playlib.PlayMP3) void {
    var isPaused: i32 = 0;
    std.debug.print("\x1b[33;1m\u{256B6} Playing The Song: \x1b[35;1m{s}\x1b[m\n", .{ mp3.track });

    //std.debug.print("\x1b[33;1m\u{256B6} Playing The Song: \x1b[35;1m{}\x1b[m\n", .{ mp3.track });
    while (c.mpg123_read(mp3.mh, mp3.buffer, mp3.buffer_size, &mp3.done) == c.MPG123_OK) {
        if (isSpacePressed()) {
            isPaused = if (isPaused == 0) 1 else 0;  // toggle isPaused
            c.usleep(300000);  // 300ms
        }

        if (isPaused == 0) {
            c.ao_play(mp3.dev, mp3.buffer, mp3.done);
        } else {
            c.usleep(100000);  // 100ms
        }
    }
}

pub fn init_PlayMP3(mp3: *playlib.PlayMP3) void {
    var err: i32 = 0;
    mp3.driver = c.ao_default_driver_id();
 const result = c.mpg123_init();
 if (result != c.MPG123_OK) {
    std.debug.print("Error initializing mpg123: {}\n", .{result});
    return; // or handle the error in some other way
}
    mp3.mh = c.mpg123_new(null, &err);
    mp3.buffer_size = c.mpg123_outblock(mp3.mh);
    mp3.buffer = std.mem.alloc(u8, mp3.buffer_size) orelse unreachable;
    c.ao_initialize();
    mp3.dev = c.ao_open_live(mp3.driver, mp3.format, &mp3.dev_info);
}

pub fn setMusic(mp3: *playlib.PlayMP3, track: []const u8) void {
    var err: i32 = 0;
    _ = err;
    var buffer_size: usize = 0;
    var done: usize = 0;
mp3.track = @ptrCast([*c]u8, track.ptr);


   // mp3.track = track.ptr;
    mp3.track_len = track.len;
    buffer_size = c.mpg123_outblock(mp3.mh);
    mp3.buffer = std.mem.alloc(u8, buffer_size) orelse unreachable;

    mp3.fd = c.open(c.strLit(mp3.track), c.O_RDONLY);
    c.mpg123_open_fd(mp3.mh, mp3.fd);
    c.mpg123_getformat(mp3.mh, &mp3.rate, &mp3.channels, &mp3.encoding);
    mp3.format.bits = c.ao_bit_depth(mp3.encoding);
    mp3.format.rate = mp3.rate;
    mp3.format.channels = mp3.channels;
    mp3.format.byte_format = c.AO_FMT_NATIVE;
    mp3.format.matrix = null;

    c.mpg123_seek(mp3.mh, 0, c.SEEK_SET);
    c.mpg123_read(mp3.mh, mp3.buffer, buffer_size, &done);
}

