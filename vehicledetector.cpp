#include "vehicledetector.h"

/*
VehicleDetector::VehicleDetector()
{
    cfg_path = std::getenv("CFG_PATH");
    weight_path = std::getenv("WEIGHT_PATH");
    class_path = std::getenv("CLASS_PATH");
    initDetectionModel();
}*/
VehicleDetector::VehicleDetector(const char* cfg_path, const char* weight_path, const char* class_path)
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
    net = load_network_custom(const_cast<char*>(cfg_path.c_str()), const_cast<char*>(weight_path.c_str()),1, 1);
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
std::string VehicleDetector::SingleDetection(cv::Mat img, float thresh)
{
    result = new std::map<std::string, cv::Rect>();
#ifndef NDEBUG
    assert(false == img.empty());
#endif
    //Alocar imagem
    image im = make_image(img.cols, img.rows, 3);

    for(int c = 0; c < img.channels(); c++)
    {
        for(int h = 0; h < img.rows; h++)
        {
            for(int w = 0; w < img.cols; w++)
            {
                int dst = w + img.cols * h + img.cols * img.rows * c;
                im.data[dst] = static_cast<float>(img.ptr<unsigned char>(h)[w * img.channels() + c] / 255.0);
            }
        }
    }

    image sized;

    if (net->w == im.w && net->h == im.h) {
        sized = make_image(im.w, im.h, im.c);
        memcpy(sized.data, im.data, im.w*im.h*im.c * sizeof(float));
    }
    else
        sized = resize_image(im, net->w, net->h);

    layer l = net->layers[net->n - 1];

    float *im_sized = sized.data;

    network_predict(*net, im_sized);

    int boxes = 0;

    detection *detections = get_network_boxes(net, im.w, im.h, thresh, 0.5, 0, 1, &boxes, 0);

    do_nms_obj(detections, boxes, l.classes, 0.45);

    for(int i = 0; i < boxes; i++)
    {
        int _class = -1;
        for(int j = 0; j < l.classes; j++)
        {
            if(detections[i].prob[j] > thresh) if(_class < 0) _class = j;
        }
        if(_class >= 0)
        {
            box b = detections[i].bbox;
            int left = (b.x - b.w / 2.) *im.w;
            int right = (b.x + b.w / 2.) * im.w;
            int top = (b.y - b.h / 2.) * im.h;
            int botton = (b.y + b.h / 2.) * im.h;

            left < 0 ? left = 0 : left;
            right > im.w - 1 ? right = im.w - 1 : right;
            top < 0 ? top = 0 : top;
            botton > im.h - 1 ? botton = im.h - 1 : botton;

            (*result).insert(std::pair<std::string, cv::Rect>(classes[_class], cv::Rect(cv::Point(left, top), cv::Point(right+1, botton+1))));
        }
    }

    // liberar espaço de memoria
    free_detections(detections, boxes);
    free_image(im);
    free_image(sized);

    return strdup(Serialize(result).c_str());
}
std::string VehicleDetector::Serialize(std::map<std::string, cv::Rect> *result)
{
   std::stringstream ss;
   ss << "{\"detections\": [";
   for(std::map<std::string, cv::Rect>::iterator obj = result->begin(); obj!=result->end();)
   {
       ss << "{\"classe\":\""<< obj->first;
       ss<<"\",\"x\":"<<obj->second.x;
       ss<<",\"y\":"<<obj->second.y;
       ss<<",\"w\":"<<obj->second.width;
       ss<<",\"h\":"<<obj->second.height;
       if(++obj == result->end()){
           ss<<"}";
       }else {
           ss<<"},";
       }
   }
   ss << "]";
   ss << "}";


   return ss.str();
}
const char* VehicleDetector::rect_to_string(cv::Rect pts)
{
    std::string tmp = "";
    tmp = "["+std::to_string(pts.x)+","+std::to_string(pts.y)+","+std::to_string(pts.height)+","+std::to_string(pts.width)+"]";

}
char* C_SingleDetection(VehicleDetector* v, char* imgData, size_t imgSize, float thres)
{
    if(v)
    {
        std::vector<uchar> data(imgData, imgData + imgSize);
        cv::Mat img = cv::imdecode(cv::Mat(data), -1);
        return strdup(v->SingleDetection(img, thres).c_str());
    }else {
        return strdup("falhou");
    }
}
VehicleDetector* C_CreateVehicleDetection(const char* cfg_path, const char* weight_path, const char* class_path)
{
    VehicleDetector* obj;

    obj = new VehicleDetector(cfg_path, weight_path, class_path);

    if(!obj)
    {
        return nullptr;
    }

    return obj;
}
