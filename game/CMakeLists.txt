file(GLOB_RECURSE Game_SRC src/game/*.cpp include/game/*.h)
file(GLOB_RECURSE Network_SRC src/network/*.cpp include/network/*.h)

source_group("Game"				FILES ${Game_SRC})
source_group("Network"				FILES ${Network_SRC})

add_library(GameLib STATIC ${Game_SRC} ${Network_SRC})
target_include_directories(GameLib PUBLIC include/)
target_link_libraries(GameLib PUBLIC CoreLib)
set_target_properties(GameLib PROPERTIES UNITY_BUILD ON)
set_target_properties (GameLib PROPERTIES FOLDER Game)