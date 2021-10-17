find_package(SFML COMPONENTS system window graphics network audio CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(GLOB_RECURSE Utils_SRC src/utils/*.cpp include/utils/*.h)
file(GLOB_RECURSE Maths_SRC src/maths/*.cpp include/maths/*.h)
file(GLOB_RECURSE Engine_SRC src/engine/*.cpp include/engine/*.h)
file(GLOB_RECURSE Graphics_SRC src/graphics/*.cpp include/graphics/*.h)

source_group("Engine"				FILES ${Engine_SRC})
source_group("Maths"				FILES ${Maths_SRC})
source_group("Utils"				FILES ${Utils_SRC})
source_group("Graphics"				FILES ${Graphics_SRC})

add_library(CoreLib STATIC ${Engine_SRC} ${Maths_SRC} ${Utils_SRC} ${Graphics_SRC})
target_include_directories(CoreLib PUBLIC include/)
target_link_libraries(CoreLib PUBLIC sfml-system sfml-network sfml-graphics sfml-window
	sfml-network sfml-audio ImGui-SFML::ImGui-SFML spdlog::spdlog fmt::fmt)
set_target_properties(CoreLib PROPERTIES UNITY_BUILD ON)

find_package(GTest CONFIG REQUIRED)
file(GLOB_RECURSE test_files test/*.cpp)
add_executable(CoreTest ${test_files})
target_link_libraries(CoreTest PRIVATE GTest::gtest GTest::gtest_main CoreLib)