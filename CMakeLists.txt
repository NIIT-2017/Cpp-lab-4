cmake_minimum_required(VERSION 2.8)

# так пишутся комментарии

project(Cpp-lab-4)
#include_directories(zlib-master)
set(ZLIB_ROOT F:/GitHub/Cpp-lab-4/zlib-master)
find_package(ZLIB REQUIRED)
include_directories(include)
set(CMAKE_CXX_STANDARD 14) 

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURC_DIR}/build)
set(OUTPUT_PATH_SERVER ${EXECUTABLE_OUTPUT_PATH}/release/server)
set(OUTPUT_PATH_SERVER ${EXECUTABLE_OUTPUT_PATH}/release/client)

# Сборка
set(FSERVER src/Server/Server.cpp src/Server/ServerMain.cpp)
set(FCLIENT src/Client/Client.cpp src/Client/ClientMain.cpp)
set(LIBSERVER include/Server/Server.h)
set(LIBCLIENT include/Client/Client.h)

# Создает исполняемые файлы
add_executable(server ${FSERVER} ${LIBSERVER})
add_executable(client ${FCLIENT} ${LIBCLIENT})

# Подсоединяем библиотеки
target_link_libraries(server zlib zlibstatic) 
target_link_libraries(client zlib zlibstatic)

if(WIN32)
  target_link_libraries(server wsock32 ws2_32)
  target_link_libraries(client wsock32 ws2_32)
endif()
 
# копирование доп файлов в релиз
configure_file(src/Server/Config.txt ${OUTPUT_PATH_SERVER}/Config.txt COPYONLY)
configure_file(src/Server/Aforisms.txt ${OUTPUT_PATH_SERVER}/Aforisms.txt COPYONLY)

# копирование собранных файлов в релиз
add_custom_command(TARGET server POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/server ${OUTPUT_PATH_SERVER}/)
add_custom_command(TARGET client POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/client ${OUTPUT_PATH_CLIENT}/)