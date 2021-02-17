#!/usr/bin/env bash

# Load variables
source envsetup.sh
source patches_list.sh

# cmake minimum version
CMAKE_MINIMUM_MAJOR_VERSION=3
CMAKE_MINIMUM_MINOR_VERSION=8
CMAKE_URL="https://github.com/Kitware/CMake/releases/download/v3.16.0/cmake-3.16.0-Linux-x86_64.tar.gz"
CMAKE_FILE_DL_NAME=$(basename ${CMAKE_URL})
CMAKE_FOLDER_NAME="${CMAKE_FILE_DL_NAME%.tar.gz}"

# necessary packages
DPKG_PACKAGES="git wget g++ openjdk-8-jdk libudev-dev"

if $BUILD_OLD_CPU
then
    echo "BUILD_OLD_CPU=$BUILD_OLD_CPU"
    echo "Warning: Using build for OLD CPU"
    OPENCV_BASELINE_BUILD="-DCPU_BASELINE=SSE2"
else
    OPENCV_BASELINE_BUILD=""
fi


set -e
# Any subsequent(*) commands which fail will cause the shell script to exit immediately

cd ${BASE_PATH}

# checking if cmake exists and version
if (cmake > /dev/null 2>&1); then
    OLD_IFS=${IFS}
    IFS='.' # dot as delimiter
    version_string=$(cmake --version | grep version | awk '{print $3}')
    read -ra version_array <<< "$version_string" # version_string is read into an array as tokens separated by IFS
    major=${version_array[0]}
    minor=${version_array[1]}
    release=${version_array[2]}
    if [ "${major}" -lt ${CMAKE_MINIMUM_MAJOR_VERSION} ] || [ "${major}" -eq "${CMAKE_MINIMUM_MAJOR_VERSION}" -a "${minor}" -lt "${CMAKE_MINIMUM_MINOR_VERSION}" ]; then
        USE_LOCAL_CMAKE=true
    else
        USE_LOCAL_CMAKE=false
    fi
    IFS=${OLD_IFS} # reseting delimiter
else
    USE_LOCAL_CMAKE=true
fi

# install cmake if necessary
if $USE_LOCAL_CMAKE
then
    if [ ! -d "${DEPENDENCIES_PATH}/${CMAKE_FOLDER_NAME}" ]; then
        mkdir -p ${DEPENDENCIES_PATH}
        if [ ! -f  "${DOWNLOADS_PATH}/${CMAKE_FILE_DL_NAME}" ]; then
            mkdir -p ${DOWNLOADS_PATH}
            wget ${CMAKE_URL} -P ${DOWNLOADS_PATH}
        fi
    fi
    if [ ! -d "${DEPENDENCIES_PATH}/${CMAKE_FOLDER_NAME}" ]; then
        tar xvf ${DOWNLOADS_PATH}/${CMAKE_FILE_DL_NAME} -C ${DEPENDENCIES_PATH}
    fi
    if [[ ! "${DEPENDENCIES_PATH}/${CMAKE_FOLDER_NAME}/bin" == *"$PATH"* ]]; then
        APPEND_PATH="${DEPENDENCIES_PATH}/${CMAKE_FOLDER_NAME}/bin"
        OLD_APPEND_PATH=$(sed -n -e "s/^.*APPEND_PATH=//p" envsetup.sh | sed -e 's/^"//' -e 's/"$//')
        sed -i "/APPEND_PATH=/ c APPEND_PATH=\"${APPEND_PATH}:${OLD_APPEND_PATH}\"" appendpath.sh
        source appendpath.sh
    fi
fi


# install necessary dpkg packages
for pkg in $DPKG_PACKAGES; do
    if ! (dpkg --get-selections | grep -q "^$pkg[:[:space:]*].*[[:space:]]*install$" >/dev/null); then
        if sudo apt-get -qq --yes install $pkg; then
            echo "Successfully installed $pkg"
        else
            echo "Error installing $pkg"
            exit 1
        fi
    fi
done

# adding append.sh to -assume-unchanged so git does not track differences
git update-index --assume-unchanged appendpath.sh

# Clone opencv and contrib
# OPenCV Library preparation
if [[ ! -d "${OPENCV_PATH}" ]]
then
    git clone -b 4.5.1 https://github.com/opencv/opencv.git ${OPENCV_PATH} || (rm -rf ${OPENCV_PATH} && exit 1) 
fi
if [[ ! -d "${OPENCVCONTRIB_PATH}" ]]
then
    git clone -b 4.5.1 https://github.com/opencv/opencv_contrib.git ${OPENCVCONTRIB_PATH} || (rm -rf ${OPENCVCONTRIB_PATH} && exit 1) 
fi

cd ${OPENCV_PATH}

mkdir -p build
mkdir -p ${OPENCV_INSTALL_PATH}

cd build
cmake -DOPENCV_EXTRA_MODULES_PATH=${OPENCVCONTRIB_PATH}/modules -DOpenCV_SHARED=OFF -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DOPENCV_FORCE_3RDPARTY_BUILD=ON -DWITH_1394=OFF -DWITH_VTK=OFF -DWITH_GTK=OFF -DWITH_NGRAPH=OFF -DWITH_CUDA=ON -DWITH_V4L=OFF -DWITH_PROTOBUF=OFF -DWITH_IMGCODEC_HDR=OFF -DWITH_IMGCODEC_SUNRASTER=OFF -DWITH_IMGCODEC_PXM=OFF -DWITH_IMGCODEC_PFM=OFF -DWITH_FFMPEG=OFF -DWITH_ITT=OFF -DWITH_GSTREAMER=OFF -DCMAKE_INSTALL_PREFIX=${OPENCV_INSTALL_PATH} -DOPENCV_GENERATE_PKGCONFIG=YES ${OPENCV_BASELINE_BUILD} .. || echo 'failed'
make -j7
make install 

# Darknet Library preparation
if [[ ! -d "${DARKNET_PATH}" ]]
then
    git clone https://bitbucket.org/vizentecpdi/darknet ${DARKNET_PATH} || $(rm -rf ${DARKNET_PATH} && exit 1)
    cd ${DARKNET_PATH}
    git checkout -f vizentec # Darknet Vizentec Branch
    APPLY_PATCH=True
    cd ${PARENT_PATH}
fi

cd ${DARKNET_PATH}

if [ $APPLY_PATCH ]
then
    git reset --hard
    for patch in ${DARKNET_PATCHES}
    do
        for file in ${DARKNET_PATCHES_PATH}/*.patch
        do
          echo $patch
          if [ "${patch}" = $(basename "${file}" ".patch") ]
          then
              echo Applying patch ${file}
              git apply -v --reject --ignore-space-change --ignore-whitespace ${file}
              break
          fi
        done
    done
fi

cd ${DARKNET_PATH}

mkdir -p share/darknet

# RELEASE
mkdir -p build_release
cd build_release
cmake .. -DCMAKE_BUILD_TYPE=Release -DENABLE_CUDA=OFF -DOpenCV_DIR="${OPENCV_INSTALL_PATH}/lib/cmake/opencv4" -DBUILD_SHARED_LIBS=OFF -DOpenCV_SHARED=OFF
cmake --build . --target install -- -j${number_of_build_workers}
#cmake --build . --target install --parallel ${number_of_build_workers}  #valid only for CMake 3.12+
rm -f DarknetConfig.cmake
rm -f DarknetConfigVersion.cmake
cd ..
cp cmake/Modules/*.cmake share/darknet/


echo ""
echo "-----------------------------------------"
echo "----------------OK--------------------"
echo "-----------------------------------------"
echo ""
