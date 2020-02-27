# allow loader to find lib
if(APPLE)
	set(basePoint @loader_path)
else()
	set(basePoint $ORIGIN)
endif()
include(GNUInstallDirs)
file(RELATIVE_PATH relDir
     ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}
     ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}
     )
set(CMAKE_INSTALL_RPATH ${basePoint} ${basePoint}/${relDir})

add_library(${PROJECT_NAME}
            ${${PROJECT_NAME}_SOURCE_DIR}/include/EquivalenceChecker.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/EquivalenceChecker.cpp
            ${${PROJECT_NAME}_SOURCE_DIR}/include/EquivalenceCheckingResults.hpp
            ${${PROJECT_NAME}_SOURCE_DIR}/include/ImprovedDDEquivalenceChecker.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ImprovedDDEquivalenceChecker.cpp
            ${${PROJECT_NAME}_SOURCE_DIR}/include/CompilationFlowEquivalenceChecker.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/CompilationFlowEquivalenceChecker.cpp
            ${${PROJECT_NAME}_SOURCE_DIR}/include/PowerOfSimulationEquivalenceChecker.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/PowerOfSimulationEquivalenceChecker.cpp
            )
add_library(JKQ::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PUBLIC
                           $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_14)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

option(TRY_USING_INSTALLED_QFR_LIBRARY "Attempt searching for installed version of QFR library and use it if found" ON)

if (TRY_USING_INSTALLED_QFR_LIBRARY)
	find_package(QFR 1.0 QUIET)
	if (QFR_FOUND)
		message(STATUS "using installed QFR library")
	else()
		message(STATUS "using QFR submodule")
		add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr")
	endif()
else()
	message(STATUS "using QFR submodule")
	add_subdirectory("${PROJECT_SOURCE_DIR}/extern/qfr" "extern/qfr")
endif ()

target_link_libraries(${PROJECT_NAME} PUBLIC JKQ::QFR)

# add coverage compiler and linker flag if COVERAGE is set
if (COVERAGE)
	target_compile_options(${PROJECT_NAME} PRIVATE --coverage)
	target_link_libraries(${PROJECT_NAME} PRIVATE --coverage)
endif()

# check if interprocedural optimization is supported
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported)
if(ipo_supported)
	set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# set compiler flags depending on compiler
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE "/W4" "/GR" "$<$<CONFIG:RELEASE>:/O2 /GL>")
	target_link_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/LTCG>")
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall $<$<CONFIG:DEBUG>:-g3 -Og -Wextra -Wpedantic -pedantic> $<$<CONFIG:RELEASE>:-O3 -mtune=native -march=native -DNDEBUG -g0>)
endif()

# installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/QCEC)

install(TARGETS ${PROJECT_NAME}
        EXPORT qcec-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME QCEC)

install(DIRECTORY ${${PROJECT_NAME}_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT qcec-targets
        FILE QCECTargets.cmake
        NAMESPACE JKQ::
        DESTINATION ${INSTALL_CONFIGDIR}
        )

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
		${CMAKE_CURRENT_BINARY_DIR}/QCECConfigVersion.cmake
		VERSION ${PROJECT_VERSION}
		COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${${PROJECT_NAME}_SOURCE_DIR}/cmake/QCECConfig.cmake.in
                              ${CMAKE_CURRENT_BINARY_DIR}/QCECConfig.cmake
                              INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
                              )

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/QCECConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/QCECConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )

# exporting from the build tree
export(EXPORT qcec-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/QCECTargets.cmake NAMESPACE JKQ::)

# Register package in user's package registry
export(PACKAGE QCEC)
