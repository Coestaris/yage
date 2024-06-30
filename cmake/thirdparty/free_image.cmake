add_definitions(-DFREEIMAGE_LIB)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/free_image)

target_include_directories(yage PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/free_image/Source)
target_link_libraries(yage PRIVATE FreeImage::FreeImage)
