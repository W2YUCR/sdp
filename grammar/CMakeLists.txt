add_library(Grammar)
target_sources(Grammar PUBLIC FILE_SET CXX_MODULES FILES
    grammar.cppm
    tokenizer.cppm
    parser.cppm
    analysis.cppm)
target_link_libraries(Grammar PRIVATE
    Boost::flyweight
    magic_enum::magic_enum)

add_executable(Analysis.Tests analysis.tests.cppm)
target_link_libraries(Analysis.Tests PUBLIC
    Grammar
    Boost::flyweight
    Catch2::Catch2WithMain
    libassert::assert)
catch_discover_tests(Analysis.Tests)