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

/*
 * Copyright (c) 2018, Open AI Lab
 * Author: chunyinglv@openailab.com
 */
#ifndef __MTCNN_HPP__
#define __MTCNN_HPP__

//#define MULTI_THREAD
#include "mtcnn_utils.hpp"
#include "common.hpp"
#include <thread>
typedef struct thread_data_{
	void *mtobj;
	int id;
}Thread_data;
class mtcnn
{
public:
    int minsize_;
    float conf_p_threshold_;
    float conf_r_threshold_;
    float conf_o_threshold_;

    float nms_p_threshold_;
    float nms_r_threshold_;
    float nms_o_threshold_;
#ifdef MULTI_THREAD
	std::vector<scale_window> win_list_g[2];
	std::vector<face_box> pnet_ret_boxes[2];
	int pnet_status[2];
	int win_id[2];
	cv::Mat working_img;
	pthread_mutex_t pnet_mutex[2];
	pthread_cond_t pnet_cond[2];
	pthread_cond_t pnet_finish;
	pthread_mutex_t mutex_sync;
	int pnet_init_flag[2];
	pthread_mutex_t mutex_rnet_trigger;
	std::vector<face_box> total_pnet_boxes;
	Thread_data thread_data[2];
#endif

    mtcnn(int minsize = 60, float conf_p = 0.6, float conf_r = 0.7, float conf_o = 0.8, float nms_p = 0.5,
          float nms_r = 0.7, float nms_o = 0.7);
    ~mtcnn()
    {
        if(postrun_graph(PNet_graph) != 0)
        {
            std::cout << "Postrun PNet graph failed, errno: " << get_tengine_errno() << "\n";
        }
        if(postrun_graph(RNet_graph) != 0)
        {
            std::cout << "Postrun RNet graph failed, errno: " << get_tengine_errno() << "\n";
        }
        if(postrun_graph(ONet_graph) != 0)
        {
            std::cout << "Postrun ONet graph failed, errno: " << get_tengine_errno() << "\n";
        }
		#ifdef MULTI_THREAD		
		postrun_graph(pnet_graph[0]);
		postrun_graph(pnet_graph[1]);
        destroy_graph(pnet_graph[0]);
        destroy_graph(pnet_graph[1]);
		#else
       	destroy_graph(PNet_graph);
		#endif
        destroy_graph(RNet_graph);
        destroy_graph(ONet_graph);
    };

    int load_3model(const std::string& model_dir);

    void detect(cv::Mat& img, std::vector<face_box>& face_list);
	
#ifdef MULTI_THREAD
	int run_PNet_Multi(int id,const cv::Mat& img, scale_window& win, std::vector<face_box>& box_list);
	graph_t pnet_graph[2];
#endif

protected:
    int run_PNet(const cv::Mat& img, scale_window& win, std::vector<face_box>& box_list);
    int run_RNet(const cv::Mat& img, std::vector<face_box>& pnet_boxes, std::vector<face_box>& output_boxes);
    int run_ONet(const cv::Mat& img, std::vector<face_box>& rnet_boxes, std::vector<face_box>& output_boxes);
private:
    graph_t PNet_graph;
    graph_t RNet_graph;
    graph_t ONet_graph;
};
#endif
