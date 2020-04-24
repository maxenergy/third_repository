#include "face_net.hpp"

face_net::face_net(const     char* model_dir, int id)
{
	printf("face_net init \n");
	nnie_id = id;
	stNnieCfg.aenNnieCoreId[0]= (SVP_NNIE_ID_E)nnie_id;
	stNnieCfg.u32MaxInputNum = 1;
	stNnieCfg.u32MaxRoiNum = 0;
    NNIE_LOAD_MODEL(&stNnieCfg, &s_Model, &s_NnieParam,model_dir);
	opencl_init();
	init_flag = 1;
}

face_net::~face_net()
{
	NNIE_Relese(&s_Model,&s_NnieParam);
	opencl_deinit();
	init_flag = 0;
	printf("~face_net\n");
}

int face_net::get_feature_map(cv::Mat image_in, int * out_buf)
{
	char pix_buf[160*160*3];
	int sent2nnie_buf[160*160*3];	
	int sent2nnie_buf_old[160*160*3];
	int *buf_temp;
	nnie_out_buf buf;
	int i =0;
	cv::Mat image;
	if(!init_flag)
		return -1;
	cv::resize(image_in, image, cv::Size(160, 160), 0, 0, 3);
	TransMatToBuffer(image,pix_buf,160,160,3,1);
	//prewhiten(pix_buf,sent2nnie_buf_old,160*160*3);
	Prewhiten(pix_buf,160*160*3,sent2nnie_buf);
	NNIE_INFERENCE(sent2nnie_buf,&stNnieCfg,&s_NnieParam);
	NNIE_GET_OUT_BUF(&s_NnieParam,&buf,1);
	buf_temp = (int*)(buf.buf_addr[0]);
	for(i = 0;i<128;i++)
	{
		out_buf[i] = buf_temp[i];
	}
	return 0;
}

int face_net::TransMatToBuffer(cv::Mat mSrc, char* ppBuffer, int nWidth, int nHeight, int nBandNum, int nBPB)
{	
        std::vector<cv::Mat> channelrgb;
 		split(mSrc, channelrgb);
		
         size_t nMemSize = nWidth * nHeight * nBandNum * nBPB;
        //这样可以改变外部*pBuffer的值
         memset(ppBuffer, 0, nMemSize);
         uchar* pT = ppBuffer;
		 for(int i=nBandNum-1;i>=0;i--){
	         for (int j = 0; j < nHeight; ++j)
	         {
				unsigned char* data = channelrgb[i].ptr<unsigned char>(j);
				memcpy(pT, data, nWidth * nBPB);
				pT +=nWidth * nBPB;
			}
		 }
 
	return 0;
}

double face_net::average_mean(char *x, int len)
{
    double sum = 0;
    for (int i = 0; i < len; i++) // 求和
        sum += x[i];
    return sum/len; // 得到平均值
}

/**
 * 求方差
 */
double face_net::variance(char *x, int len)
{
    double sum=0;
    double average = average_mean(x, len);
    for (int i = 0; i < len; i++) // 求和
        sum += pow((double)x[i] - average, 2);
    return sum/len; // 得到平均值
}

/**
 * 求标准差
 */
double face_net::average_std(char *x, int len)
{
    double variance_v = variance(x, len);
    return sqrt(variance_v); // 得到标准差
}


void face_net::prewhiten(char* inbuf,int *out_buf ,int len)
{
	double mean,std;
	int i =0;
	mean = average_mean(inbuf,len);
	std = average_std(inbuf,len);
	printf("mean:%f std:%f \n",mean,std);
	for(i =0;i<len;i++)
	{
		out_buf[i] = (int)(((double)inbuf[i] - mean)*4096/std);
	}

}



int face_net::Match_Feature(int * face,int *face_lib,int lib_size,Match_Ret* out_buf)
{
	return MatchFeature(face,face_lib,lib_size,out_buf);
}



