project(Computer_Vision)
cmake_minimum_required(VERSION 2.8)
find_package(OpenCV REQUIRED )
aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries( Computer_Vision  opencv_core opencv_videostab opencv_videoio opencv_video opencv_ml opencv_calib3d opencv_objdetect opencv_features2d opencv_flann opencv_imgproc opencv_imgcodecs opencv_highgui)# ${OpenCV_LIBS} )

