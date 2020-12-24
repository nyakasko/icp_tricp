cmake_minimum_required(VERSION 2.8)
project( 3dsens_icp )

#find_package(nanoflann)
find_package(Eigen3 REQUIRED)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(3dsens_icp src/main.cpp)
target_include_directories(3dsens_icp PRIVATE ${EIGEN3_INCLUDE_DIR})
target_link_libraries( 3dsens_icp ${OpenCV_LIBS} )
#target_link_libraries(3dsens_icp nanoflann::nanoflann)

# READ MORE:
#   https://github.com/jlblancoc/nanoflann#11-obtaining-the-code
#   https://github.com/jlblancoc/nanoflann#17-use-in-your-project-via-cmake