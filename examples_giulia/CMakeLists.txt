# My programs (giulia)

set(executables "gtest" "ghadamard" "dj" "grover" "period-finding" "approx-pf" "shor")

foreach(exec ${executables})
    add_executable(${exec} "${exec}.cpp")
    target_link_libraries(${exec} QuEST m)
endforeach()

