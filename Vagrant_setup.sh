#!/usr/bin/env bash

# Update and Upgrade system
sudo apt-get update
#DEBIAN_FRONTEND=noninteractive sudo apt-get upgrade -y

# Install developer tools
sudo apt-get install -y build-essential cmake git pkg-config

# Install tools for image processing
sudo apt-get install -y libjpeg8-dev libtiff5-dev libjasper-dev libpng12-dev

# Install tools for video processing
sudo apt-get install -y libavcodec-dev libavformat-dev libswscale-dev libv4l-dev libxvidcore-dev libx264-dev

# Install tools for gstreamer
sudo apt-get install -y gstreamer1.0* libgstreamer-plugins-base1.0-dev

# For GUI - not needed?
#sudo apt-get install -y libgtk-3-dev

# For optimization 
sudo apt-get install -y libatlas-base-dev gfortran

# Other packages needed
sudo apt-get install -y python-dev python3-dev python-numpy python3-numpy liblapacke-dev libxml2-dev libxslt-dev

# Install python 3.6
#sudo add-apt-repository ppa:jonathonf/python-3.6
#sudo apt-get update
#sudo apt-get install -y python3.6

# Install ffmpeg
sudo add-apt-repository ppa:jonathonf/ffmpeg-3
sudo apt-get update
sudo apt-get install -y ffmpeg libav-tools x264 x265


# install boost
if [ ! -d /usr/include/boost ]
then
  echo
  echo "*************************************"
  echo "Installing Boost 1.64"
  echo "*************************************"
  echo
  cd
  wget -nv -O boost_1_64_0.tar.bz2 https://sourceforge.net/projects/boost/files/boost/1.64.0/boost_1_64_0.tar.bz2 && \
  tar --bzip2 -xf boost_1_64_0.tar.bz2 && rm boost_1_64_0.tar.bz2
  cd boost_1_64_0
  export CPLUS_INCLUDE_PATH="$CPLUS_INCLUDE_PATH:/usr/include/"
  ./bootstrap.sh --prefix=/usr --with-python=python3 && ./b2 stage threading=multi link=shared
  sudo ./b2 install threading=multi link=shared
  cd
  rm -rf boost_1_64_0
else
  echo
  echo "*************************************"
  echo "Boost already installed"
  echo "*************************************"
  echo
fi

  #sed -e '/using python/ s@;@: /usr/include/python${PYTHON_VERSION/3*/${PYTHON_VERSION}m} ;@' -i bootstrap.sh
#usr/include/python3.5m/"

# Install OpenBLAS
if [ ! -f /usr/include/openblas_config.h ]
then
  echo
  echo "*************************************"
  echo "Installing OpenBLAS 0.2.20"
  echo "*************************************"
  echo
  cd
  wget -nv -O OpenBLAS-0.2.20.tar.gz http://github.com/xianyi/OpenBLAS/archive/v0.2.20.tar.gz
  tar -xf OpenBLAS-0.2.20.tar.gz
  cd OpenBLAS-0.2.20
  make
  sudo make PREFIX=/usr install
  cd
  rm -rf OpenBLAS-0.2.20
  rm OpenBLAS-0.2.20.tar.gz
else
  echo
  echo "*************************************"
  echo "OpenBLAS already installed"
  echo "*************************************"
  echo
fi

