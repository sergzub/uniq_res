#!/bin/bash -ue

BUILD_DIR='bld'

mkdir "${BUILD_DIR}" 2>/dev/null || true
(cd bld && cmake ..)
cmake --build "${BUILD_DIR}"
