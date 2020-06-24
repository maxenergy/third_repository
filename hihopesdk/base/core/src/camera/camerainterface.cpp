#include "camerainterface.h"

CameraInterface::~CameraInterface() {
    release();
}

bool CameraInterface::open() {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::open(int) {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::open(std::string) {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::read(cv::Mat &) {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::read(cv::Mat &, cv::Mat &) {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::read(unsigned char *frame) {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::isVaild() {
    std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
    return false;
}

bool CameraInterface::read(VIFrame &){
	std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
}

bool CameraInterface::read(VIFrame &,VIFrame &){
	std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;
}
bool CameraInterface::read(int vpss_channel,VIFrame &viFrame){

std::cout << "[Error!!!] Unsupport func. Please call from sub class" << std::endl;

}

unsigned int CameraInterface::getWidth() {
    return mWidth;
}

unsigned int CameraInterface::getHeight() {
    return mHeigth;
}

void CameraInterface::release() {

}
