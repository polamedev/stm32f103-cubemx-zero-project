set(LAME_CMAKELIB ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Директория библиотеки lame_cmakelib")

macro(subdirlist result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()


# Функция по созданию таргета - просто подключает файл в котором создается таргет 
function(add_lame_target targetName)
  message(STATUS "add_lame_target " ${targetName})
  include(${CMAKE_CURRENT_FUNCTION_LIST_DIR}/target/include_${targetName}.cmake)
endfunction(add_lame_target)
