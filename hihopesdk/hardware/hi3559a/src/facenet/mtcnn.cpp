/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * License); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * AS IS BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*双线程，gpu+cpu 双核双网络*/
/*load model 时启动多线程，每个网络两个线程，根据id 不同设置不同device,detec 时注意消息的分发，以及多线程的同步*/
/*
 * Copyright (c) 2018, Open AI Lab
 * Author: chunyinglv@openailab.com
 */
#include "mtcnn.hpp"
#include "mtcnn_utils.hpp"
#include <sys/time.h>

//#define TEST_TIME 
//#define PRINT_TIME


#define MT_PRINT_TIME(value,func,index) do{\
\
	struct timeval st0_time##index;	\		
	struct timeval st1_time##index; \
	gettimeofday(&st0_time##index, NULL); \
	value = func; \
	gettimeofday(&st1_time##index, NULL); \
	float mytime##index = ( float )((st1_time##index.tv_sec * 1000000 + st1_time##index.tv_usec) - (st0_time##index.tv_sec * 1000000 + st0_time##index.tv_usec)) / 1000; \
	printf("[%d] MT_PRINT_TIME cost time %f \n",index,mytime##index); \
} while(0)


#define MT_PRINT_TIME_NORET(func,index) do{\
\
	struct timeval st0_time##index;	\		
	struct timeval st1_time##index; \
	gettimeofday(&st0_time##index, NULL); \
	func; \
	gettimeofday(&st1_time##index, NULL); \
	float mytime##index = ( float )((st1_time##index.tv_sec * 1000000 + st1_time##index.tv_usec) - (st0_time##index.tv_sec * 1000000 + st0_time##index.tv_usec)) / 1000; \
	printf("[%d] MT_PRINT_TIME_NORET cost time %f \n",index,mytime##index); \
} while(0)


mtcnn::mtcnn(int minsize, float conf_p, float conf_r, float conf_o, float nms_p, float nms_r, float nms_o)
{
	int i = 0;
    minsize_ = minsize;
    conf_p_threshold_ = conf_p;
    conf_r_threshold_ = conf_r;
    conf_o_threshold_ = conf_o;
    nms_p_threshold_ = nms_p;
    nms_r_threshold_ = nms_r;
    nms_o_threshold_ = nms_o;
#ifdef MULTI_THREAD
	for(i =0;i<2;i++){
		pthread_cond_init(&pnet_cond[i], NULL);
		pthread_mutex_init(&pnet_mutex[i], NULL);
	}
	pthread_mutex_init(&mutex_sync, NULL );
	pthread_mutex_init(&mutex_rnet_trigger, NULL );
	pthread_cond_init(&pnet_finish, NULL);
#endif
}

#ifdef MULTI_THREAD
void PNet_graph_loop(Thread_data *thread_data)
{
	std::string proto_name, mdl_name;
	proto_name = "models/det1.prototxt";
	mdl_name = "models/det1.caffemodel";
	mtcnn *mtcnn_obj = (mtcnn*)(thread_data->mtobj);
	int id = thread_data->id;
	//graph_t pnet_graph;
    mtcnn_obj->pnet_graph[id] = create_graph(nullptr, "caffe", proto_name.c_str(), mdl_name.c_str());
    if(mtcnn_obj->pnet_graph[id] == nullptr)
    {
        std::cout << "Create Pnet Graph failed\n";
        std::cout << "errno: " << get_tengine_errno() << "\n";
        return 1;
    }
	if(id)
		set_graph_device(mtcnn_obj->pnet_graph[id], "CPU");
	else
		set_graph_device(mtcnn_obj->pnet_graph[id], "GPU");
	
	mtcnn_obj->pnet_init_flag[id] = 1;	
	printf("PNet_graph_loop start! %d\n",id);
	while(1){
		pthread_mutex_lock(&(mtcnn_obj->pnet_mutex[id]));
		pthread_cond_wait( &(mtcnn_obj->pnet_cond[id]), &(mtcnn_obj->pnet_mutex[id]));//等待事件被触发
	    for(unsigned int i = 0; i < mtcnn_obj->win_list_g[id].size(); i++)
		{
			std::vector<face_box> boxes;
			if(mtcnn_obj->run_PNet_Multi(id,mtcnn_obj->working_img, mtcnn_obj->win_list_g[id][i], boxes)!= 0)
				mtcnn_obj->pnet_status[id] = -1;
			pthread_mutex_lock(&(mtcnn_obj->mutex_sync));
			mtcnn_obj->total_pnet_boxes.insert(mtcnn_obj->total_pnet_boxes.end(), boxes.begin(), boxes.end());
			pthread_mutex_unlock(&(mtcnn_obj->mutex_sync));
		}
		
		if(mtcnn_obj->pnet_status[id] == 0){
			pthread_mutex_lock(&(mtcnn_obj->mutex_sync));
			mtcnn_obj->pnet_status[id] = 1;
			if((mtcnn_obj->pnet_status[0] == 1)&&(mtcnn_obj->pnet_status[1] == 1))
			{
				pthread_cond_signal(&(mtcnn_obj->pnet_finish));
			}
			pthread_mutex_unlock(&(mtcnn_obj->mutex_sync));
		}else{
			pthread_cond_signal(&(mtcnn_obj->pnet_finish));
		}
		pthread_mutex_unlock(&(mtcnn_obj->pnet_mutex[id]));
	
	}
}
#endif
int mtcnn::load_3model(const std::string& model_dir)
{
    std::string proto_name, mdl_name;

    // Pnet
    proto_name = model_dir + "/det1.prototxt";
    mdl_name = model_dir + "/det1.caffemodel";
    if(!check_file_exist(proto_name) or (!check_file_exist(mdl_name)))
    {
        return 1;
    }


#ifndef MULTI_THREAD
	PNet_graph = create_graph(nullptr, "caffe", proto_name.c_str(), mdl_name.c_str());
    if(PNet_graph == nullptr)
    {
        std::cout << "Create Pnet Graph failed\n";
        std::cout << "errno: " << get_tengine_errno() << "\n";
        return 1;
    }
#else
thread_data[0].mtobj=this;
thread_data[1].mtobj=this;
thread_data[0].id=0;
thread_data[1].id=1;
std::thread t0(PNet_graph_loop,&thread_data[0]);
t0.detach();
std::thread t1(PNet_graph_loop,&thread_data[1]);
t1.detach();
#endif
   // Rnet
    proto_name = model_dir + "/det2.prototxt";
    mdl_name = model_dir + "/det2.caffemodel";
    if(!check_file_exist(proto_name) or (!check_file_exist(mdl_name)))
    {
        return 1;
    }
    RNet_graph = create_graph(nullptr, "caffe", proto_name.c_str(), mdl_name.c_str());
    if(RNet_graph == nullptr)
    {
        std::cout << "Create Rnet Graph failed\n";
        std::cout << "errno: " << get_tengine_errno() << "\n";
        return 1;
    }

    // Onet
    proto_name = model_dir + "/det3.prototxt";
    mdl_name = model_dir + "/det3.caffemodel";
    if(!check_file_exist(proto_name) or (!check_file_exist(mdl_name)))
    {
        return 1;
    }
    ONet_graph = create_graph(nullptr, "caffe", proto_name.c_str(), mdl_name.c_str());
    if(ONet_graph == nullptr)
    {
        std::cout << "Create Onet Graph failed\n";
        std::cout << "errno: " << get_tengine_errno() << "\n";
        return 1;
    }
#ifndef MULTI_THREAD
	set_graph_device(PNet_graph, "GPU");
#endif
	set_graph_device(RNet_graph, "GPU");
	set_graph_device(ONet_graph, "GPU");
    return 0;
}

#ifdef MULTI_THREAD
int mtcnn::run_PNet_Multi(int id,const cv::Mat& img, scale_window& win, std::vector<face_box>& box_list)
{
    cv::Mat resized;
    int scale_h = win.h;
    int scale_w = win.w;
    float scale = win.scale;

    static bool first_run = true;

    cv::resize(img, resized, cv::Size(scale_w, scale_h), 0, 0, cv::INTER_NEAREST);
    /* input */

    tensor_t input_tensor = get_graph_tensor(pnet_graph[id], "data");
    int dims[] = {1, 3, scale_h, scale_w};
    set_tensor_shape(input_tensor, dims, 4);
    int in_mem = sizeof(float) * scale_h * scale_w * 3;
    // std::cout<<"mem "<<in_mem<<"\n";
    float* input_data = ( float* )malloc(in_mem);

    std::vector<cv::Mat> input_channels;
    set_cvMat_input_buffer(input_channels, input_data, scale_h, scale_w);
    cv::split(resized, input_channels);

    set_tensor_buffer(input_tensor, input_data, in_mem);

    if(first_run)
    {
        if(prerun_graph(pnet_graph[id]) != 0)
        {
            std::cout << "Prerun PNet graph failed, errno: " << get_tengine_errno() << "\n";
            return 1;
        }
        first_run = false;
    }

    if(run_graph(pnet_graph[id], 1) != 0)
    {
        std::cout << "Run PNet graph failed, errno: " << get_tengine_errno() << "\n";
        return 1;
    }
    free(input_data);
    /* output */
    tensor_t tensor = get_graph_tensor(pnet_graph[id], "conv4-2");
    get_tensor_shape(tensor, dims, 4);
    float* reg_data = ( float* )get_tensor_buffer(tensor);
    int feature_h = dims[2];
    int feature_w = dims[3];
    // std::cout<<"Pnet scale h,w= "<<feature_h<<","<<feature_w<<"\n";

    tensor = get_graph_tensor(pnet_graph[id], "prob1");
    float* prob_data = ( float* )get_tensor_buffer(tensor);
    std::vector<face_box> candidate_boxes;
    generate_bounding_box(prob_data, reg_data, scale, conf_p_threshold_, feature_h, feature_w, candidate_boxes, true);

    release_graph_tensor(input_tensor);
    release_graph_tensor(tensor);

    nms_boxes(candidate_boxes, 0.5, NMS_UNION, box_list);

    // std::cout<<"condidate boxes size :"<<candidate_boxes.size()<<"\n";
    return 0;
}
#endif

int mtcnn::run_PNet(const cv::Mat& img, scale_window& win, std::vector<face_box>& box_list)
{
    cv::Mat resized;
    int scale_h = win.h;
    int scale_w = win.w;
    float scale = win.scale;

    static bool first_run = true;

    cv::resize(img, resized, cv::Size(scale_w, scale_h), 0, 0, cv::INTER_NEAREST);
    /* input */

    tensor_t input_tensor = get_graph_tensor(PNet_graph, "data");
    int dims[] = {1, 3, scale_h, scale_w};
    set_tensor_shape(input_tensor, dims, 4);
    int in_mem = sizeof(float) * scale_h * scale_w * 3;
    // std::cout<<"mem "<<in_mem<<"\n";
    float* input_data = ( float* )malloc(in_mem);

    std::vector<cv::Mat> input_channels;
    set_cvMat_input_buffer(input_channels, input_data, scale_h, scale_w);
    cv::split(resized, input_channels);

    set_tensor_buffer(input_tensor, input_data, in_mem);

    if(first_run)
    {
        if(prerun_graph(PNet_graph) != 0)
        {
            std::cout << "Prerun PNet graph failed, errno: " << get_tengine_errno() << "\n";
            return 1;
        }
        first_run = false;
    }

    if(run_graph(PNet_graph, 1) != 0)
    {
        std::cout << "Run PNet graph failed, errno: " << get_tengine_errno() << "\n";
        return 1;
    }
    free(input_data);
    /* output */
    tensor_t tensor = get_graph_tensor(PNet_graph, "conv4-2");
    get_tensor_shape(tensor, dims, 4);
    float* reg_data = ( float* )get_tensor_buffer(tensor);
    int feature_h = dims[2];
    int feature_w = dims[3];
    // std::cout<<"Pnet scale h,w= "<<feature_h<<","<<feature_w<<"\n";

    tensor = get_graph_tensor(PNet_graph, "prob1");
    float* prob_data = ( float* )get_tensor_buffer(tensor);
    std::vector<face_box> candidate_boxes;
    generate_bounding_box(prob_data, reg_data, scale, conf_p_threshold_, feature_h, feature_w, candidate_boxes, true);

    release_graph_tensor(input_tensor);
    release_graph_tensor(tensor);

    nms_boxes(candidate_boxes, 0.5, NMS_UNION, box_list);

    // std::cout<<"condidate boxes size :"<<candidate_boxes.size()<<"\n";
    return 0;
}

int mtcnn::run_RNet(const cv::Mat& img, std::vector<face_box>& pnet_boxes, std::vector<face_box>& output_boxes)
{
    int batch = pnet_boxes.size();
    int channel = 3;
    int height = 24;
    int width = 24;
    static bool first_run = true;

    tensor_t input_tensor = get_graph_tensor(RNet_graph, "data");
    int dims[] = {batch, channel, height, width};
    set_tensor_shape(input_tensor, dims, 4);
    int img_size = channel * height * width;
    int in_mem = sizeof(float) * batch * img_size;
    float* input_data = ( float* )malloc(in_mem);
    float* input_ptr = input_data;
    set_tensor_buffer(input_tensor, input_ptr, in_mem);

    for(int i = 0; i < batch; i++)
    {
        copy_one_patch(img, pnet_boxes[i], input_ptr, width, height);
        input_ptr += img_size;
    }

    if(first_run)
    {
        if(prerun_graph(RNet_graph) != 0)
        {
            std::cout << "Prerun RNet graph failed, errno: " << get_tengine_errno() << "\n";
            return 1;
        }
        first_run = false;
    }

    if(run_graph(RNet_graph, 1) != 0)
    {
        std::cout << "Run RNet graph failed, errno: " << get_tengine_errno() << "\n";
        return 1;
    }
    free(input_data);

    tensor_t tensor = get_graph_tensor(RNet_graph, "conv5-2");
    float* reg_data = ( float* )get_tensor_buffer(tensor);

    tensor = get_graph_tensor(RNet_graph, "prob1");
    float* confidence_data = ( float* )get_tensor_buffer(tensor);
	

    int conf_page_size = 2;
    int reg_page_size = 4;

    for(int i = 0; i < batch; i++)
    {
        if(*(confidence_data + 1) > conf_r_threshold_)
        {
            face_box output_box;
            face_box& input_box = pnet_boxes[i];

            output_box.x0 = input_box.x0;
            output_box.y0 = input_box.y0;
            output_box.x1 = input_box.x1;
            output_box.y1 = input_box.y1;

            output_box.score = *(confidence_data + 1);

            /*Note: regress's value is swaped here!!!*/

            output_box.regress[0] = reg_data[1];
            output_box.regress[1] = reg_data[0];
            output_box.regress[2] = reg_data[3];
            output_box.regress[3] = reg_data[2];

            output_boxes.push_back(output_box);
            // std::cout<<"in ";
        }

        confidence_data += conf_page_size;
        reg_data += reg_page_size;
    }

    release_graph_tensor(input_tensor);
    release_graph_tensor(tensor);

    return 0;
}

int mtcnn::run_ONet(const cv::Mat& img, std::vector<face_box>& rnet_boxes, std::vector<face_box>& output_boxes)
{
    int batch = rnet_boxes.size();

    int channel = 3;
    int height = 48;
    int width = 48;
    tensor_t input_tensor = get_graph_tensor(ONet_graph, "data");
    int dims[] = {batch, channel, height, width};
    set_tensor_shape(input_tensor, dims, 4);
    int img_size = channel * height * width;
    int in_mem = sizeof(float) * batch * img_size;
    float* input_data = ( float* )malloc(in_mem);
    float* input_ptr = input_data;
    static bool first_run = true;

    set_tensor_buffer(input_tensor, input_ptr, in_mem);
    for(int i = 0; i < batch; i++)
    {
        copy_one_patch(img, rnet_boxes[i], input_ptr, width, height);
        input_ptr += img_size;
    }

    if(first_run)
    {
        if(prerun_graph(ONet_graph) != 0)
        {
            std::cout << "Prerun ONet graph failed, errno: " << get_tengine_errno() << "\n";
            return 1;
        }
        first_run = false;
    }

    if(run_graph(ONet_graph, 1) != 0)
    {
        std::cout << "Run ONet graph failed, errno: " << get_tengine_errno() << "\n";
        return 1;
    }
    free(input_data);
    /* output */
    tensor_t tensor = get_graph_tensor(ONet_graph, "conv6-3");
    float* points_data = ( float* )get_tensor_buffer(tensor);

    tensor = get_graph_tensor(ONet_graph, "prob1");
    float* confidence_data = ( float* )get_tensor_buffer(tensor);

    tensor = get_graph_tensor(ONet_graph, "conv6-2");
    float* reg_data = ( float* )get_tensor_buffer(tensor);
    int conf_page_size = 2;
    int reg_page_size = 4;
    int points_page_size = 10;
    for(int i = 0; i < batch; i++)
    {
        if(*(confidence_data + 1) > conf_r_threshold_)
        {
            face_box output_box;
            face_box& input_box = rnet_boxes[i];

            output_box.x0 = input_box.x0;
            output_box.y0 = input_box.y0;
            output_box.x1 = input_box.x1;
            output_box.y1 = input_box.y1;

            output_box.score = *(confidence_data + 1);

            output_box.regress[0] = reg_data[1];
            output_box.regress[1] = reg_data[0];
            output_box.regress[2] = reg_data[3];
            output_box.regress[3] = reg_data[2];

            /*Note: switched x,y points value too..*/

            for(int j = 0; j < 5; j++)
            {
                output_box.landmark.x[j] = *(points_data + j + 5);
                output_box.landmark.y[j] = *(points_data + j);
            }

            output_boxes.push_back(output_box);
        }

        confidence_data += conf_page_size;
        reg_data += reg_page_size;
        points_data += points_page_size;
    }
    release_graph_tensor(input_tensor);
    release_graph_tensor(tensor);
    return 0;
}

void mtcnn::detect(cv::Mat& img, std::vector<face_box>& face_list)
{
#ifndef MULTI_THREAD
    cv::Mat working_img;
#endif
    float alpha = 0.0078125;
    float mean = 127.5;
    img.convertTo(working_img, CV_32FC3);
    working_img = (working_img - mean) * alpha;
    working_img = working_img.t();
    cv::cvtColor(working_img, working_img, cv::COLOR_BGR2RGB);

    int img_h = working_img.rows;
    int img_w = working_img.cols;

    std::vector<scale_window> win_list;
#ifndef MULTI_THREAD
    std::vector<face_box> total_pnet_boxes;
#endif
    std::vector<face_box> total_rnet_boxes;
    std::vector<face_box> total_onet_boxes;

    cal_scale_list(img_h, img_w, minsize_, win_list);
#ifndef MULTI_THREAD
   for(unsigned int i = 0; i < win_list.size(); i++)
    {
        std::vector<face_box> boxes;
	 #ifndef PRINT_TIME
      	if(run_PNet(working_img, win_list[i], boxes) != 0)
        	return;
     #else
        int pnet_ret=0;
		MT_PRINT_TIME(pnet_ret,run_PNet(working_img, win_list[i], boxes),1);
		if(pnet_ret != 0)
            return;
	  #endif
        total_pnet_boxes.insert(total_pnet_boxes.end(), boxes.begin(), boxes.end());
    }
#else
	win_list_g[0].clear();
 	win_list_g[1].clear();
	for(unsigned int i = 0; i < win_list.size(); i++)
	{
		win_list_g[i%2].push_back(win_list[i]);
	}
	pthread_cond_signal(&pnet_cond[0]);
	pthread_cond_signal(&pnet_cond[1]);
	pnet_status[0] = 0;
	pnet_status[1] = 0;
	pthread_cond_wait(&pnet_finish, &mutex_rnet_trigger);//等待事件被触发
	if((pnet_status[0]!=1)||(pnet_status[1]!=1))
		return;
#endif
#ifndef TEST_TIME
    win_list.clear();
    std::vector<face_box> pnet_boxes;
	
#ifndef PRINT_TIME
    process_boxes(total_pnet_boxes, img_h, img_w, pnet_boxes, nms_p_threshold_);
#else
	MT_PRINT_TIME_NORET(process_boxes(total_pnet_boxes, img_h, img_w, pnet_boxes, nms_p_threshold_),1);
#endif
    if(!pnet_boxes.size())
        return;

#ifndef PRINT_TIME
    if(run_RNet(working_img, pnet_boxes, total_rnet_boxes) != 0)
		 return;
#else
	int rnet_ret=0;
	MT_PRINT_TIME(rnet_ret,run_RNet(working_img, pnet_boxes, total_rnet_boxes),2);
	if(rnet_ret != 0)
        return;
#endif
	total_pnet_boxes.clear();

    std::vector<face_box> rnet_boxes;
	
#ifndef PRINT_TIME
    process_boxes(total_rnet_boxes, img_h, img_w, rnet_boxes, nms_r_threshold_);
#else
	MT_PRINT_TIME_NORET(process_boxes(total_rnet_boxes, img_h, img_w, rnet_boxes, nms_r_threshold_),2);
#endif
    if(!rnet_boxes.size())
        return;

#ifndef PRINT_TIME
    if(run_ONet(working_img, rnet_boxes, total_onet_boxes) != 0)
		return;
#else
	int onet_ret=0;
	MT_PRINT_TIME(onet_ret,run_ONet(working_img, rnet_boxes, total_onet_boxes) ,3);
	if(onet_ret != 0)
        return;
#endif
	total_rnet_boxes.clear();

    for(unsigned int i = 0; i < total_onet_boxes.size(); i++)
    {
        face_box& box = total_onet_boxes[i];

        float h = box.x1 - box.x0 + 1;
        float w = box.y1 - box.y0 + 1;

        for(int j = 0; j < 5; j++)
        {
            box.landmark.x[j] = box.x0 + w * box.landmark.x[j] - 1;
            box.landmark.y[j] = box.y0 + h * box.landmark.y[j] - 1;
        }
    }
    regress_boxes(total_onet_boxes);
    nms_boxes(total_onet_boxes, nms_o_threshold_, NMS_MIN, face_list);
    total_onet_boxes.clear();

    for(unsigned int i = 0; i < face_list.size(); i++)
    {
        face_box& box = face_list[i];

        std::swap(box.x0, box.y0);
        std::swap(box.x1, box.y1);

        for(int l = 0; l < 5; l++)
        {
            std::swap(box.landmark.x[l], box.landmark.y[l]);
        }
    }
#endif
}
