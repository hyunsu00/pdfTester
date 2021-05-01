# CMakeList.txt
cmake_minimum_required (VERSION 2.8)

option(BUILD_PDFBOX_TESTER "Enable building pdfboxTester" ON)
option(BUILD_PDFIUM_TESTER "Enable building pdfiumTester" ON)

if(BUILD_PDFBOX_TESTER)
    add_subdirectory(pdfboxTester)
endif()
if(BUILD_PDFIUM_TESTER)
    add_subdirectory(pdfiumTester)
endif()
