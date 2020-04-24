//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include "tool.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

using namespace std;

cl_platform_id platform;
cl_context context;
cl_command_queue commandQueue;
cl_device_id *devices;
cl_program program;

static const char *filename = "clkernels/facenet.cl";

float average_mean(char *x, int len)
{
    float sum = 0;
    for (int i = 0; i < len; i++) // 求和
        sum += x[i];
    return sum/len; // 得到平均值
}

float average_std(float *x, int len)
{
    float sum = 0;
    for (int i = 0; i < 160; i++) // 求和
        sum += x[i];
    return sqrt(sum/len); // 得到平均值
}

void opencl_prewhiten(char *Input,int len,int *out_buf)
{
    cl_int    status;
    float out0[len];
    float avmean=0;
    float std=0.0;
    avmean = average_mean(Input,len);

    cl_kernel kernel_prewhiten = clCreateKernel(program,"Prewhiten", NULL);

    cl_kernel kernel_variance = clCreateKernel(program,"Variance", NULL);

    cl_float cl_avmean = avmean;
    cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (len) * sizeof(char),(char *)Input, NULL);
    cl_mem outputBuffer_0 = clCreateBuffer(context, CL_MEM_WRITE_ONLY , 160 * sizeof(float), NULL, NULL);
    /**Step 9: Sets Kernel arguments.*/
    status = clSetKernelArg(kernel_variance, 0, sizeof(cl_mem), (void *)&inputBuffer);
    status = clSetKernelArg(kernel_variance, 1, sizeof(cl_float), &cl_avmean);
    status = clSetKernelArg(kernel_variance, 2, sizeof(cl_mem), (void *)&outputBuffer_0);

    /**Step 10: Running the kernel.*/
    size_t global_work_size[1] = {160};
    size_t local_work_size[1] = {160/4};
    cl_event enentPoint;
    status = clEnqueueNDRangeKernel(commandQueue, kernel_variance, 1, NULL, global_work_size, local_work_size, 0, NULL, &enentPoint);
    clWaitForEvents(1,&enentPoint); ///wait
    clReleaseEvent(enentPoint);
    /**Step 11: Read the cout put back to host memory.*/
    status = clEnqueueReadBuffer(commandQueue, outputBuffer_0, CL_TRUE, 0, 160 * sizeof(float), out0, 0, NULL, NULL);

    std = average_std(out0,len);
    cl_float cl_std = std;
    cl_mem outputBuffer_1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY , len * sizeof(int), NULL, NULL);
    /**Step 9: Sets Kernel arguments.*/
    status = clSetKernelArg(kernel_prewhiten, 0, sizeof(cl_mem), (void *)&inputBuffer);
    status = clSetKernelArg(kernel_prewhiten, 1, sizeof(cl_float), &cl_avmean);
    status = clSetKernelArg(kernel_prewhiten, 2, sizeof(cl_float), &cl_std);
    status = clSetKernelArg(kernel_prewhiten, 3, sizeof(cl_mem), (void *)&outputBuffer_1);

    status = clEnqueueNDRangeKernel(commandQueue, kernel_prewhiten, 1, NULL, global_work_size, local_work_size, 0, NULL, &enentPoint);
    clWaitForEvents(1,&enentPoint); ///wait
    clReleaseEvent(enentPoint);

    status = clEnqueueReadBuffer(commandQueue, outputBuffer_1, CL_TRUE, 0, len * sizeof(int), out_buf, 0, NULL, NULL);

    /**Step 12: Clean the resources.*/
    status = clReleaseKernel(kernel_variance);//*Release kernel.
    status = clReleaseKernel(kernel_prewhiten);//*Release kernel.
    status = clReleaseMemObject(inputBuffer);//Release mem object.
    status = clReleaseMemObject(outputBuffer_0);
    status = clReleaseMemObject(outputBuffer_1);
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

    cl_kernel kernel = clCreateKernel(program,"Prewhiten", NULL);
    cl_kernel kernel_1 = clCreateKernel(program,"Variance", NULL);
    cl_kernel kernel_2 = clCreateKernel(program,"MatchFeature", NULL);

    status = clReleaseKernel(kernel);//*Release kernel.
    status = clReleaseKernel(kernel_1);//*Release kernel.
    status = clReleaseKernel(kernel_2);//*Release kernel.
}

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

unsigned int Prewhiten(char *int_buf,unsigned int len, int *out_buf)
{
    opencl_prewhiten(int_buf,len,out_buf);
    return 0;
}

typedef struct Match_Ret_{
    float best_match;
    int base_index;
}Match_Ret;


int MatchFeature(int *idxOut, float *outDistance, int * face, int *face_lib, int lib_size)
{
    Match_Ret ret_buf = {100, 0};
    cl_int    status;
    float out0[lib_size];
    cl_kernel kernel_match = clCreateKernel(program,"MatchFeature", NULL);
    cl_mem inputface = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (128) * sizeof(int),(int *)face, NULL);
    cl_mem facelib = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_COPY_HOST_PTR, (128*lib_size) * sizeof(int),(int *)face_lib, NULL);
    cl_mem outputBuffer_0 = clCreateBuffer(context, CL_MEM_WRITE_ONLY , lib_size * sizeof(float), NULL, NULL);

    /**Step 9: Sets Kernel arguments.*/
    status = clSetKernelArg(kernel_match, 0, sizeof(cl_mem), (void *)&inputface);
    status = clSetKernelArg(kernel_match, 1, sizeof(cl_mem), (void *)&facelib);
    status = clSetKernelArg(kernel_match, 2, sizeof(cl_mem), (void *)&outputBuffer_0);

    /**Step 10: Running the kernel.*/
    size_t global_work_size[1] = {(size_t)lib_size};
    size_t local_work_size[1] = {1};
    cl_event enentPoint;
    status = clEnqueueNDRangeKernel(commandQueue, kernel_match, 1, NULL, global_work_size, local_work_size, 0, NULL, &enentPoint);
    clWaitForEvents(1,&enentPoint); ///wait
    clReleaseEvent(enentPoint);
    /**Step 11: Read the cout put back to host memory.*/
    status = clEnqueueReadBuffer(commandQueue, outputBuffer_0, CL_TRUE, 0, lib_size * sizeof(float), out0, 0, NULL, NULL);

    /**Step 12: Clean the resources.*/
    status = clReleaseKernel(kernel_match);//*Release kernel.
    status = clReleaseMemObject(inputface);//Release mem object.
    status = clReleaseMemObject(outputBuffer_0);
    status = clReleaseMemObject(facelib);
    for(int i = 0; i < lib_size; i++) {
        if(out0[i] < ret_buf.best_match){
            ret_buf.best_match = out0[i];
            ret_buf.base_index = i;
        }
    }
    *idxOut = ret_buf.base_index;
    *outDistance = ret_buf.best_match;
}


