project(nf_comments)
set(NF_DEPENDENTS_DIRS ${CMAKE_SOURCE_DIR}/dependents)
include_directories(${NF_DEPENDENTS_DIRS}/include)
link_directories(${NF_DEPENDENTS_DIRS}/lib)

set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/output CACHE STRING "install paths" FORCE)
###################################################################
# build sqlite3
###################################################################
find_library(SQLITE_FOUND_RESULT sqlite3 PATHS ${NF_DEPENDENTS_DIRS}/lib NO_DEFAULT_PATH)
if("SQLITE_FOUND_RESULT-NOTFOUND" STREQUAL SQLITE_FOUND_RESULT)
    message(STATUS "Build sqllite3, please waiting...")
    execute_process(COMMAND chmod +x build_and_install_sqlite3.sh
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools             
    )
    execute_process(COMMAND ${CMAKE_SOURCE_DIR}/tools/build_and_install_sqlite3.sh ${NF_DEPENDENTS_DIRS} sqlite_build.log
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vendors
                    RESULT_VARIABLE SHELL_RET 
    )
    if(NOT "${SHELL_RET}" STREQUAL "0")
        message(FATAL_ERROR "Fatal:when installing sqlite3 please see log:${CMAKE_SOURCE_DIR}/vendors/temp/sqlite-snapshot-201802271540/sqlite_build.log")
    else()
        message(STATUS "Build sqlite3 successful.")
    endif()
endif()
###################################################################
# build jsoncpp
###################################################################
find_library(JSONCPP_FOUND_RESULT jsoncpp PATHS ${NF_DEPENDENTS_DIRS}/lib NO_DEFAULT_PATH)
if("JSONCPP_FOUND_RESULT-NOTFOUND" STREQUAL JSONCPP_FOUND_RESULT)
    message(STATUS "Build jsoncpp, please waiting...")
    execute_process(COMMAND chmod +x build_and_install_jsoncpp.sh
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tools             
    )
    execute_process(COMMAND ${CMAKE_SOURCE_DIR}/tools/build_and_install_jsoncpp.sh ${NF_DEPENDENTS_DIRS} jsoncpp_build.log
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/vendors
                    RESULT_VARIABLE SHELL_RET 
    )
    if(NOT "${SHELL_RET}" STREQUAL "0")
        message(FATAL_ERROR "Fatal:when installing jsoncpp please see log:${CMAKE_SOURCE_DIR}/vendors/temp/jsoncpp-0.y.z/build/jsoncpp_build.log")
    else()
        message(STATUS "Build jsoncpp successful.")
    endif()
endif()
###################################################################
# add srcs
###################################################################
set(SOURCE_FILES main.cpp
        CommentsDataManager.cpp
        CommentsCacheManager.cpp
        CommentElement.cpp
        CommentsDataCache.cpp
        CommentsData.cpp
        Config.cpp
        TestCaseLoader.cpp
        Logger.cpp
        mongoose.c
        HttpServer.cpp
        Service.cpp)
###################################################################
# add executable
###################################################################
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} sqlite3 jsoncpp pthread dl)
###################################################################
# install
###################################################################
install(TARGETS ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_PREFIX})

find_file(RUN_CONFIG_FILE NAMES nf_config.json  PATHS ${CMAKE_INSTALL_PREFIX} NO_DEFAULT_PATH)
if("RUN_CONFIG_FILE-NOTFOUND" STREQUAL RUN_CONFIG_FILE)
    install(FILES ${CMAKE_SOURCE_DIR}/conf/nf_config.json
            DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()