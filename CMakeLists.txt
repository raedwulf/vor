cmake_minimum_required(VERSION 2.8)
project(vor C)

set(CMAKE_C_FLAGS "-std=c11")
set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")

include_directories(include)

set(VOR_HEADERS include/acoder.h include/pd_order0.h)

add_executable(acoder_test tests/acoder_test.c ${VOR_HEADERS})
add_executable(vorcmp tests/vorcmp.c)
add_executable(nn_test tests/nn_test.c)
add_executable(vor src/vor.c ${VOR_HEADERS})

set(ENWIK8_URL "http://cs.fit.edu/~mmahoney/compression/enwik8.zip")
set(ENWIK8_ZIP "${CMAKE_CURRENT_BINARY_DIR}/enwik8.zip")
set(ENWIK8_FILE "${CMAKE_CURRENT_BINARY_DIR}/enwik8")

if (NOT EXISTS "${ENWIK8_FILE}")
	if (NOT EXISTS "${ENWIK8_ZIP}")
		file(DOWNLOAD "${ENWIK8_URL}" "${ENWIK8_ZIP}")
	endif()
endif()

add_custom_command(
    OUTPUT "${ENWIK8_FILE}"
    COMMAND unzip -n "${ENWIK8_ZIP}"
    DEPENDS "${ENWIK8_ZIP}")

add_custom_target(enwik8_ DEPENDS "${ENWIK8_FILE}")
add_dependencies(acoder_test enwik8_)

target_link_libraries(acoder_test m)
target_link_libraries(nn_test m)
target_link_libraries(vor m)

enable_testing()

add_test(NAME acoder_test_c_enwik8
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND acoder_test c enwik8 enwik8.act)
add_test(NAME acoder_test_d_enwik8
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND acoder_test d enwik8.act enwik8.2)
add_test(NAME acoder_test_cmp_enwik8
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND vorcmp enwik8 enwik8.2)
add_test(NAME nn_test
	COMMAND nn_test)

set_tests_properties(acoder_test_c_enwik8 PROPERTIES DEPENDS enwik8)
set_tests_properties(acoder_test_d_enwik8 PROPERTIES DEPENDS acoder_test_c_enwik8)
set_tests_properties(acoder_test_cmp_enwik8 PROPERTIES DEPENDS acoder_test_d_enwik8)
