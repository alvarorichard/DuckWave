toolchain("DuckWave_Toolchain")
    set_kind("standalone")

    set_toolset("cc", "clang")
    set_toolset("cxx", "clang++", "clang")
    set_toolset("ld", "clang", "clang++")
toolchain_end()

target("DuckWave")
    set_kind("binary")

    -- TODO: In future, is better using globbing instead file to file
    add_includedirs("include")
     
    add_files("src/main.c", "src/duckwave.c")

    set_optimize("fastest")


    -- Set C language standard, if needed (e.g., c99, c11)
    set_languages("c17")

    -- Add any necessary compile flags, if specific flags are needed
      add_ldflags("-lm","-Wall" ,"-Wextra")

