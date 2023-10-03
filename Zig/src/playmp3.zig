const std = @import("std");


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

pub fn isSpacePressed() void {
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

    if(ch == ' ')
        return 1;

    return 0;

}


