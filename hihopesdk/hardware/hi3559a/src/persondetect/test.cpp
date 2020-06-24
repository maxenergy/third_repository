//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include "tool.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;
extern "C" {
#include <sys/time.h>
#include <dlfcn.h>
}


#define TEST_PIC_ID 0

typedef struct hiSAMPLE_SVP_NNIE_YOLOV2_BBOX
{
    float f32Xmin;
    float f32Xmax;
    float f32Ymin;
    float f32Ymax;
    int s32ClsScore;
    unsigned int u32ClassIdx;
    unsigned int u32Mask;
}SAMPLE_SVP_NNIE_YOLOV2_BBOX_S;

cl_platform_id platform;
cl_context context;
cl_command_queue commandQueue;
cl_device_id *devices;
cl_program program;


int count_box_size(cl_kernel *kernel,int len,SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *out_box,int flag)
{
	int count = 0;
	for(int i= 0; i< len*len*3;i++)
	{
		if((out_box[i].s32ClsScore > 0.8f*4096)&&(out_box[i].u32Mask == 0))
			count++;
	}
	printf("flag[%d]:count is %d %p \n",flag,count,kernel);
	return count;
}

//each_box_infer_num 255
void opencl_get_box(int *InputBlob,int len,int stride,float *af32Bias,SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *out_box,int class_num)
{

	int each_box_infer_num = (class_num+5) *3; //Ã¿Ò»žöÔ€²â£¬°üÀš (5 + class num ) * 3
	int input_size = (each_box_infer_num-1)*len*stride/4 + stride * (len-1)/4 + (len-1); //ÊäÈëµÄŽóÐ¡
	int NUM = each_box_infer_num * len * len; //13 x 13 * 85 ×ÜµÄÊýŸÝ³€¶È 
	int out_box_size = len * len * 3;
	float output[NUM];
    cl_int    status;
	
	SAMPLE_SVP_NNIE_YOLOV2_BBOX_S outbox_temp[out_box_size];
	
    cl_kernel kernel = clCreateKernel(program,"ReSortArry", NULL);
    cl_kernel kernel_1 = clCreateKernel(program,"GetBoxes", NULL);
	cl_kernel kernel_2 = clCreateKernel(program,"cal_iou", NULL);

	
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (input_size) * sizeof(int),(int *)InputBlob, NULL);
	cl_int cllen = len;
	cl_int clstride = stride;
	cl_int clgrid_num = 3;
	cl_int cclass_n = class_num;
	
    cl_mem outputBuffer_0 = clCreateBuffer(context, CL_MEM_WRITE_ONLY , NUM * sizeof(float), NULL, NULL);

	cl_mem clafbias = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR , 6 * sizeof(float), (float *)af32Bias, NULL);
	cl_mem outputBuffer_1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY , out_box_size * sizeof(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S), NULL, NULL);

	/**Step 9: Sets Kernel arguments.*/
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&inputBuffer);
	status = clSetKernelArg(kernel, 1, sizeof(cl_int), &cllen);
	status = clSetKernelArg(kernel, 2, sizeof(cl_int), &clstride);
	status = clSetKernelArg(kernel, 3, sizeof(cl_int), &cclass_n);
	status = clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&outputBuffer_0);

	/**Step 10: Running the kernel.*/
		 size_t global_work_size[1] = {NUM/each_box_infer_num};
		 size_t local_work_size[1] = {len};
		 cl_event enentPoint;
		 status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, &enentPoint);
		 clWaitForEvents(1,&enentPoint); ///wait
		 clReleaseEvent(enentPoint);
		 /**Step 11: Read the cout put back to host memory.*/
		 status = clEnqueueReadBuffer(commandQueue, outputBuffer_0, CL_TRUE, 0, NUM * sizeof(float), output, 0, NULL, NULL);
		 
		 cl_mem inputBuffer_1 = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR , NUM * sizeof(float), (float *)output, NULL);
	
		 /**Step 9: Sets Kernel arguments.*/
		 status = clSetKernelArg(kernel_1, 0, sizeof(cl_mem), (void *)&inputBuffer_1);
		 status = clSetKernelArg(kernel_1, 1, sizeof(cl_int), &cllen);
		 status = clSetKernelArg(kernel_1, 2, sizeof(cl_int), &clgrid_num);
		 status = clSetKernelArg(kernel_1, 3, sizeof(cl_mem), (void *)&clafbias);
		 status = clSetKernelArg(kernel_1, 4, sizeof(cl_int), &cclass_n);
		 status = clSetKernelArg(kernel_1, 5, sizeof(cl_mem), (void *)&outputBuffer_1);
		 
		 status = clEnqueueNDRangeKernel(commandQueue, kernel_1, 1, NULL, global_work_size, local_work_size, 0, NULL, &enentPoint);
		 clWaitForEvents(1,&enentPoint); ///wait
		 clReleaseEvent(enentPoint);
		 
		 status = clEnqueueReadBuffer(commandQueue, outputBuffer_1, CL_TRUE, 0, out_box_size * sizeof(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S), outbox_temp, 0, NULL, NULL);
		 cl_mem inputBuffer_2 = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR , out_box_size * sizeof(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S), (SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *)outbox_temp, NULL);
		 cl_mem outputBuffer_2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY , out_box_size * sizeof(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S), NULL, NULL);

		 /**Step 9: Sets Kernel arguments.*/
		 status = clSetKernelArg(kernel_2, 0, sizeof(cl_mem), (void *)&inputBuffer_2);
		 status = clSetKernelArg(kernel_2, 1, sizeof(cl_int), &cllen);
		 status = clSetKernelArg(kernel_2, 2, sizeof(cl_int), &clgrid_num);
		 status = clSetKernelArg(kernel_2, 3, sizeof(cl_mem), (void *)&outputBuffer_2);
		 
		 global_work_size[0] = len;
		 local_work_size[0] = len;

		 status = clEnqueueNDRangeKernel(commandQueue, kernel_2, 1, NULL, global_work_size, local_work_size, 0, NULL, &enentPoint);
		 clWaitForEvents(1,&enentPoint); ///wait
		 clReleaseEvent(enentPoint);

		 status = clEnqueueReadBuffer(commandQueue, outputBuffer_2, CL_TRUE, 0, out_box_size * sizeof(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S), out_box, 0, NULL, NULL);

		 /**Step 12: Clean the resources.*/
		 status = clReleaseKernel(kernel);//*Release kernel.
		 status = clReleaseKernel(kernel_1);//*Release kernel.
		 status = clReleaseKernel(kernel_2);//*Release kernel.
		 status = clReleaseMemObject(inputBuffer);//Release mem object.
		 status = clReleaseMemObject(inputBuffer_1);//Release mem object.
		 status = clReleaseMemObject(outputBuffer_0);
		 status = clReleaseMemObject(outputBuffer_1);
		 status = clReleaseMemObject(inputBuffer_2);
		 status = clReleaseMemObject(outputBuffer_2);
		 status = clReleaseMemObject(clafbias);
}

