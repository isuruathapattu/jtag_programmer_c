add_executable(svf_prog_mb
    src/main.c
)

target_link_libraries(svf_prog_mb PRIVATE
    xsvf
)