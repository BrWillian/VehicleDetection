#ifndef VEHICLEDETECTOR_H
#define VEHICLEDETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>

#if defined(__GNUC__)
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
    #define CDECL __attribute__((cdecl))
#endif

extern "C" {
#include </opt/darknet/include/darknet.h>
}

class VehicleDetector
{
private:
    std::string cfg_path;
    std::string weight_path;
    std::string class_path;
    std::vector<std::string> classes;

    std::map<std::string, cv::Rect> *result;
    void initDetectionModel();
    bool GetClasses();
    std::string Serialize(std::map<std::string, cv::Rect> *result);
    network *net;
public:
    VehicleDetector();
    VehicleDetector(const char* cfg_path, const char* weight_path, const char* class_path);
    virtual ~VehicleDetector();
    std::string SingleDetection(cv::Mat, float thresh);
};

extern "C" {
    EXPORT VehicleDetector* C_CreateVehicleDetectionEnv();
    EXPORT VehicleDetector* C_CreateVehicleDetection(const char* cfg_path, const char* weight_path, const char* class_path);
    EXPORT char* C_SingleDetection(VehicleDetector* v, char* imgData, size_t imgSize, float thres);
    EXPORT void C_FreeResult(char * result);
}

#endif // VEHICLEDETECTOR_H
