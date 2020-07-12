#include "dummy.h"
#ifndef BUILD_FACTORY_TEST_APP
namespace A {
#include "pola_sdk_export.h"
};
#endif
#include <random>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <sys/time.h>
#ifndef BUILD_FACTORY_TEST_APP
#ifdef ZQ_DETECT_YOLOV3
void yolv3nniealikInit();
void yolv3nniealikDetect(cv::Mat &image , ObjectDetectInterface::Out &out);
#endif
#endif

#define ADD_FACE_NO_LIVENESSCHECK

#define DummyNetImpl_IMPLEMENT(T1, T2, NAME) \
    ReflectObject_SpecRegister_IMPLEMENT(T1, AIInterface::getName("dummy" , NAME)); \
    bool T1::load(int device) { \
        std::cout << "dummy load \n"; \
        return true; \
    }

static std::default_random_engine sRandom;


//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wunused-parameter"

DummyNetImpl_IMPLEMENT(MtcnnDummyImpl, MtcnnInterface, "mtcnn");
DummyNetImpl_IMPLEMENT(FaceNeDummyImpl, FaceNetInterface, "facenet");

ReflectObject_SpecRegister_IMPLEMENT(ObjectDetectDummyImpl, AIInterface::getName("dummy" , "objectdetect")); \
bool ObjectDetectDummyImpl::load(int device) { 
	std::cout << "dummy objectdetect load \n"; 
#ifndef BUILD_FACTORY_TEST_APP
#ifdef ZQ_DETECT_YOLOV3
	yolv3nniealikInit();
#endif
#endif
	return true; 
}



#ifdef __cplusplus
extern "C" {
#endif
//extern FACE_API FaceRetCode detect(const char *image, int width, int height, FaceImageType type, FaceHandle **handle, int *length);
//FaceRetCode detect1(const char *image, int width, int height, FaceImageType type, FaceHandle **handle, int *length){
    //return detect(image,  width,  height, type, handle, length);
//}
#ifdef __cplusplus
}
#endif
/*检测过程*/
/*图片detect,track,然后出box*/
/*handle 送识别*/
/*识别检测錚pose,pose 符合条件则出结果*/
/*结果和ID对应，放入队列*/
/*track 到的id 去找已有识别结果,id 丢失从队列里面删除该识别结果*/
#ifdef BUILD_FACTORY_TEST_APP
bool MtcnnDummyImpl::detect(int device, Frame &frame, MtcnnInterface::Out &out) {
    usleep(30*1000);
	return true;
}

bool FaceNeDummyImpl::detect(int device, Frame &frame, FaceNeDummyImpl::Out &out) {	
    usleep(30*1000);
	return true;
}

bool ObjectDetectDummyImpl::detect(int device, Frame &frame, ObjectDetectInterface::Out &out) {
	usleep(30*1000);
	return true;
}


#else
bool MtcnnDummyImpl::detect(int device, Frame &frame, MtcnnInterface::Out &out) {
    A::Pola_Object *rgb_obj ,*ir_obj;
    int length,irLength;
	A::ERROR_CODE ret_code;

	if(!frame.mFrameData.empty()){
		ret_code = A::face_detect(A::ImageType::RGB,(const char *)frame.mFrameData.data, frame.mFrameData.cols, frame.mFrameData.rows, &rgb_obj, &length);
	}
	else if(!frame.mRawdata.empty()){
		if(frame.mRawdata.mFormat == VIFrame::PixelFormat::BGR)
			ret_code = A::face_detect(A::ImageType::RGB,(const char *)frame.mRawdata.mData, frame.mRawdata.mWidth, frame.mRawdata.mHeiht, &rgb_obj, &length);
		else
			ret_code = A::face_detect(A::ImageType::YUV,(const char *)frame.mRawdata.mData, frame.mRawdata.mWidth, frame.mRawdata.mHeiht, &rgb_obj, &length);
		  //  ret_code = A::face_detect(A::ImageType::YUV,(const char *)frame.IR_mRawdata.mData, frame.IR_mRawdata.mWidth, frame.IR_mRawdata.mHeiht, &ir_obj, &irLength);
	}

	int t_id[length];
    ret_code = A::face_track(rgb_obj,length,t_id);
	
	if (ret_code == A::RET_OK){
		for (int i = 0; i < length; i++) {
			A::Facebox rect;
			ret_code = A::getFacebox(rgb_obj[i], &rect);
			MtcnnOut o;
			o.mRect = cv::Rect2f(cv::Point2f(rect.left, rect.top), cv::Point2f(rect.right, rect.bottom));
			o.mTrackID = t_id[i];
			o.object = rgb_obj;
			o.mUserID = -1;
			out.mOutList.push_back(o);
		}
	}else{
		printf("track error! \n");
	}
	return true;
}

