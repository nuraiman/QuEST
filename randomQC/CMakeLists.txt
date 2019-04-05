# My programs (giulia)

set(executables "rqc")

foreach(exec ${executables})
    add_executable(${exec} "${exec}.cpp")
    target_link_libraries(${exec} QuEST m)
endforeach()

