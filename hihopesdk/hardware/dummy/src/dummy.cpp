#include "dummy.h"
namespace A {
#include "pola_sdk_export.h"
};
#include <random>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <vector>

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
DummyNetImpl_IMPLEMENT(ObjectDetectDummyImpl, ObjectDetectInterface, "objectdetect");

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

bool MtcnnDummyImpl::detect(int device, Frame &frame, MtcnnInterface::Out &out) {
    A::Pola_Object *rgb_obj ,*ir_obj;
    int length,irLength;
	A::ERROR_CODE ret_code;

	//printf("mRawdata Mtcnn detect %d %d \n ", frame.mRawdata.mWidth,frame.mRawdata.mHeiht);
	if(!frame.mFrameData.empty()){
		//ret_code = A::face_detect((const char *)frame.mFrameData.data, frame.mFrameData.cols, frame.mFrameData.rows, &rgb_obj, &length);
		printf("mFrameData not support rgb data!\n ");
	}
	else if(!frame.mRawdata.empty()){
    	ret_code = A::face_detect((const char *)frame.mRawdata.mData, frame.mRawdata.mWidth, frame.mRawdata.mHeiht, &rgb_obj, &length);
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

//#pragma GCC diagnostic pop

