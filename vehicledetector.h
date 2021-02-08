#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include "vehicledetector_global.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#define BLOCK 512
extern "C" {
#include <darknet/include/darknet.h>
}

class VEHICLEDETECTORSHARED_EXPORT VehicleDetector
{
private:
    std::string cfg_path;
    std::string weight_path;
    std::string class_path;
    std::vector<std::string> classes;
    void initDetectionModel();
    void GetClasses();
    network *net;
public:
    VehicleDetector();
    VehicleDetector(std::string cfg_path, std::string weight_path, std::string class_path);
    virtual ~VehicleDetector();
    const char* SingleDetection(cv::Mat, float thresh = 0.5);
};

extern "C" {
    VehicleDetector* C_CreateVehicleDetection();
    const char* C_SingleDetection(VehicleDetector* v, char* imgData, size_t imgSize, float thres = 0.5);
}

#endif // VEHICLEDETECTOR_H
