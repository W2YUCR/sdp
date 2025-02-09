add_executable(ll-table-gen)
target_sources(ll-table-gen PUBLIC FILE_SET CXX_MODULES FILES ll-table-gen.cppm)
target_link_libraries(ll-table-gen PRIVATE Grammar mio::mio)