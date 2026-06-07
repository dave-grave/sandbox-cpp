# lib/CMakeLists.txt

add_library(thg_core INTERFACE)

target_sources(thg_core
	INTERFACE 
		FILE_SET thg_headers
		TYPE HEADERS
		BASE_DIRS ${PROJECT_SOURCE_DIR}
		FILES 
			string.h
)

# include testing subdirectory
add_subdirectory(tests)

# link subdirectory to library target inside of tests/CML
