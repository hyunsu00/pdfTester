# CMakeList.txt
cmake_minimum_required (VERSION 2.8)

#
# 프로젝트 이름 지정
project ("pdfboxTester")
message(STATUS "\${PROJECT_NAME} = ${PROJECT_NAME}")

# 빌드 상세 과정을 모두 출력하는 Makefile을 생성
set (CMAKE_VERBOSE_MAKEFILE true)

###
# 패키지 찾기 - PDFiumConfig.cmake 파일 인클루드
find_package(JNI REQUIRED)
include_directories(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
message(STATUS "\${JAVA_INCLUDE_PATH} = ${JAVA_INCLUDE_PATH}")
message(STATUS "\${JAVA_INCLUDE_PATH2} = ${JAVA_INCLUDE_PATH2}")
message(STATUS "\${CMAKE_CURRENT_LIST_DIR} = ${CMAKE_CURRENT_LIST_DIR}")

###
#
set (EXTERN_DIR ${CMAKE_SOURCE_DIR}/../Extern)
message(STATUS "\${EXTERN_DIR} = ${EXTERN_DIR}")
include_directories(${EXTERN_DIR})

set (COMMON_DIR ${CMAKE_SOURCE_DIR}/../common)
message(STATUS "\${COMMON_DIR} = ${COMMON_DIR}")
include_directories(${COMMON_DIR})

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
)
message(STATUS "\${EXTERN_FILES} = ${EXTERN_FILES}")
set(COMMON_FILES 
	"${COMMON_DIR}/pdf_assert.h" 
	"${COMMON_DIR}/pdf_utils.h"
)
message(STATUS "\${COMMON_FILES} = ${COMMON_FILES}")
set(SOURCE_FILES 
	"main.cpp" 
	"PDFBoxConverter.cpp"
	"PDFBoxConverter.h"
)
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
target_link_libraries(${PROJECT_NAME} ${JNI_LIBRARIES})
message(STATUS "\${JNI_LIBRARIES} = ${JNI_LIBRARIES}")
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(${PROJECT_NAME} "Shlwapi.lib")
endif()

# LINUX GCC C++ 11 지원 -> 버전이 낮으면 지원하지 않는다.
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
	COMMENT "pdfbox 라이브러리를 복사합니다."
	COMMAND ${CMAKE_COMMAND} -E copy "${EXTERN_DIR}/pdfbox/PDFBoxModule.jar" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/PDFBoxModule.jar"
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/samples/"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/../samples" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/samples/"
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>/result/"
)
message(STATUS "\${CMAKE_COMMAND} = ${CMAKE_COMMAND}")

install(TARGETS ${PROJECT_NAME} DESTINATION ${PROJECT_NAME})
install(FILES "${EXTERN_DIR}/pdfbox/PDFBoxModule.jar" DESTINATION ${PROJECT_NAME})

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
set(CPACK_PACKAGE_VENDOR "Hncom.Inc")
set(CPACK_PACKAGE_CONTACT "Kim HyunSu")
# set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
# set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
# install(FILES ${CPACK_RESOURCE_FILE_LICENSE} ${CPACK_RESOURCE_FILE_README} DESTINATION ${PROJECT_NAME})
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/package")
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
