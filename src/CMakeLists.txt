find_library(GRPC_PROTO_LIB grpc_proto_lib)
if (NOT GRPC_PROTO_LIB)
    message(FATAL_ERROR "Not found GRPC_PROTO Library.")
endif()

add_executable(ConfigNodeTestPub "ConfigNodeTestPub.cpp")
target_link_libraries(ConfigNodeTestPub
   ${_CORE_LIBRARIES} ${GRPC_PROTO_LIB})
add_executable(ConfigNodeTestSub "ConfigNodeTestSub.cpp")
target_link_libraries(ConfigNodeTestSub
   ${_CORE_LIBRARIES} ${GRPC_PROTO_LIB})
