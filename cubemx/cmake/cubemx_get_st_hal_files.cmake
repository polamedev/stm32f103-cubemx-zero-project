# ------------------------------------------------------------------------------------------------------------
# Функция cubemx_get_st_hal_files получает список необходимых файлов .c библиотеки ST_HAL, сгенерированным CubeMX
function(cubemx_get_st_hal_files varName)
    # Получить путь директории генерации CubeMX
    get_property(cubemx_gen_dir GLOBAL PROPERTY cubemx_gen_dir_property)

    # Поиск строк, содержащих файлы с hal драйвером, в cmake файле, сгенерированном cubemx
    file(STRINGS ${cubemx_gen_dir}/cmake/stm32cubemx/CMakeLists.txt cubemx_hal_file_cmake_string REGEX "Drivers/STM32F1xx_HAL_Driver/Src/")
    # Разделим полученные 
    string(REPLACE "    \${CMAKE_SOURCE_DIR}/Drivers/STM32F1xx_HAL_Driver/Src/" ";" cubemx_hal_file_list ${cubemx_hal_file_cmake_string})
    # Первый элемент в списке файлов пустой, его нужно убрать 
    list(REMOVE_AT cubemx_hal_file_list 0)

    # Установить список файлов в имя переданной переменной с помощью PARENT_SCOPE
    set(${varName} "${cubemx_hal_file_list}" PARENT_SCOPE)

endfunction()
