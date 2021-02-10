#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include "vehicledetector_global.h"
#include <opencv2/opencv.hpp>
#include <sstream>
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
    std::vector<std::string> classes = {"carro",
                                        "moto",
                                        "onibus",
                                        "caminhao",
                                        "carretinha",
                                        "caminhonete",
                                        "van"};

    std::map<std::string, cv::Rect> *result;
    void initDetectionModel();
    void GetClasses();
    std::string Serialize(std::map<std::string, cv::Rect> *result);
    const char* rect_to_string(cv::Rect pts);
    network *net;
public:
    VehicleDetector(const char* cfg_path, const char* weight_path, const char* class_path);
    virtual ~VehicleDetector();
    std::string SingleDetection(cv::Mat, float thresh);
};

extern "C" {
    EXPORT VehicleDetector* C_CreateVehicleDetection(const char* cfg_path, const char* weight_path, const char* class_path);
    EXPORT char* C_SingleDetection(VehicleDetector* v, char* imgData, size_t imgSize, float thres);
}

#endif // VEHICLEDETECTOR_H
