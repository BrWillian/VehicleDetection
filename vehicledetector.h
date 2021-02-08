#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include "vehicledetector_global.h"
#include <opencv2/opencv.hpp>
#include <fstream>
#include <vector>
#include <map>
#include <string>
#define BLOCK 512

#if defined(__GNUC__)
    //  GCC
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
    #define CDECL __attribute__((cdecl))
#endif

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
    VehicleDetector(const char* cfg_path, const char* weight_path, const char* class_path);
    virtual ~VehicleDetector();
    const char* SingleDetection(cv::Mat, float thresh = 0.5);
};

extern "C" {
    EXPORT VehicleDetector* CDECL C_CreateVehicleDetection(const char* cfg_path, const char* weight_path, const char* class_path);
    EXPORT const char* CDECL C_SingleDetection(VehicleDetector* v, char* imgData, size_t imgSize, float thres = 0.5);
}

#endif // VEHICLEDETECTOR_H
