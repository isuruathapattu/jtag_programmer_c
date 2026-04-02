add_executable(svf_sim
    src/svf_sim.c
)

target_link_libraries(svf_sim PRIVATE
    xsvf
)