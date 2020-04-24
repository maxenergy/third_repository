
#include <atomic>

#include <string>
#include <sys/time.h>
#include "mtcnn.hpp"
#include "mtcnn_utils.hpp"
#include "common.hpp"
#include "face_net.hpp"
#include "face_lib.hpp"
#define MTCNN_WIDTH  640
#define MTCNN_HEIGHT 480


#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 720

#define DETECT_SCAL 6

#define BEST_MATCH_THRESHOULD 0.9

//#define TEST_TIME
#define IPC_MODE 0
#define FRAME_INTERVAL  3
int cam_mode_facenet =1;
mtcnn* det;
struct timeval start_time_facenet;
int creat_lib_fnish =0;

pthread_mutex_t mutex[2];
pthread_cond_t cond[2];
pthread_cond_t cond_finish[2];
pthread_mutex_t mutex_finish;
float best_match_threshould = BEST_MATCH_THRESHOULD;
face_net *facenet[2];
std::vector<cv::Mat> image_list[2];

std::vector<Face_data> face_data_vector;
face_lib *flib;

int show_box_index =0;

std::vector<face_box> face_info_0;
std::vector<face_box> face_info_1;

std::vector<face_box> *face_info_show;
std::vector<face_box> *face_info_detect;

int sync_recoginze = 0;
int finish_flag[2];
int nnie_thread_init_flag[3];
std::vector<face_box> *get_detect_box()
{
	if(show_box_index == 0){
		return &face_info_1;
	}else{
		return &face_info_0;
	}
}

std::vector<face_box> *get_show_box()
{
	if(show_box_index == 0){
		return &face_info_0;
	}else{
		return &face_info_1;
	}
}

float match_feature(int * feature0,int * feature1)
{
	int i = 0;
	float sum = 0;
	for(i = 0;i<128;i++)
	{
		float dis =0;
		dis =(float)(feature0[i]-feature1[i])/4096;
		sum = sum + dis*dis;
	}
	return sqrt(sum)/10;
}

int find_id(int * feature,face_net *facenet_t)
{
	int size = face_data_vector.size();
	int lib_data[size*128];
	int i = 0;
	float distance=0;
	Match_Ret ret_buf;
	for(i =0;i<size;i++)
	{
		memcpy((int *)(lib_data+i*128),face_data_vector[i].fearure_map,128*sizeof(int));
	}
    facenet_t->Match_Feature(feature, lib_data, size,&ret_buf);
	if(ret_buf.best_match < best_match_threshould)
		return ret_buf.base_index;

	return -1;
}

//#define CREAT_LIB

