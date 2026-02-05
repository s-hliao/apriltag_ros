cmake_minimum_required(VERSION 3.5)

project(apriltag_ros)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

option(ASAN "use AddressSanitizer to detect memory issues" OFF)

if(ASAN)
    set(ASAN_FLAGS "\
        -fsanitize=address \
        -fsanitize=bool \
        -fsanitize=bounds \
        -fsanitize=enum \
        -fsanitize=float-cast-overflow \
        -fsanitize=float-divide-by-zero \
        -fsanitize=nonnull-attribute \
        -fsanitize=returns-nonnull-attribute \
        -fsanitize=signed-integer-overflow \
        -fsanitize=undefined \
        -fsanitize=vla-bound \
        -fno-sanitize=alignment \
        -fsanitize=leak \
        -fsanitize=object-size \
    ")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ASAN_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ASAN_FLAGS}")
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(apriltag_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(image_transport REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core calib3d)
find_package(apriltag 3.2 REQUIRED)

# Foxy uses older versions, so we don't add these compile definitions
# cv_bridge < 3.3.0 in Foxy
# image_transport < 6.3 in Foxy
# tf2_ros < 0.45 in Foxy

# database of tag functions
add_library(tags STATIC
    src/tag_functions.cpp
)
target_link_libraries(tags
    apriltag::apriltag
)
set_property(TARGET tags PROPERTY
    POSITION_INDEPENDENT_CODE ON
)

# conversion functions as template specialisation
add_library(conversion STATIC
    src/conversion.cpp
)
target_link_libraries(conversion
    apriltag::apriltag
    Eigen3::Eigen
    opencv_core
    tf2::tf2
)
ament_target_dependencies(conversion
    geometry_msgs
    tf2_geometry_msgs
)
set_property(TARGET conversion PROPERTY
    POSITION_INDEPENDENT_CODE ON
)

# pose estimation methods
add_library(pose_estimation STATIC
    src/pose_estimation.cpp
)
target_link_libraries(pose_estimation
    apriltag::apriltag
    Eigen3::Eigen
    opencv_calib3d
    conversion
    tf2::tf2
)
ament_target_dependencies(pose_estimation
    geometry_msgs
    tf2_geometry_msgs
)
set_property(TARGET pose_estimation PROPERTY
    POSITION_INDEPENDENT_CODE ON
)

# composable node
add_library(AprilTagNode SHARED
    src/AprilTagNode.cpp
)
target_link_libraries(AprilTagNode
    apriltag::apriltag
    tags
    pose_estimation
)
ament_target_dependencies(AprilTagNode
    rclcpp
    rclcpp_components
    sensor_msgs
    apriltag_msgs
    cv_bridge
    image_transport
    tf2_ros
)

rclcpp_components_register_node(AprilTagNode
    PLUGIN "AprilTagNode"
    EXECUTABLE "apriltag_node"
)

install(TARGETS 
    tags
    conversion
    pose_estimation
    AprilTagNode
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY cfg
    DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()