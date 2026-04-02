add_executable(svf_sim
    src/main.c
    src/vcd_writer.c
    src/svf_sim_host.c
)

target_include_directories(svf_sim PRIVATE
    include
)

target_link_libraries(svf_sim PRIVATE
    xsvf
)