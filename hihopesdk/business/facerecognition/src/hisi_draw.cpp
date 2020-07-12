extern "C" {
#include <unistd.h>  
#include <stdio.h>  
#include <fcntl.h>  
#include <linux/fb.h>  
#include <sys/mman.h>  
#include <stdlib.h>  
#include <string.h>
#include <sys/ioctl.h>
}
#include "facedetect.h"
#include <opencv2/opencv.hpp>

#define DIS_STAR_Y 150
#define DIS_END_Y  1000
static char* fb_addr=0;
static int fp=0;
static struct fb_var_screeninfo vinfo;
static unsigned int screensize;
std::list<MtcnnInterface::Out> g_box_pipe;
pthread_mutex_t mutex_box;
pthread_cond_t cond_box;
unsigned char *back_surface;
cv::Mat cv_back_image;
void draw_blank(void);
#define REC_LINE_LENTH 30
#define REC_LINE_WIDTH 3
#define PIC_WIDTH 800.f
#define PIC_HEIGHT 1280.f

void draw_box_thread(int arg)
{
	cv::Mat cvback_image;
	while(1){
		pthread_mutex_lock(&mutex_box);
		if(g_box_pipe.size()>0){
			memset(back_surface,0x00,screensize);
			cvback_image = cv::Mat(1280, 800, CV_8UC4, back_surface, 0);
            MtcnnInterface::Out boxlist(g_box_pipe.front());
			for (MtcnnOut box : boxlist.mOutList) {
				#if 0
				int x_start = box.mRect.tl().x*800.f/1080.f;
				int y_start = box.mRect.tl().y*1280.f/1920.f;
				
				int x_end = box.mRect.br().x*800.f/1080.f;
				int y_end = box.mRect.br().y*1280.f/1920.f;
				#else

				int x_start = box.mRect.tl().x;
				int y_start = box.mRect.tl().y;
				int x_end = box.mRect.br().x;
				int y_end = box.mRect.br().y;
				
				#endif
				CvPoint p0 =  cvPoint(x_start, y_start);
				CvPoint p1 =  cvPoint(x_end, y_start);
				CvPoint p2 =  cvPoint(x_end, y_end);
				CvPoint p3 =  cvPoint(x_start, y_end);

				CvScalar draw_color = cv::Scalar(255, 255, 255,255);
				
                //cv::rectangle(cvback_image,cvPoint(box.mRect.tl().x*800.f/1080.f,box.mRect.tl().y*1280.f/1920.f), \
				//	cvPoint(box.mRect.br().x*800.f/1080.f,box.mRect.br().y*1280.f/1920.f), cv::Scalar(255,255, 255,255));
				
				cv::line(cvback_image, p0, cvPoint(x_start, y_start + REC_LINE_LENTH), draw_color, REC_LINE_WIDTH);
				cv::line(cvback_image, p0, cvPoint(x_start+REC_LINE_LENTH, y_start), draw_color, REC_LINE_WIDTH);


				cv::line(cvback_image, p1, cvPoint(x_end, y_start + REC_LINE_LENTH), draw_color, REC_LINE_WIDTH);
				cv::line(cvback_image, cvPoint(x_end - REC_LINE_LENTH, y_start), p1, cv::Scalar(255, 255, 255,255), REC_LINE_WIDTH);


				cv::line(cvback_image, cvPoint(x_end, y_end - REC_LINE_LENTH), p2, draw_color, REC_LINE_WIDTH);
				cv::line(cvback_image, cvPoint(x_end - REC_LINE_LENTH, y_end), p2, draw_color, REC_LINE_WIDTH);


				cv::line(cvback_image, cvPoint(x_start, y_end - REC_LINE_LENTH), p3, draw_color, REC_LINE_WIDTH);
				cv::line(cvback_image, p3, cvPoint(x_start + REC_LINE_LENTH, y_end), draw_color, REC_LINE_WIDTH);

				
				std::string text(box.mUserName.c_str());
				if(box.mUserID != -1)
                cv::putText(cvback_image, cv::String(box.mUserName.c_str()), cvPoint(x_start,y_start+20), cv::FONT_HERSHEY_COMPLEX, 1, draw_color, 1, 8, 0);
				}
			g_box_pipe.pop_front();
			memcpy(fb_addr+800*DIS_STAR_Y*4,back_surface+800*DIS_STAR_Y*4,800*(DIS_END_Y-DIS_STAR_Y)*4);
			ioctl(fp, FBIOPAN_DISPLAY, &vinfo);
		}else{
			//draw_blank();
			pthread_cond_wait( &cond_box, &mutex_box);
		}
		pthread_mutex_unlock(&mutex_box);
	}
}

int init_face_rect ()   
{  
    fp = open("/dev/fb0", O_RDWR);
    if(fp < 0) {  
        printf("Error : Can not open framebuffer device/n");  
        return 0;
    }  
    if(ioctl(fp, FBIOGET_VSCREENINFO, &vinfo)){  
        printf("Error reading variable information/n");  
        return 0;
    }
	 screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;  
	 fb_addr =(char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fp,0); 
    if ((int)fb_addr == -1)
    {    
        printf ("Error: failed to map framebuffer device to memory./n");  
        return 0;
    }
	back_surface = (unsigned char*)malloc(screensize);
	memset(back_surface,0x00,screensize);
	//memset(back_surface+800*DIS_STAR_Y*4,0x00,800*(DIS_END_Y-DIS_STAR_Y)*4);
	
	//cv_back_image = cv::Mat(1280, 800, CV_8UC4, back_surface, 0);
	pthread_cond_init(&cond_box, NULL);
	pthread_mutex_init(&mutex_box, NULL );
	std::thread t0(draw_box_thread,0);	
	t0.detach();
    return 0;
}

void draw_box(MtcnnInterface::Out detect_box)
{
	g_box_pipe.push_back(detect_box);
	pthread_cond_signal(&cond_box);
}


void draw_blank(void)
{
   memset(fb_addr+800*DIS_STAR_Y*4,0x00,800*(DIS_END_Y-DIS_STAR_Y)*4);
   ioctl(fp, FBIOPAN_DISPLAY, &vinfo);
}

void close_dev()
{
    munmap(fb_addr, screensize);
    close (fp);
}
