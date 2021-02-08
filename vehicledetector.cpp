#include "vehicledetector.h"


VehicleDetector::VehicleDetector()
{
    cfg_path = std::getenv("CFG_PATH");
    weight_path = std::getenv("WEIGHT_PATH");
    class_path = std::getenv("CLASS_PATH");
    initDetectionModel();
}
VehicleDetector::VehicleDetector(std::string cfg_path, std::string weight_path, std::string class_path)
{
    this->cfg_path = cfg_path;
    this->weight_path = weight_path;
    this->class_path = class_path;
    initDetectionModel();
}
VehicleDetector::~VehicleDetector()
{

}
void VehicleDetector::initDetectionModel()
{
    net = load_network(const_cast<char*>(cfg_path.c_str()), const_cast<char*>(weight_path.c_str()),0);
    set_batch_network(net, 1);
}
void VehicleDetector::GetClasses()
{
    std::ifstream class_file(const_cast<char*>(class_path.c_str()));
    std::string line;

    if(class_file.is_open())
    {
        while(std::getline(class_file, line))
        {
            classes.push_back(line);
        }
        class_file.close();
    }else {
        std::cout<<"Cannot load labels"<<std::endl;
        return;
    }
}
const char* VehicleDetector::SingleDetection(cv::Mat img, float thresh)
{
#ifndef NDEBUG
    assert(false == img.empty());
#endif
    std::map<std::string, std::vector<cv::Rect>> returnVal;

    //Alocar imagem
    image im = make_image(img.cols, img.rows, 3);

    for(int c = 0; c < img.channels(); c++)
    {
        for(int h = 0; h < img.rows; h++)
        {
            for(int w = 0; w < img.cols; w++)
            {
                int dst = w + img.cols * h + img.cols * img.rows * img.rows * c;
                im.data[dst] = static_cast<float>(img.ptr<unsigned char>(h)[w * img.channels() + c] / 255.0);
            }
        }
    }
    image im_sized = letterbox_image(im,net->w,net->h);


    //Detecção
    float *im_pred = im_sized.data;

    network_predict(net, im_pred);
    int boxes;

    detection *detections = get_network_boxes(net, im.w, im.h, thresh, 0.5, nullptr, 1, &boxes);
    layer layer = net->layers[net->n-1];
    do_nms_sort(detections, boxes, layer.classes, float(0.45));

    for(int i = 0; i < boxes; i++)
    {
        int _class = -1;
        for(int j = 0; j < layer.classes; j++)
        {
            if(detections[i].prob[j] > thresh && _class < 0)
            {
                _class = j;
            }
        }
        if(_class >= 0)
        {
            box box = detections[i].bbox;
            int left = (box.x - box.w / 2.0) * im.w;
            int right = (box.x + box.w / 2.0) * im.w;
            int top = (box.y - box.h / 2.0) *im.h;
            int botton = (box.y + box.h / 2.0) *im.h;

            left < 0 ? left = 0 : left;
            right > im.w - 1 ? right = im.w - 1 : right;
            top < 0 ? top = 0 : top;
            botton > im.h - 1 ? botton = im.h - 1 : botton;

            returnVal[classes[_class]].push_back(cv::Rect(cv::Point(left, top), cv::Point(right + 1, botton +1)));
        }
    }

    // liberar espaço de memoria

    free_detections(detections, boxes);
    free_image(im);
    free_image(im_sized);

    return "teste";
}
const char* C_SingleDetection(VehicleDetector* v, char* imgData, size_t imgSize, float thres)
{
    try {
        std::vector<uchar> data(imgData, imgData + imgSize);
        cv::Mat img = cv::imdecode(cv::Mat(data), -1);
        return v->SingleDetection(img, thres);
    } catch (const std::exception& e) {
        return e.what();
    }
}
