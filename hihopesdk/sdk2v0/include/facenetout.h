#ifndef FACENETOUT_H
#define FACENETOUT_H

#include <vector>
#include <string>
#include <sstream>

class FaceNetOut {
public:
    float mFeatureMap[512];
	void** object;
	void** object_ir;
	int ir_length;
	int mTrackID;
	float mScore;
	int tracking_flag;
    std::string toString() {
        std::stringstream ss;
        ss << "{";
        ss << "}";
        return ss.str();
    }
};

typedef std::vector<FaceNetOut> FaceNetOutList;

#endif // FACENETOUT_H
