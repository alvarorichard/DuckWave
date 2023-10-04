const Builder = @import("std").Build;

pub fn build(b: *Builder) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "ZigSoundWave",
        .target = target,
        .optimize = optimize,
        .root_source_file = .{ .path = "src/main.zig" },
    });

    exe.addIncludePath(.{ .path = "src" }); // Caminho para cabeçalhos
    exe.linkLibC();
    exe.linkSystemLibrary("ao"); // Vincular com a libao

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", b.fmt("Run the {s} app.", .{exe.name}));
    run_step.dependOn(&run_cmd.step);
}
