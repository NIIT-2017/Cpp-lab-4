cmake_minimum_required(VERSION 3.12.1)
project(fortune-сlient-server)

#Стандарт языка
set(CMAKE_CXX_STANDARD 11)

#Библиотеки
find_library(ZLIB_LIBRARY z)
find_package(OpenSSL REQUIRED)

#QT
#Без явного указания пути, у меня не работает
#set(Qt5Network_DIR /home/dmitriy/Programs/Qt/5.11.1/gcc_64/lib/cmake/Qt5Network)

find_package(Qt5Network REQUIRED)
set(CMAKE_AUTOMOC ON)

#Automatically add the current source- and build directories to the include path
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#Директория для выходных фалов
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
set(OUTPUT_PATH_SERVER ${EXECUTABLE_OUTPUT_PATH}/release/fserver)
set(OUTPUT_PATH_CLIENT ${EXECUTABLE_OUTPUT_PATH}/release/fclient)

#Сборка программы
set(FSERVER server/main.cpp server/ClientThread.cpp server/MyTcpServer.cpp)
set(FCLIENT client/main.cpp client/MyTcpClient.cpp)
set(MYLIB mylib/Connection.cpp mylib/ConsoleHelper.cpp mylib/Message.cpp mylib/MyTcpApp.cpp)

add_executable(fserver ${FSERVER} ${MYLIB})
add_executable(fclient ${FCLIENT} ${MYLIB})

#Присоединение библиотек
target_link_libraries(fserver ${ZLIB_LIBRARY})
target_link_libraries(fserver ${OPENSSL_LIBRARIES})
target_link_libraries(fclient ${ZLIB_LIBRARY})
target_link_libraries(fclient ${OPENSSL_LIBRARIES})

qt5_use_modules(fserver Network)
qt5_use_modules(fclient Network)

#Копирование дополнительных файлов в папку релиза
configure_file(server/config_server.ini ${OUTPUT_PATH_SERVER}/config_server.ini COPYONLY)
configure_file(aphorisms.txt ${OUTPUT_PATH_SERVER}/aphorisms.txt COPYONLY)
configure_file(client/config_client.ini ${OUTPUT_PATH_CLIENT}/config_client.ini COPYONLY)

#Копирование собранных файлов в релиз
add_custom_command(TARGET fserver POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/fserver ${OUTPUT_PATH_SERVER}/)
add_custom_command(TARGET fclient POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy ${EXECUTABLE_OUTPUT_PATH}/fclient ${OUTPUT_PATH_CLIENT}/)