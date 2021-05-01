# CMakeList.txt
cmake_minimum_required (VERSION 2.8)

#
# 프로젝트 이름 지정
project ("pdfTester")
message(STATUS "\${PROJECT_NAME} = ${PROJECT_NAME}")

option(BUILD_PDFBOX_TESTER "Enable building pdfboxTester" ON)
option(BUILD_PDFIUM_TESTER "Enable building pdfiumTester" ON)

if(BUILD_PDFBOX_TESTER)
    add_subdirectory(pdfboxTester)
endif()
if(BUILD_PDFIUM_TESTER)
    add_subdirectory(pdfiumTester)
endif()
