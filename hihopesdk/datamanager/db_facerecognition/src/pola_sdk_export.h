#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>


#define POLA_SDK

#ifdef __cplusplus
extern "C" {
#endif
//typedef void* FaceHandle;
typedef void* Pola_Object;

typedef void* Pola_Object_GP;

typedef enum ERROR_CODE {
	RET_OK = 0,
	RET_ERROR,
	RET_NO_FACE
} ERROR_CODE;

typedef enum Log_Level {
	LOG_LEVEL_ERROR = 1,
	LOG_LEVEL_INFO = 2,
} Log_Level ;

typedef enum ImageType {
	YUV = 0,
	RGB = 1,
} ImageType;

typedef struct Pola_Settins {
	const char *detect_model;
	const char *postfilter_model;
	const char *refine_model;
	const char *pose_blur_model;
	const char *stn_model;
	const char *feature_model;
	const char *liveness_model;
	const char *anchor_path;
	const char *group_model_path;
	const char *age_gender_model;
	const char *rc_model;
} Pola_Settins;

typedef struct Point {
	float x;
	float y;
} Point;

typedef struct Pola_Landmark {
	float score;
	Point points[81];
} Pola_Landmark;

typedef struct Facebox {
	int left;
	int top;
	int right;
	int bottom;
} Facebox;

typedef struct Pola_Pose {
	float roll;
	float pitch;
	float yaw;
	float blur;
} Pola_Pose;

typedef struct Pola_Human_Attr {
	float age;				// age
	float gender[2];		// ["male", "female"]

	float hair[5];			// [bald, little_hair, short_hair, long_hair, unknown]
	float hair_confidence;	//invalid param
	float beard[4]; 		// [no_beard, moustache, whisker, unknown]
	float beard_confidence; //invalid param
	float hat[9];			// [no_hat, safety_helmet, chef_hat, student_hat, helmet, taoism_hat, kerchief, others, unknown]
	float hat_confidence;	//invalid param
	float respirator[6];	// [no_respirator, surgical, anti-pollution, common, unknown]
	float respirator_confidence; //invalid param
	float glasses[5];		// [no_glasses, glasses_with_dark_frame, regular_glasses, sunglasses, unknown]
	float glass_confidence; //invalid param
	float skin_color[5];
	float skin_color_confidence;
} Pola_Human_Attr;



typedef void* Pola_Object;
typedef void* Pola_Object_GP;

//POLA_SDK ERROR_CODE detect(const char *image_buf, int width, int height, ImageType type, Pola_Object **obj, int *length);
POLA_SDK ERROR_CODE face_detect(const char *image_buf, int width, int height, Pola_Object **obj, int *length);

//POLA_SDK ERROR_CODE getFaceRect(Pola_Object handle, facebox *result);
POLA_SDK ERROR_CODE getFacebox(Pola_Object handle, Facebox *result);

//POLA_SDK ERROR_CODE extract(Pola_Object handle, float** feature_result, int *size);
POLA_SDK ERROR_CODE getFeature(Pola_Object handle, float** feature_result, int *size);

//POLA_SDK ERROR_CODE releaseFeature(float* feature);
POLA_SDK ERROR_CODE freeFeature(float* feature);

//POLA_SDK ERROR_CODE releaseAllFace(Pola_Object *handle, int size);
POLA_SDK ERROR_CODE freeAllFace(Pola_Object *handle, int size);

//POLA_SDK ERROR_CODE createFaceGroup(Pola_Object_GP *Obj_gp);
POLA_SDK ERROR_CODE makeFaceGp(Pola_Object_GP *Obj_gp);

//POLA_SDK ERROR_CODE insertFaceGroup(Pola_Object_GP Obj_gp, const float *feature, int feature_length, uint64_t face_id);
POLA_SDK ERROR_CODE GpinsertItem(Pola_Object_GP Obj_gp, const float *feature, int feature_length, uint64_t face_id);

//POLA_SDK ERROR_CODE removeFaceGroup(Pola_Object_GP Obj_gp, uint64_t face_id);
POLA_SDK ERROR_CODE GpdelItem(Pola_Object_GP Obj_gp, uint64_t face_id);

//POLA_SDK ERROR_CODE identifyFromFaceGroup(Pola_Object_GP handle, const float *target, int feature_length, float *ret, uint64_t *idx);
POLA_SDK ERROR_CODE GpqueryItem(Pola_Object_GP handle, const float *target, int feature_length, float *ret, uint64_t *idx);

//POLA_SDK ERROR_CODE getFaceGroupCount(Pola_Object_GP handle, size_t *total_sum);
POLA_SDK ERROR_CODE getGPsize(Pola_Object_GP handle, size_t *total_sum);

//POLA_SDK ERROR_CODE releaseFaceGroup(Pola_Object_GP handle);
POLA_SDK ERROR_CODE freeGp(Pola_Object_GP handle);

//POLA_SDK ERROR_CODE getLiveness(Pola_Object handle, float *liveness_result)
POLA_SDK ERROR_CODE Check_Liveness(Pola_Object handle, float *liveness_result);

// POLA_SDK ERROR_CODE BGR_IR_match(Pola_Object *handle, int length, Pola_Object *ir_handle, int ir_length, int *pair)
POLA_SDK ERROR_CODE Match_Lv(Pola_Object *handle, int length, Pola_Object *ir_handle, int ir_length, int *pair);

//POLA_SDK ERROR_CODE getLiveness_ir(Pola_Object bgr_handle, Pola_Object ir_handle, float *liveness_result);
POLA_SDK ERROR_CODE getScore(Pola_Object bgr_handle, Pola_Object ir_handle, float *liveness_result);

//POLA_SDK ERROR_CODE compare(const float *first_feature, const float *second_feature, int feature_length, float *ret);
POLA_SDK ERROR_CODE calc_Distance(const float *first_feature, const float *second_feature, int feature_length, float *ret);

//POLA_SDK ERROR_CODE getFaceAttrResult(Pola_Object handle, Pola_Human_Attr *ret);
POLA_SDK ERROR_CODE calc_Attr(Pola_Object handle, Pola_Human_Attr *ret);
#ifdef __cplusplus
}
#endif
