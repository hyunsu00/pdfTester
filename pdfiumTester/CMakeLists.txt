# CMakeList.txt
cmake_minimum_required (VERSION 2.8)

#
# 프로젝트 이름 지정
project ("pdfiumTester")
message(STATUS "\${PROJECT_NAME} = ${PROJECT_NAME}")

# 빌드 상세 과정을 모두 출력하는 Makefile을 생성
set (CMAKE_VERBOSE_MAKEFILE true)

###
# pfdium 설치위치 설정
set (PDFium_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Extern/pdfium)
message(STATUS "\${CMAKE_CURRENT_SOURCE_DIR} = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "\${PDFium_DIR} = ${PDFium_DIR}")

###
# 패키지 찾기 - PDFiumConfig.cmake 파일 인클루드
find_package(PDFium REQUIRED)
message(STATUS "\${PDFium_INCLUDE_DIR} = ${PDFium_INCLUDE_DIR}")
message(STATUS "\${PDFium_LIBRARY} = ${PDFium_LIBRARY}")
message(STATUS "\${CMAKE_CURRENT_LIST_DIR} = ${CMAKE_CURRENT_LIST_DIR}")

# 패키지 찾기 - PNGConfig.cmake 파일 인클루드
# sudo apt install libpng-dev -y (데비안)
# sudo yum install libpng-devel -y (센트OS)
# sudo apt install libpng-devel -y (데비안)
# sudo yum install libtbb-dev -y (센트OS)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	# PNG
	find_package(PNG REQUIRED)
	include_directories(${PNG_INCLUDE_DIR})
	message(STATUS "\${PNG_INCLUDE_DIR} = ${PNG_INCLUDE_DIR}")

	# TBB
	list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
	message(STATUS "\${CMAKE_MODULE_PATH} = ${CMAKE_MODULE_PATH}")
	find_package(TBB REQUIRED)
	include_directories(${TBB_INCLUDE_DIRS})
	message(STATUS "\${TBB_INCLUDE_DIRS} = ${TBB_INCLUDE_DIRS}")
endif() 

###
#
set (EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Extern)
message(STATUS "\${EXTERN_DIR} = ${EXTERN_DIR}")
include_directories(${EXTERN_DIR})

set (COMMON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../common)
message(STATUS "\${COMMON_DIR} = ${COMMON_DIR}")
include_directories(${COMMON_DIR})

if (CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(TARGET_PLATFORM "x64")
elseif (CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(TARGET_PLATFORM "x86")
endif()
message(STATUS "\${TARGET_PLATFORM} = ${TARGET_PLATFORM}")

# 플랫폼 독립 코딩 위해 CRT 보안관련 경고 및 VC컴파일러 경고 비활성화
if (MSVC)
	# 파일 관련 함수 보안 경고 무시
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	# cmdline.h 사용시 컴파일러 경고 무시
	add_compile_options(
		/wd4267
		/wd4566
		/wd4101
	)
endif()

###
# 소스 파일 설정
set(EXTERN_FILES 
	"${EXTERN_DIR}/cmdline.h" 
	"${EXTERN_DIR}/span.h"
)
message(STATUS "\${EXTERN_FILES} = ${EXTERN_FILES}")
set(COMMON_FILES 
	"${COMMON_DIR}/pdf_assert.h" 
	"${COMMON_DIR}/pdf_utils.h"
)
message(STATUS "\${COMMON_FILES} = ${COMMON_FILES}")
set(SOURCE_FILES 
	"main.cpp" 
	"PDFiumConverter.cpp" 
	"PDFiumConverter.h"
	"fpdf_converter.h"
	"fpdf_raii.h"
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(SOURCE_FILES ${SOURCE_FILES} "image_png.cpp" "image_png.h" "write_libpng.inl")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(SOURCE_FILES ${SOURCE_FILES} "write_gdiplus.inl")
endif()
message(STATUS "\${SOURCE_FILES} = ${SOURCE_FILES}")

###
# 이 프로젝트의 실행 파일에 소스를 추가합니다.
add_executable (${PROJECT_NAME} 
	${SOURCE_FILES}
	${COMMON_FILES}
	${EXTERN_FILES}
)

###
# 실행파일 생성후에 지정
target_link_libraries(${PROJECT_NAME} pdfium)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} ${PNG_LIBRARY})
	message(STATUS "\${PNG_LIBRARY} = ${PNG_LIBRARY}")
	target_link_libraries(${PROJECT_NAME} ${TBB_LIBRARIES})
	message(STATUS "\${TBB_LIBRARIES} = ${TBB_LIBRARIES}")
else()
	target_link_libraries(${PROJECT_NAME} "gdiplus.lib")
	target_link_libraries(${PROJECT_NAME} "Shlwapi.lib")
endif()

# LINUX C++ 11 사용위해 - GCC 버전 체크후 설정해주어야 한다.
message(STATUS "\${CMAKE_SYSTEM_NAME} = ${CMAKE_SYSTEM_NAME}")
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	message(STATUS "\${CMAKE_COMPILER_IS_GNUCC} = ${CMAKE_COMPILER_IS_GNUCC}")
	message(STATUS "\${CMAKE_CXX_COMPILER_VERSION} = ${CMAKE_CXX_COMPILER_VERSION}")
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif()

###
# DLL 실행파일 실행파일 위치로 복사
# ${CMAKE_COMMAND} -E copy_if_different : cmake -E copy_if_different
# $<TARGET_FILE:pdfium> : 주 파일 (.exe, .so.1.2, .a)의 전체 경로입니다. 여기서은 pdfium대상의 이름입니다.
# $<TARGET_FILE_DIR:pdfiumSample02> : 기본 파일 (.exe, .so.1.2, .a)의 디렉토리입니다. 여기서은 pdfiumSample02대상의 이름입니다.
add_custom_command(
	TARGET ${PROJECT_NAME} POST_BUILD
	COMMENT "pdfium 라이브러리를 복사합니다."
	COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:pdfium> $<TARGET_FILE_DIR:${PROJECT_NAME}>
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/samples/"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../samples" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/samples/"
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/result/"
)
message(STATUS "\${CMAKE_COMMAND} = ${CMAKE_COMMAND}")

# 윈도우에서는 메모리닉 체크 DLL을 복사한다.
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(VLD_DIR "${EXTERN_DIR}/vld/${TARGET_PLATFORM}/bin")
	message(STATUS "\${VLD_DIR} = ${VLD_DIR}")

	add_custom_command(
		TARGET ${PROJECT_NAME} POST_BUILD
		COMMENT "vld파일을 복사 합니다."
		COMMAND ${CMAKE_COMMAND} -E copy_directory "${VLD_DIR}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
	)
endif()
message(STATUS "\${CMAKE_CURRENT_BINARY_DIR} = ${CMAKE_CURRENT_BINARY_DIR}")

###
# 인스톨
install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
install(FILES ${PDFium_LIBRARY} DESTINATION ${PROJECT_NAME})
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/samples" DESTINATION ${PROJECT_NAME})
install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/result" DESTINATION ${PROJECT_NAME})

###
# cpack
set(_fmt TGZ)
if(WIN32)
  set(_fmt ZIP)
endif()

# 인스톨 파일 압축
set(CPACK_GENERATOR ${_fmt})
# 소스 파일 압축
# set(CPACK_SOURCE_GENERATOR ${_fmt})
set(CPACK_PACKAGE_VENDOR "Hancom")
set(CPACK_PACKAGE_CONTACT "Kim HyunSu")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
# set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# install(FILES ${CPACK_RESOURCE_FILE_LICENSE} ${CPACK_RESOURCE_FILE_README} DESTINATION ${PROJECT_NAME})
# set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/package")
# set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

message(STATUS "\${CPACK_OUTPUT_FILE_PREFIX} = ${CPACK_OUTPUT_FILE_PREFIX}")
message(STATUS "\${CPACK_PACKAGE_DIRECTORY} = ${CPACK_PACKAGE_DIRECTORY}")
message(STATUS "\${CPACK_PACKAGE_VERSION} = ${CPACK_PACKAGE_VERSION}")
message(STATUS "\${CPACK_PROJECT_NAME} = ${CPACK_PROJECT_NAME}")
message(STATUS "\${CPACK_PROJECT_VERSION} = ${CPACK_PROJECT_VERSION}")

include(CPack)