#define GOOD_FACE_SCORE 0.5

bool FaceNeDummyImpl::detect(int device, Frame &frame, FaceNeDummyImpl::Out &out) {	
	A::ERROR_CODE ret_code;
	int face_lenth = 0;
	int i = 0;
#ifdef ADD_FACE_NO_LIVENESSCHECK
	int igrore =0;
#endif
	for (FaceNetOut face : out.mOutList) {
			face_lenth++;
        }
	
    A::Pola_Object *rgb_obj;
	
	for (i=0; i< out.mOutList.size(); i++) {
			float *feature_result = 0;
			int size;
			rgb_obj = (A::Pola_Object *)out.mOutList[i].object;
#ifdef ADD_FACE_NO_LIVENESSCHECK
			if(out.mOutList[i].tracking_flag == 0x33)
		 	{
				igrore = 1;
			}
#endif
			if(out.mOutList[i].tracking_flag != 1){
				A::Pola_Pose pose;
				ret_code = getPose(rgb_obj[i], &pose);
				if(pose.yaw<0)
					pose.yaw = 0-pose.yaw;
				
				if(pose.pitch<0)
					pose.pitch = 0-pose.pitch;
				
				if((pose.yaw < 30)||(pose.pitch < 30)){
					out.mOutList[i].mScore = 1 - pose.blur;
				}else{
					out.mOutList[i].mScore = 0.011;
				}
		    }else{
				out.mOutList[i].mScore = 0.022;
			}
			
			float live_score;
	
	#ifdef ADD_FACE_NO_LIVENESSCHECK
			if(!igrore){
				A::Check_Liveness(rgb_obj[i], &live_score);
			}
			else{
				printf("add face ,no need check livenees!\n");
				live_score = 1.0;
			}
	#else
				A::Check_Liveness(rgb_obj[i], &live_score);
	#endif

			if(live_score < 0.7)
				out.mOutList[i].mScore = 0.077;

			//printf("live_score is %f \n",live_score);
			
			if(out.mOutList[i].mScore >= GOOD_FACE_SCORE){
                ret_code = A::getFeature(rgb_obj[i], &feature_result, &size);
				if(ret_code == A::RET_OK){
					memcpy(out.mOutList[i].mFeatureMap, feature_result, size * sizeof(float));
					ret_code = A::freeFeature(feature_result);
				}else if(feature_result != 0)
				{
					printf("feature_result is not null need free!\n");
					ret_code = A::freeFeature(feature_result);
				}
			}
        }
		if(face_lenth){
			ret_code = A::freeAllFace(rgb_obj, face_lenth);
			if(ret_code != A::RET_OK)
				printf("failed to freeAllFace face_lenth %d \n",face_lenth);
		}
		
    return true;
}

//#define TIME_DEBUG 
bool ObjectDetectDummyImpl::detect(int device, Frame &frame, ObjectDetectInterface::Out &out) {
    if (frame.mRawdata.empty()) {
        return false;
    }
#ifdef TIME_DEBUG	
	struct timeval start_time;
	struct timeval stop_time;
	gettimeofday(&start_time, NULL);
#endif
#ifdef ZQ_DETECT_YOLOV3
  	cv::Mat yuvFrame = cv::Mat(frame.mRawdata.mHeiht*3/2, frame.mRawdata.mWidth, CV_8UC1, frame.mRawdata.mData);
	cv::Mat detectImage;
	cv::cvtColor(yuvFrame, detectImage, cv::COLOR_YUV420sp2BGR);
	yolv3nniealikDetect(detectImage, out);
#endif
#ifdef TIME_DEBUG
	gettimeofday(&stop_time, NULL);
	printf("cost all %ld ms \n",(stop_time.tv_sec-start_time.tv_sec)*1000+(stop_time.tv_usec-start_time.tv_usec)/1000);
#endif
    return true;
}
#endif



