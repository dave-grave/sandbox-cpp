# tests/CMakeLists.txt

add_executable(
	thg_lib_tests	
	string_test.cpp
)

target_link_libraries(
	thg_lib_tests

	PUBLIC
		GTest::gtest_main
	PRIVATE
		thg_core
)

# include(GoogleTest)
# gtest_discover_tests(thg_lib_tests)
