#ifndef MTCNNOUT_H
#define MTCNNOUT_H

#include <string>
#include <sstream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "facenetout.h"


class MtcnnOut {
public:
    float x0;
    float y0;
    float x1;
    float y1;

    /* confidence score */

    /*regression scale */
    float regress[4];

    /* padding stuff*/
    float px0;
    float py0;
    float px1;
    float py1;

    int id; //for track
	FaceNetOut faceinfo;
	//float mFeatureMap[512];
    //UserInfo users;
    int mUserID;
    std::string mUserName;
	
    float mScore;
    cv::Rect2f mRect;
    std::vector<cv::Point2f> mKeyPoints;
	void **object;
	int mTrackID;
	int tracking_flag;
//    Rect<Point2F> mRect;
//    Point2F mKeyPoint[5];

//    std::string toString() {
//        std::stringstream ss;
//        ss << "Rect:" <<  mRect.toString();
//        ss << "mKeyPoint: {" <<  mRect.toString();
//        for (int i = 0; i < 5; i++) {
//            ss << mKeyPoint[i].toString();
//        }
//        ss << "}";
//        return ss.str();
//    }

};


typedef std::vector<MtcnnOut> MtcnnOutList;


#endif // MTCNNOUT_H