void opencl_init(void)
{
		cl_int	  status;
	   /**Step 1: Getting platforms and choose an available one(first).*/
	   getPlatform(platform);
	
	   /**Step 2:Query the platform and choose the first GPU device if has one.*/
	   devices=getCl_device_id(platform);
	   /**Step 3: Create context.*/
	   context = clCreateContext(NULL,1, devices,NULL,NULL,NULL);
	
	   /**Step 4: Creating command queue associate with the context.*/
	   commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);
	
	   /**Step 5: Create program object */
	   const char *filename = "resort.cl";
	   string sourceStr;
	   status = convertToString(filename, sourceStr);
	   const char *source = sourceStr.c_str();
	   size_t sourceSize[] = {strlen(source)};
	   program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
	
	   /**Step 6: Build program. */
	   status=clBuildProgram(program, 1,devices,NULL,NULL,NULL);
	
	   printf("build status %d \n",status);
	   if(status)
	   {
		   size_t len;
		   char buffer[8 * 1024];
	
		   printf("Error: Failed to build program executable!\n");
		   clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		   printf("%s\n", buffer);
	   }
	   
	cl_kernel kernel = clCreateKernel(program,"ReSortArry", NULL);
	cl_kernel kernel_1 = clCreateKernel(program,"GetBoxes", NULL);
	cl_kernel kernel_2 = clCreateKernel(program,"cal_iou", NULL);

	status = clReleaseKernel(kernel);//*Release kernel.
	status = clReleaseKernel(kernel_1);//*Release kernel.	
	status = clReleaseKernel(kernel_2);//*Release kernel.
}


//typedef cl_int (*clreleaseprogram)(cl_program program); 
//typedef cl_int (*clreleasecommandqueue)(cl_command_queue commandQueue); 
//typedef cl_int (*clreleasecontext)(cl_context context); 
//typedef cl_int (*clreleasekernel)(cl_kernel kernel);
//typedef cl_kernel (*clcreatekernel)(cl_program  program,const char *kernel_name,cl_int *errcode_ret);

void opencl_deinit()
{
	cl_int	status;
	status = clReleaseProgram(program);    //Release the program object.
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
	status = clReleaseContext(context);//Release context.
    if (devices != NULL)
    {
        free(devices);
        devices = NULL;
    }
}


unsigned int get_yolo_reslut(int **ret_buf,unsigned int *len,unsigned int *stride,float *af32Bias,unsigned int grid_num,SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *out,int class_num)
{
	struct timeval start_time;
	struct timeval stop_time;
	unsigned int box_count =0;
	int max_box_size =0;	
	for(int i =0;i<grid_num;i++)
	{
 		max_box_size += len[i]*len[i]*3;
	}
	SAMPLE_SVP_NNIE_YOLOV2_BBOX_S h_out_box[max_box_size];
	SAMPLE_SVP_NNIE_YOLOV2_BBOX_S *out_box[grid_num];

	for(int i =0;i<grid_num;i++)
	{
		if(i == 0)
			out_box[i]= h_out_box;
		else
			out_box[i]= &h_out_box[len[i-1]*len[i-1]*3];
	}
	for(int i =0;i<grid_num;i++)
	{
		opencl_get_box(ret_buf[i],len[i],stride[i],&af32Bias[i*6],out_box[i],class_num);
	}
	memset(out,0,max_box_size*sizeof(SAMPLE_SVP_NNIE_YOLOV2_BBOX_S));



	int list_pic_id_flag =0;
	for(int i=0;i<max_box_size;i++)
	{
            if((h_out_box[i].s32ClsScore > 0.5f*4096)&&(h_out_box[i].u32Mask == 0))
			{
				out[box_count].f32Xmax = h_out_box[i].f32Xmax;
				out[box_count].f32Ymax = h_out_box[i].f32Ymax;
				out[box_count].f32Xmin = h_out_box[i].f32Xmin;
				out[box_count].f32Ymin = h_out_box[i].f32Ymin;
				out[box_count].s32ClsScore = h_out_box[i].s32ClsScore;
				out[box_count].u32ClassIdx = h_out_box[i].u32ClassIdx;
				out[box_count].u32Mask = h_out_box[i].u32Mask;
				box_count++;
			}
	}
	return box_count;
}