#if 0
bool MtcnnDummyImpl::detect(int device, Frame &frame, MtcnnInterface::Out &out) {
    A::Pola_Object *rgb_obj ,*ir_obj;
    int length,irLength;
	A::ERROR_CODE ret_code;

	if(!frame.mFrameData.empty()){
		ret_code = A::face_detect(A::ImageType::RGB,(const char *)frame.mFrameData.data, frame.mFrameData.cols, frame.mFrameData.rows, &rgb_obj, &length);
	}
	else if(!frame.mRawdata.empty()){
		if(frame.mRawdata.mFormat == VIFrame::PixelFormat::BGR)
			ret_code = A::face_detect(A::ImageType::RGB,(const char *)frame.mRawdata.mData, frame.mRawdata.mWidth, frame.mRawdata.mHeiht, &rgb_obj, &length);
		else
			ret_code = A::face_detect(A::ImageType::YUV,(const char *)frame.mRawdata.mData, frame.mRawdata.mWidth, frame.mRawdata.mHeiht, &rgb_obj, &length);
        //ret_code = A::detect((const char *)frame.IR_mRawdata.mData, frame.IR_mRawdata.mWidth, frame.IR_mRawdata.mHeiht, A::YUV, &ir_obj, &irLength);
	}
#if 0
	std::vector<int> matchPair(length, irLength);
	float score = 0;
	
	ret_code = A::BGR_IR_match(faceHandle, length, irHandle, irLength, matchPair.data());

	for(int i = 0; i < length; i++) {
		if(matchPair[i] == irLength){
			printf("[FACE_ARC_TEST] This bgrIndex=%d face handle is attack, can not pass the ir filter process!!\n", i);
		}
		else
		{
			//auto sp = cw::ProfilerFactory::get()->make("getLiveness_ir");
			ret_code = A::getLiveness_ir(faceHandle[i], irHandle[matchPair[i]],&score);
			printf("[FACE_ARC_TEST] bgrIndex=%d--score=%f--irIndex=%d\n", i, score, matchPair[i]); 
		}
		
	}
#endif
	if (ret_code == A::RET_OK) {
		for (int i = 0; i < length; i++) {
			A::Facebox rect;
            ret_code = A::getFacebox(rgb_obj[i], &rect);
			MtcnnOut o;
			
	        o.mRect = cv::Rect2f(cv::Point2f(rect.left, rect.top), cv::Point2f(rect.right, rect.bottom));
	        
            float *feature_result;
			int size;
            A::ERROR_CODE ret_code = A::getFeature(rgb_obj[i], &feature_result, &size);
			memcpy(o.faceinfo.mFeatureMap, feature_result, size * sizeof(float));
			//printf("size %d\n", size);
			//std::cout << "face detect 1 "<< o.mFeatureMap << std::endl;
			out.mOutList.push_back(o);
			ret_code = A::freeFeature(feature_result);
			
           // std::cout << "face detect 1 "<< frame.mFrameData.cols << frame.mFrameData.rows << std::endl;
          //  std::cout << "rect.left" << rect.left << "rect.top" << rect.top << "rect.right" << rect.right << "rect.bottom" << rect.bottom << std::endl;
			
        }
		ret_code = A::freeAllFace(rgb_obj, length);
		if(!frame.IR_mRawdata.empty()){
           // ret_code = A::releaseAllFace(irHandle, irLength);
		}
    }
    return true;
}


bool FaceNeDummyImpl::detect(int device, Frame &frame, FaceNeDummyImpl::Out &out) {
#if 0

    FaceHandle *faceHandle;
    int length = 0;

	std::cout << "faceNet detect 1 " << std::endl;
    FaceRetCode ret_code = A::detect((const char *)frame.mFrameData.data, frame.mFrameData.cols, frame.mFrameData.rows, BGR, &faceHandle, &length);

	if (ret_code == RET_OK) {
		for (int i = 0; i < length; i++) {
            float *feature_result;
			int size;
            FaceRetCode ret_code = A::extract(faceHandle[i], &feature_result, &size);
			printf("ret_code %d\n", size);
			#if 0
			Landmark landmark;
			ret_code = A::getFaceLandmark(faceHandle[i], &landmark);
			printf("Landmark Score:%f\n", landmark.score);
			FacePoseBlur poseBlur;
            ret_code = A::getPoseBlurAttribute(faceHandle[i], &poseBlur);
			printf("[FACEPASS_DV300_TEST] getPoseBlurAttribute:roll=%f--yaw=%f--pitch=%f--blur=%f\n", poseBlur.roll, poseBlur.yaw, poseBlur.pitch, 
			poseBlur.blur);
			#endif
			
			FaceNetOut box;
		    if (ret_code == RET_OK) {
				//memcpy(box.mFeatureMap,feature_result,sizeof(feature_result));
				printf("size %d\n", sizeof(feature_result));
		        out.mOutList.push_back(box);
		    }
            ret_code = A::releaseFeature(feature_result);
        }
		ret_code = A::releaseAllFace(faceHandle, length);
    }
#endif
    return true;
}

bool ObjectDetectDummyImpl::detect(int device, Frame &frame, ObjectDetectInterface::Out &out) {
    int counts =  sRandom() % 20;
	std::cout << "face detect 3 " << std::endl;
    for(int i = 0; i< counts; i++) {
        ObjectDetectOut item;
        sprintf(item.mName, "person");
        out.mOutList.push_back(item);
    }
    usleep(10*1000);
    return true;
}
#endif
//#pragma GCC diagnostic pop

