set_xmakever("2.8.9")
set_version("0.1.0", { build = "%Y%m%d%H%M" })

set_allowedmodes("debug", "release")
set_defaultmode("debug")
add_rules("mode.debug", "mode.release")

toolchain("dwtc") -- recommended toolchain
set_kind("standalone")

set_toolset("cc", "clang")
set_toolset("cxx", "clang++", "clang")
set_toolset("ld", "clang", { force = true }) -- ensure mold is used as linker
set_toolset("sh", "clang++", "clang")
set_toolset("ar", "llvm-ar")
toolchain_end()

target("duckwave")
set_kind("binary")
add_includedirs("include")
add_files("src/main.c", "src/duckwave.c")

if is_mode("release") then
	set_optimize("fastest")
	set_symbols("hidden")
elseif is_mode("debug") then
	set_optimize("none")
	set_symbols("debug")
end

set_targetdir("bin")
set_languages("c17")
add_ldflags("-fuse-ld=mold", { force = true }) -- add mold linker flag
-- add_links("avformat", "avcodec", "ao", "m")
add_links("ncurses", "m")
target_end()

