__kernel void Variance(__global unsigned char* in, float average,__global float* out)
{
     int n = get_global_id(0);
     int index = n*160*3;
     int i = 0;
     float sum = 0;
     for(i=0;i<160*3;i++)
     {
     		sum += pow((float)(in[index+i]) - average, 2);
     }
     out[n] = sum;
}


__kernel void Prewhiten(__global unsigned char* in, float average,float std,__global int* out)
{
     int n = get_global_id(0);
     int index = n*160*3;
     int i = 0;
     for(i=0;i<160*3;i++)
     {
     		out[index+i] = (int)(((float)in[index+i] - average)*4096/std);
     }
}


__kernel void MatchFeature(__global int* in, __global int* lib,__global float* out)
{
     int n = get_global_id(0);
     int index = n*128;
     int i = 0;
     float sum =0;
     float dis =0;
     for(i=0;i<128;i++)
     {
				dis =(float)(lib[index+i]-in[i])/4096;
				sum += dis*dis;
     }
     out[n] = sqrt(sum)/10;
}

