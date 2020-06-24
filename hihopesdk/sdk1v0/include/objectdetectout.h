#ifndef OBJECTDETECTOUT_H
#define OBJECTDETECTOUT_H

#include <string>
#include <sstream>
#include <vector>

class ObjectDetectOut {
public:
    char  mName[20];
    int mBox[4];
    float mScore;
	int obj_id;

    std::string toString() {
        std::stringstream ss;
        ss << "{";
        ss << "name:" << mName << ", " ;
        ss << "score:" << mScore << ", " ;
        ss << "x0:" << mBox[0] << ", " ;
        ss << "x1:" << mBox[1] << ", " ;
        ss << "y0:" << mBox[2] << ", " ;
        ss << "y1:" << mBox[3] << "]" ;
        return ss.str();
    }
};

typedef std::vector<ObjectDetectOut> ObjectDetectOutList;

#endif // OBJECTDETECTOUT_H