#define PRINT_TIME_NORET(func,index) do{\
\
	struct timeval st0_time##index;	\		
	struct timeval st1_time##index; \
	gettimeofday(&st0_time##index, NULL); \
	func; \
	gettimeofday(&st1_time##index, NULL); \
	float mytime##index = ( float )((st1_time##index.tv_sec * 1000000 + st1_time##index.tv_usec) - (st0_time##index.tv_sec * 1000000 + st0_time##index.tv_usec)) / 1000; \
	printf("[%d] PRINT_TIME cost time %f \n",index,mytime##index); \
} while(0)



#define PRINT_TIME(value,func,index) do{\
\
	struct timeval st0_time##index;	\		
	struct timeval st1_time##index; \
	gettimeofday(&st0_time##index, NULL); \
	value = func; \
	gettimeofday(&st1_time##index, NULL); \
	float mytime##index = ( float )((st1_time##index.tv_sec * 1000000 + st1_time##index.tv_usec) - (st0_time##index.tv_sec * 1000000 + st0_time##index.tv_usec)) / 1000; \
	printf("[%d] PRINT_TIME cost time %f \n",index,mytime##index); \
} while(0)

void face_recoginze_loop(int id){
    int i = 0;
	int face_id = -1;
	int count=0;
	struct timeval stop_time;
	facenet[id] = new face_net("facenet_inst.wk",id);
     if(id ==0) {
		flib = new face_lib(facenet[0],det);
		flib->load_lib_from_file("facelib.bin", &face_data_vector);
    }
 	printf("face_recoginze_loop[%d]\n",id);
	while(1){
		pthread_mutex_lock(&mutex[id]);
		nnie_thread_init_flag[id] = 1;
		pthread_cond_wait( &cond[id], &mutex[id]);//等待事件被触发
		for(i =0;i<image_list[id].size();i++){
			cv::Mat image = image_list[id][i];
			int feature_map[128];
			facenet[id]->get_feature_map(image,feature_map);			
			face_id = find_id(feature_map,facenet[id]);
			//PRINT_TIME(face_id,find_id(feature_map,facenet[id]),1);
			face_box& box = (*face_info_detect)[i*2+id];
			box.id = face_id;
		}
		pthread_mutex_lock(&mutex_finish);
		finish_flag[id] = 1;
		if((finish_flag[0] == 1)&&(finish_flag[1] == 1))
		{
			show_box_index = (show_box_index+1)%2;
			sync_recoginze = 0;
			gettimeofday(&stop_time, NULL);
            float mytime = ( float )((stop_time.tv_sec * 1000000 + stop_time.tv_usec) - (start_time_facenet.tv_sec * 1000000 + start_time_facenet.tv_usec)) / 1000;
			printf("cost time %f ms\n",mytime);
		}
		pthread_mutex_unlock(&mutex_finish);
		pthread_mutex_unlock(&mutex[id]);
	}
}

bool check_box_vailed(face_box output_box)
{
	if((output_box.x0 >= 0)&&(output_box.x1 >= 0)&&(output_box.y0 >= 0)&&(output_box.y1 >= 0))
		if((output_box.x0 <= CAMERA_WIDTH/DETECT_SCAL)&&(output_box.x1 <= CAMERA_WIDTH/DETECT_SCAL)&&(output_box.y0 <= CAMERA_HEIGHT/DETECT_SCAL)&&(output_box.y1 <=CAMERA_HEIGHT/DETECT_SCAL))
			return true;
	return false;
}


void creat_lib_task(char *path)
{
	std::string model_dir = "models";
    int min_size = 160;
    float conf_p = 0.8;
    float conf_r = 0.8;
    float conf_o = 0.8;

    float nms_p = 0.5;
    float nms_r = 0.7;
    float nms_o = 0.7;

    det = new mtcnn(min_size, conf_p, conf_r, conf_o, nms_p, nms_r, nms_o);
    det->load_3model(model_dir);

	printf("creat_lib_task +\n");
	facenet[0] = new face_net("facenet_inst.wk",0);
	printf("creat_lib_task -\n");
	flib = new face_lib(facenet[0],det);
	flib->creat_new_lib(path, "facelib.bin");
	creat_lib_fnish = 1;
}



void add_new_face2lib(char *path,char*name)
{
	std::string model_dir = "models";
    int min_size = 160;
    float conf_p = 0.8;
    float conf_r = 0.8;
    float conf_o = 0.8;

    float nms_p = 0.5;
    float nms_r = 0.7;
    float nms_o = 0.7;

    det = new mtcnn(min_size, conf_p, conf_r, conf_o, nms_p, nms_r, nms_o);
    det->load_3model(model_dir);

	printf("add_new_face2lib +\n");
	facenet[0] = new face_net("facenet_inst.wk",0);
	printf("add_new_face2lib -\n");
	flib = new face_lib(facenet[0],det);
	flib->add_to_lib_file("facelib.bin",path,name);
	creat_lib_fnish = 1;
}



void privew_loop(int device_id)
{	
	cv::VideoCapture vcap_usb(device_id);
    cv::Mat image;
	int font_face = cv::FONT_HERSHEY_COMPLEX; 
	double font_scale = 0.8;
	int thickness = 1;
	char *buf_show;
	int nnie_index = 0;

	if(!vcap_usb.isOpened()) {  
		std::cout << "Error opening video stream or file" << std::endl;  
	}
	int test_fps=0;	
	int frame_skip=0;
	cv::namedWindow("facedetect",1);

	vcap_usb.set(CV_CAP_PROP_FRAME_WIDTH,CAMERA_WIDTH);
	vcap_usb.set(CV_CAP_PROP_FRAME_HEIGHT,CAMERA_HEIGHT);

	while(1){
			if(!vcap_usb.read(image)){
					std::cout << "USB No frame" << std::endl;  
					cv::waitKey();
				    continue;
				}
		   cv::Mat dstImage;
		   resize(image, dstImage, cv::Size(CAMERA_WIDTH/DETECT_SCAL, CAMERA_HEIGHT/DETECT_SCAL), 0, 0, 3);
		   if((sync_recoginze == 0)&&(nnie_thread_init_flag[0] == 1)&&(nnie_thread_init_flag[1] == 1))
		   {
		   		face_info_detect = get_detect_box();
				face_info_detect->clear();
				det->detect(dstImage, *face_info_detect);
				int thread_id=0;
				image_list[0].clear();
				image_list[1].clear();
				for(unsigned int i = 0; i < face_info_detect->size(); i++)
				{
					face_box& box = (*face_info_detect)[i];
					cv::Mat facenet_image;
					if(check_box_vailed(box)){
						image(cv::Rect(box.x0*DETECT_SCAL,box.y0*DETECT_SCAL,(box.x1-box.x0)*DETECT_SCAL,(box.y1-box.y0)*DETECT_SCAL)).copyTo(facenet_image);
						image_list[thread_id].push_back(facenet_image);
						thread_id = (thread_id+1)%2;
					}
				}				
                gettimeofday(&start_time_facenet, NULL);
				pthread_mutex_lock(&mutex_finish);
				if(face_info_detect->size()>0){
					finish_flag[0] = 0;
					finish_flag[1] = 0;
					pthread_cond_signal(&cond[0]);
					pthread_cond_signal(&cond[1]);					
					sync_recoginze = 1;
				}else{
					show_box_index = (show_box_index+1)%2;
				}
				pthread_mutex_unlock(&mutex_finish);
		  }
	   		face_info_show = get_show_box();
			for(unsigned int i = 0; i < face_info_show->size(); i++)
			{
				face_box& box = (*face_info_show)[i];
				cv::rectangle(image, cv::Point(box.x0*DETECT_SCAL, box.y0*DETECT_SCAL), cv::Point(box.x1*DETECT_SCAL, box.y1*DETECT_SCAL), cv::Scalar(0, 255, 0), 1);
				if((box.id>=0)||(box.id<=face_data_vector.size())){
					std::string text(face_data_vector[box.id].name);
					cv::putText(image, text, cvPoint(box.x0*DETECT_SCAL, box.y0*DETECT_SCAL-10), font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
				}
			}
	   		cv::Mat showImage;
	   		resize(image, showImage, cv::Size(1000,1000), 0, 0, 3);
	   		cv::imshow("facedetect", showImage); //若当前帧捕捉成功，显示
	   		cv::waitKey(30); //延时30毫秒
		   frame_skip++;
	}

}
#define CREAT_LIB
int facenet_test(int argc, char** argv)
{
    const std::string root_path = get_root_path();
    std::string face_net_model_dir = "facenet_inst.wk";
    std::string model_dir = "models";
	int i =0;
	int creat_new_lib=0;
	int device_id = 0;
	int list_face = 0;
	int add_new_lib = 0;
    char *lib_path;
	char *face_name;
	char *face_pic_path;
	creat_lib_fnish = 0;
	lib_path = argv[i+1];

	if(argc >1){
			for(int i = 1;i<argc;i++)
			{
				if (strcmp("-c", argv[i]) == 0){
					creat_new_lib = 1;
					lib_path = argv[i+1];
					printf("creat_new_lib from path %s\n",lib_path);
					continue;
				}
//				if (strcmp("-d", argv[i]) == 0){
//					device_id = argv[i+1];
//					printf("device_id is %s\n",device_id);
//					continue;
//				}
				if (strcmp("-add", argv[i]) == 0){
					add_new_lib = 1;
					face_name = argv[i+1];
					face_pic_path = argv[i+2];
					printf("add new face is %s\n",face_name,face_pic_path);
					continue;
				}
				if (strcmp("-th", argv[i]) == 0){
					best_match_threshould = (float)(atoi(argv[i+1]))/10;
					printf("set best match %f \n",best_match_threshould);
					continue;
				}
			}
		}
    if(init_tengine() < 0)
    {
        std::cout << " init tengine failed\n";
        return 1;
    }
    if(request_tengine_version("0.9") < 0)
    {
        std::cout << " request tengine version failed\n";
        return 1;
    }


	if(creat_new_lib){
		creat_lib_task(lib_path);
        return 1;
	}

	if(add_new_lib){
		add_new_face2lib(face_pic_path,face_name);
        return 1;
	}	

	
	for(i =0;i<2;i++){
		pthread_cond_init(&cond[i], NULL);
		pthread_cond_init(&cond_finish[i], NULL);
		pthread_mutex_init(&mutex[i], NULL );
	}
	pthread_mutex_init(&mutex_finish, NULL );


    int min_size = 160/DETECT_SCAL;
    float conf_p = 0.6;
    float conf_r = 0.7;
    float conf_o = 0.8;

    float nms_p = 0.5;
    float nms_r = 0.7;
    float nms_o = 0.7;

    det = new mtcnn(min_size, conf_p, conf_r, conf_o, nms_p, nms_r, nms_o);
    det->load_3model(model_dir);
	nnie_thread_init_flag[0] = 0;
	nnie_thread_init_flag[1] = 0;
	std::thread t0(face_recoginze_loop,0);
	t0.detach();
	std::thread t2(face_recoginze_loop,1);
	t2.detach();
	privew_loop(device_id);
    release_tengine();
    return 0;
}







/*
std::atomic_bool sRunning;

void face_recoginze_loop_xxx(int id){
    int i = 0;
    int face_id = -1;
    struct timeval stop_time;
    while(sRunning){
        pthread_mutex_lock(&mutex[id]);
        nnie_thread_init_flag[id] = 1;
        pthread_cond_wait( &cond[id], &mutex[id]); //等待事件被触发
        for(i =0;i<image_list[id].size();i++){
            cv::Mat image = image_list[id][i];
            int feature_map[128];
            facenet[id]->get_feature_map(image,feature_map);
            face_id = find_id(feature_map,facenet[id]);
            //PRINT_TIME(face_id,find_id(feature_map,facenet[id]),1);
            face_box& box = (*face_info_detect)[i*2+id];
            box.id = face_id;
        }
        pthread_mutex_lock(&mutex_finish);
        finish_flag[id] = 1;
        if((finish_flag[0] == 1)&&(finish_flag[1] == 1))
        {
            show_box_index = (show_box_index+1)%2;
            sync_recoginze = 0;
            gettimeofday(&stop_time, NULL);
            float mytime = ( float )((stop_time.tv_sec * 1000000 + stop_time.tv_usec) - (start_time_facenet.tv_sec * 1000000 + start_time_facenet.tv_usec)) / 1000;
            printf("cost time %f ms\n",mytime);
        }
        pthread_mutex_unlock(&mutex_finish);
        pthread_mutex_unlock(&mutex[id]);
    }
}

void FaceNetInit() {
    std::string face_net_model_dir = "facenet_inst.wk";
    std::string model_dir = "models";
    int min_size = 160/DETECT_SCAL;
    float conf_p = 0.6f;
    float conf_r = 0.7f;
    float conf_o = 0.8f;

    float nms_p = 0.5f;
    float nms_r = 0.7f;
    float nms_o = 0.7f;

    if(init_tengine() < 0)
    {
        std::cout << " init tengine failed\n";
        return;
    }
    if(request_tengine_version("0.9") < 0)
    {
        std::cout << " request tengine version failed\n";
        return;
    }
    det = new mtcnn(min_size, conf_p, conf_r, conf_o, nms_p, nms_r, nms_o);
    det->load_3model(model_dir);

    for (int i = 0; i < 2; i++) {
        facenet[i] = new face_net("facenet_inst.wk", i);
        printf("face_recoginze_loop[%d]\n", i);
    }
    flib = new face_lib(facenet[0], det);
}

void FaceNetInsertFaceInfoByDir(char *dir) {
    printf("FaceNetInsertFaceInfoByDir \n");
    flib->creat_new_lib(dir, "facelib.bin");
}

void FaceNetInsertFaceInfoByFile(char *file, char *name) {
    printf("FaceNetInsertFaceInfoByFile \n");
    flib->add_to_lib_file("facelib.bin", file, name);
}

void FaceNetStart() {
    flib->load_lib_from_file("facelib.bin", &face_data_vector);

    for (int i = 0; i < 2; i++) {
        pthread_cond_init(&cond[i], NULL);
        pthread_cond_init(&cond_finish[i], NULL);
        pthread_mutex_init(&mutex[i], NULL );
    }
    pthread_mutex_init(&mutex_finish, NULL);
    nnie_thread_init_flag[0] = 0;
    nnie_thread_init_flag[1] = 0;

    sRunning = true;
    std::thread t0(face_recoginze_loop_xxx,0);
    t0.detach();
    std::thread t2(face_recoginze_loop_xxx,1);
    t2.detach();
}

void FaceNetStop() {
    sRunning = true;
    nnie_thread_init_flag[0] = 0;
    nnie_thread_init_flag[1] = 0;
}

void FaceNetOutAdd(FaceNetOut &out, face_box& box) {
    FaceNetOutItem item;

    item.x0 = box.x0;
    item.y0 = box.y0;
    item.x1 = box.x1;
    item.y1 = box.y1;

    item.px0 = box.px0;
    item.py0 = box.py0;
    item.px1 = box.px1;
    item.py1 = box.py1;

    item.score = box.score;

    for (int i = 0; i < 4; i++) {
        item.regress[i] = box.regress[i];
    }

    for (int i = 0; i < 5; i++) {
        item.face_landmark_x[i] = box.landmark.x[i];
        item.face_landmark_y[i] = box.landmark.y[i];
    }

    item.identify = std::string(face_data_vector[static_cast<size_t>(box.id)].name);

    out.push_back(item);
}

void FaceNetInference(cv::Mat &image, FaceNetOut &out) {
    static int font_face = cv::FONT_HERSHEY_COMPLEX;
    static double font_scale = 0.8;
    static int thickness = 1;
    static int frame_skip=0;

    cv::Mat dstImage;
    resize(image, dstImage, cv::Size(CAMERA_WIDTH/DETECT_SCAL, CAMERA_HEIGHT/DETECT_SCAL), 0, 0, 3);
    if((sync_recoginze == 0)&&(nnie_thread_init_flag[0] == 1)&&(nnie_thread_init_flag[1] == 1))
    {
         face_info_detect = get_detect_box();
         face_info_detect->clear();
         det->detect(dstImage, *face_info_detect);
         int thread_id=0;
         image_list[0].clear();
         image_list[1].clear();
         for(unsigned int i = 0; i < face_info_detect->size(); i++)
         {
             face_box& box = (*face_info_detect)[i];
             cv::Mat facenet_image;
             if(check_box_vailed(box)){
                 image(cv::Rect(box.x0*DETECT_SCAL,box.y0*DETECT_SCAL,(box.x1-box.x0)*DETECT_SCAL,(box.y1-box.y0)*DETECT_SCAL)).copyTo(facenet_image);
                 image_list[thread_id].push_back(facenet_image);
                 thread_id = (thread_id+1)%2;
             }
         }

         gettimeofday(&start_time_facenet, NULL);
         pthread_mutex_lock(&mutex_finish);
         if(face_info_detect->size()>0){
             finish_flag[0] = 0;
             finish_flag[1] = 0;
             pthread_cond_signal(&cond[0]);
             pthread_cond_signal(&cond[1]);
             sync_recoginze = 1;
         }else{
             show_box_index = (show_box_index+1)%2;
         }
         pthread_mutex_unlock(&mutex_finish);
   }
     face_info_show = get_show_box();
     for(unsigned int i = 0; i < face_info_show->size(); i++)
     {
         face_box& box = (*face_info_show)[i];
         cv::rectangle(image, cv::Point(box.x0*DETECT_SCAL, box.y0*DETECT_SCAL), cv::Point(box.x1*DETECT_SCAL, box.y1*DETECT_SCAL), cv::Scalar(0, 255, 0), 1);
         if((box.id>=0)||(box.id<=face_data_vector.size())){
             std::string text(face_data_vector[box.id].name);
             cv::putText(image, text, cvPoint(box.x0*DETECT_SCAL, box.y0*DETECT_SCAL-10), font_face, font_scale, cv::Scalar(0, 0, 255), thickness, 8, 0);
         }

         FaceNetOutAdd(out, box);
     }
     frame_skip++;
}
*/
