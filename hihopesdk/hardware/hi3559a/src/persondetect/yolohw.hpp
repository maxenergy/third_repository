#ifndef __YOLOHW_LIB__
#define __YOLOHW_LIB__
void opencl_init(void);
void opencl_deinit();
unsigned int get_yolo_reslut(int **ret_buf,unsigned int *len,unsigned int *stride,float *af32Bias,unsigned int grid_num,SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *out,int class_num);
#endif

