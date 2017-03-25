
find_package(Qt5Core)
find_package(Qt5Widgets)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${Qt5Core_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})

add_library(CarValueLib
    Brand.cpp
    Brands.cpp
    CarValueApplication.cpp
    CarValueWindow.cpp
    CsvParser.cpp
    Model.cpp
)
target_link_libraries(CarValueLib Qt5::Core Qt5::Widgets)

add_executable(CarValue WIN32 main.cpp)
target_link_libraries(CarValue CarValueLib)

get_target_property(QT5_LIBRARYPATH Qt5::Core LOCATION)
get_filename_component(QT5_LIBRARYPATH ${QT5_LIBRARYPATH} PATH)

set(dependency_directories ${QT5_LIBRARYPATH})

include(InstallRequiredSystemLibraries)

install(TARGETS CarValue DESTINATION bin)
install(
    DIRECTORY "../data/trafi"
    DESTINATION "data"
    FILES_MATCHING PATTERN "*.csv")
install(
    FILES "../data/rust_protection.csv"
    DESTINATION "data")

install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/bin/CarValue.exe\" \"\" \"${dependency_directories}\")
")
