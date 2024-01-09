const std = @import("std");
const playlib = @import("playlib.zig");
const playmp3 = @import("playmp3.zig");
const c = @import("c.zig");

pub fn main() !void {
 var mp3: playlib.PlayMP3 = undefined;

    playmp3.init_PlayMP3(&mp3);
    playmp3.setMusic(&mp3, "musica.mp3");
    playmp3.play(&mp3);
}

