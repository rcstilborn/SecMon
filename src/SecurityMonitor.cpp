/*
 * SceneMonitor.h
 *
 *  Created on: Jul 27, 2015
 *      Author: richard
 *
 *  Copyright 2017 Richard Stilborn
 *  Licensed under the MIT License
 */

#include <glog/logging.h>
#include <gflags/gflags.h>
#include <boost/asio/io_service.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread/thread.hpp>

#ifndef CPU_ONLY
#include <opencv2/core/cuda.hpp>
#endif

#include <termios.h>
#include <unistd.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "GUI/GUI_Interface.h"
#include "PerformanceMetrics.h"
#include "SceneMonitor.h"

DEFINE_string(camera1, "", "Source for camera 1 stream (file or ip address)");
// This is needed because of compiler warning for release builds
#ifdef DEBUG
static bool ValidateCamera(const char* flagname, const std::string& value) {
  if (value != "")   // value is ok
  return true;
  std::cout << "Invalid value for --" << flagname << ": " << value << std::endl;
  return false;
}
DEFINE_validator(camera1, &ValidateCamera);
#endif // DEBUG

DEFINE_string(camera1_name, "", "Display name for for camera 1");

DEFINE_double(realtime_factor, 1.0, "Factor of real-time to operate at.  1.0 means realtime, 0.5 means half speed");
// This is needed because of compiler warning for release builds
#ifdef DEBUG
static bool ValidateRealtimeFactor(const char* flagname, const double value) {
  if (0.0 < value && value <= 1.0) return true;  // value is ok
  std::cout << "Invalid value for --" << flagname << ": " << value << std::endl;
  return false;
}
DEFINE_validator(realtime_factor, &ValidateRealtimeFactor);
#endif // DEBUG

/*

 HD Video Sources
 ../TrainingData/PL_Pizza/%04d.jpg
 ../TrainingData/PNNL_Parking_Lot/PNNLParkingLot1.avi
 ../TrainingData/PNNL_Parking_Lot/PNNLParkingLot2.avi
 ../TrainingData/TownCentre/TownCentre.avi


 Camera Stream Examples
 http://192.168.1.11/videostream.cgi?user=foo&pwd=&resolution=640*480&rate=30
 http://hal:hal@192.168.1.67:80/mjpeg.cgi?user=hal&password=hal&.mjpg


 Old data
 "/home/richard/Dev/TheMachine/TrainingData/ManInRoom.mpg",
 "../TrainingData/CourtyardWithSun.mpg",
 "/home/richard/Dev/TheMachine/TrainingData/ParkingLotWithBusyBackground.avi",
 "/home/richard/Dev/TheMachine/TrainingData/MunicipalDump.avi",
 "/home/richard/Dev/TheMachine/TrainingData/Downtown Traffic.avi",
 "/home/richard/Dev/TheMachine/TrainingData/2014_Dataset/dataset/outdoors-dynamic/highway/input/in%06d.jpg",
 "/home/richard/Dev/TheMachine/TrainingData/2014_Dataset/dataset/outdoors-dynamic/tramstop/input/in%06d.jpg"

 */

const int kNumThreadsInPool = 8;

//void doMenu();
char getch();

int main(int argc, char * argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  gflags::SetUsageMessage(argv[0] + ':');
  gflags::SetVersionString("0.1");

#ifdef DEBUG
  FLAGS_log_dir = "./";
  FLAGS_logtostderr = 1;
  FLAGS_stderrthreshold = 1;
#endif // DEBUG

  ::google::InitGoogleLogging("SecMon");
  ::google::InstallFailureSignalHandler();

  std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

#ifndef CPU_ONLY
  std::cout << "GPUs found: " << cv::cuda::getCudaEnabledDeviceCount() << std::endl;
  cv::cuda::printShortCudaDeviceInfo(cv::cuda::getDevice());
#endif

  std::cout << "Starting " << argv[0] << std::endl;

  // Create the asio service
  boost::asio::io_service io_service;

  boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
  signals.async_wait(boost::bind(&boost::asio::io_service::stop, &io_service));

  // Create the threadpool
  boost::thread_group threadpool;

  try {
    // GUI interface
    GUI_Interface gui(io_service);

    // Main component
    SceneMonitor sceneMonitor(io_service, gui);

    // Create the first scene
    sceneMonitor.start_monitoring(FLAGS_camera1_name, FLAGS_camera1, FLAGS_realtime_factor);

    // Start the threadpool
    for (int i = 0; i < kNumThreadsInPool; i++)
      threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &io_service));

    gui.start();
    std::cout << "Press <ENTER> to exit" << std::endl << std::endl;
    while (1) {
      int ch = getch();
      // p = 112
      // Enter = 10
      if (ch == 112)  // p - pause
        sceneMonitor.toggle_pause();
      else if (ch == 115)  // s - slow
        sceneMonitor.set_realtime_factor(0.1);
      else if (ch == 110)  // n - normal
        sceneMonitor.set_realtime_factor(0.5);
      else if (ch == 102)  // f - fast
        sceneMonitor.set_realtime_factor(1.0);
      else if (ch == 10)
        break;
    }
    std::cout << "Stopping monitoring..." << std::endl;
    sceneMonitor.stop_all_monitoring();
    std::cout << "Stopping GUI Interface..." << std::endl;
    gui.shutdown();
    std::cout << "Shutting down the io service..." << std::endl;
    io_service.stop();
    std::cout << "Waiting for threads..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    threadpool.interrupt_all();
  } catch (std::exception& e) {
    std::cout << "Caught exception: " << e.what() << std::endl;
  }
  return 0;
}

// void doMenu()
// {
//    //    initscr();
//    //    cbreak();
//    //    noecho();
//    //    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
//    std::cout << "Press <ENTER> to exit";
//    //    int ch = getch();
//    std::cin.get();
// }

char getch() {
  char buf = 0;
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
  struct termios old = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
#pragma GCC diagnostic error "-Wmissing-field-initializers"
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror("tcsetattr ~ICANON");
  return (buf);
}

// a   97
// b   98
// c   99
// d  100
// e  101
// f  102
// g  103
// h  104
// i  105
// j  106
// k  107
// l  108
// m  109
// n  110
// o  111
// p  112
// q  113
// r  114
// s  115
// t  116
// u  117
// v  118
// w  119