# Install OpenCV 3.3.0
if [ ! -d /usr/local/include/opencv2 ]
then
  echo
  echo "*************************************"
  echo "Installing OpenCV 3.3.0"
  echo "*************************************"
  echo

  cd
  wget -nv -O opencv-3.2.0.tar.gz https://github.com/opencv/opencv/archive/3.2.0.tar.gz \
    && tar -xf opencv-3.2.0.tar.gz && rm opencv-3.2.0.tar.gz
  wget -nv -O opencv_contrib-3.2.0.tar.gz https://github.com/opencv/opencv_contrib/archive/3.2.0.tar.gz \
    && tar -xf opencv_contrib-3.2.0.tar.gz && rm opencv_contrib-3.2.0.tar.gz
  cd opencv-3.2.0
  wget https://patch-diff.githubusercontent.com/raw/opencv/opencv/pull/7999.patch
  patch -p1 <7999.patch
  mkdir build
  cd build
  cmake -D CMAKE_BUILD_TYPE=RELEASE \
        -D CMAKE_INSTALL_PREFIX=/usr/local \
        -D INSTALL_C_EXAMPLES=ON \
        -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_OPENMP=ON -D WITH_IPP=ON -D WITH_PTHREADS_PF=ON  \
        -D CMAKE_C_FLAGS="-march=native" -D CMAKE_CXX_FLAGS="-march=native" \
        -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.2.0/modules \
        -D BUILD_EXAMPLES=ON ..

#        -D WITH_OPENNI=ON -D WITH_OPENNI2=ON
#        -D PYTHON_INCLUDE_DIRS=$HOME/anaconda3/include  -D PYTHON_LIBRARIES=$HOME/anaconda3/lib \
#        -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_NEW_PYTHON_SUPPORT=ON \
#        -D WITH_OPENGL=ON - Needs GTK and/or Qt

 # cd
 # wget -nv -O opencv-3.3.0.zip https://github.com/opencv/opencv/archive/3.3.0.zip && unzip opencv-3.3.0.zip && rm opencv-3.3.0.zip
 # wget -nv -O opencv_contrib-3.3.0.zip https://github.com/opencv/opencv_contrib/archive/3.3.0.zip && unzip opencv_contrib-3.3.0.zip && rm opencv_contrib-3.3.0.zip
 # cd opencv-3.3.0
 # mkdir build
 # cd build/
 # cmake -D CMAKE_BUILD_TYPE=RELEASE -D ENABLE_CXX11=ON \
 #       -D CMAKE_INSTALL_PREFIX=/usr/local \
 #       -D INSTALL_C_EXAMPLES=ON \
 #       -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_NEW_PYTHON_SUPPORT=ON \
 #       -D PYTHON_INCLUDE_DIRS=../../anaconda3/include  -D PYTHON_LIBRARIES=../../anaconda3/lib \
 #       -D WITH_TBB=ON -D WITH_V4L=ON -D WITH_OPENGL=ON -D WITH_OPENMP=ON -D WITH_IPP=ON -D WITH_OPENNI=ON \
 #       -D WITH_PTHREADS_PF=ON -D WITH_OPENNI2=ON \
 #       -D CMAKE_C_FLAGS="-std=c11 -march=native" -D CMAKE_CXX_FLAGS="-std=c++11 -march=native" \
 #       -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.3.0/modules \
 #       -D BUILD_EXAMPLES=ON ..

  make -j2
  sudo make install
  sudo sh -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/opencv.conf'
  sudo ldconfig
else
  echo
  echo "*************************************"
  echo "OpenCV already installed"
  echo "*************************************"
  echo
fi


# Install anaconda
if [ ! -d $HOME/anaconda3 ]
then
  echo
  echo "*************************************"
  echo "Installing Anaconda3"
  echo "*************************************"
  echo
  wget -nv https://repo.continuum.io/archive/Anaconda3-4.4.0-Linux-x86_64.sh -O ~/Anaconda3-4.4.0-Linux-x86_64.sh
  bash ~/Anaconda3-4.4.0-Linux-x86_64.sh -b
  export PATH="$HOME/anaconda3/bin:$PATH"
  echo 'export PATH="$HOME/anaconda3/bin:$PATH"' >> ~/.bashrc
  rm ~/Anaconda3-4.4.0-Linux-x86_64.sh
else
  echo
  echo "*************************************"
  echo "Anaconda3 already installed"
  echo "*************************************"
  echo
fi

