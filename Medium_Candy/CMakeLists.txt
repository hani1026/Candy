cmake_minimum_required(VERSION 3.10)
project(Medium_Candy)

# ─ Geant4 ─
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

# ─ ROOT ─
find_package(ROOT REQUIRED COMPONENTS Core Tree RIO Hist)

include_directories(${PROJECT_SOURCE_DIR}/include
                    ${Geant4_INCLUDE_DIR}
                    ${ROOT_INCLUDE_DIRS})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(sim main.cc ${sources} ${headers})

# C++17 적용 ------------------------------
set_target_properties(sim PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

target_link_libraries(sim PUBLIC ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

# 매크로 복사
set(SiPM_SCRIPTS init_vis.mac vis.mac)
foreach(_script ${SiPM_SCRIPTS})
  configure_file(${PROJECT_SOURCE_DIR}/${_script}
                 ${PROJECT_BINARY_DIR}/${_script}
                 COPYONLY)
endforeach()
