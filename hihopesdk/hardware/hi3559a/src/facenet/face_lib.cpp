#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>  
#include <sys/stat.h>  

#include "face_lib.hpp"


#define dmax(a,b)  (((a) > (b)) ? (a) : (b))
#define dmin(a,b)  (((a) < (b)) ? (a) : (b))

face_lib::face_lib(face_net *facenet,mtcnn *mt)
{
	Fnet = facenet;
	Mtcnn = mt;
}
face_lib::~face_lib(){

}

//获取特定格式的文件名
int face_lib::readFileList(std::vector<string> &filelist, const char *basePath, string format)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    //file
        {
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            string temp = ptr->d_name;
            //cout  << temp << endl;
            string sub = temp.substr(temp.length() - 4, temp.length()-1);
            //cout  << sub << endl;
            if(sub == format)
            {
                string path = basePath;
                path += "/";
                path += ptr->d_name;
                filelist.push_back(path);
            }
        }
        else if(ptr->d_type == 10)    ///link file
        {
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
        }
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(filelist, base, format);
        }
    }
    closedir(dir);
    return 1;
}

//找出目录中所有子目录
int face_lib::findAllSubDir(std::vector<string> &filelist, const char *basePath)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    //file
        {
            // //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            // string temp = ptr->d_name;
            // //cout  << temp << endl;
            // string sub = temp.substr(temp.length() - 4, temp.length()-1);
            // //cout  << sub << endl;
            // if(sub == format)
            // {
            //     string path = basePath;
            //     path += "/";
            //     path += ptr->d_name;
            //     filelist.push_back(path);
            // }
        }
        else if(ptr->d_type == 10)    ///link file
        {
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
        }
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            filelist.push_back(ptr->d_name);
            findAllSubDir(filelist, base);
        }
    }
    closedir(dir);
    return 1;
}


void face_lib::findDir(string src, string &facefolder, string &facenameindex, string filePath)
{
    int begin = src.find(filePath) + filePath.size() + 1;
    int end = 0;
    for (int i = src.size() - 1; i >= 0; --i)
    {
        //cout << src[i] << endl;
        if (src[i] == '/')
        {
            end = i;
            break;
        }
    }
    //cout << begin << endl;
    //cout << end << endl;
    facefolder = src.substr(begin, end - 1 - begin + 1);
    facenameindex = src.substr(end + 1, src.size() - 1 - (end + 1) + 1);
}


void face_lib::GetStringFileName(const string &filePath, string &filename, string &fileformat)
{
    int fileformat_begin = 0;
    int fileformat_end = filePath.length() - 1;
    int filename_begin = 0;
    int filename_end = filePath.length() - 1;
    for (int i = filePath.length() - 1; i >= 0; --i)
    {
        //cout << filePath[i] << endl;
        if (filePath[i] == '.')
        {
            fileformat_begin = i + 1;
            filename_end = i - 1;
        }

        if (filePath[i] == '/')
        {
            filename_begin = i + 1;
            break;
        }
    }
    // cout << filename_begin << endl;
    // cout << filename_end << endl;
    filename = filePath.substr(filename_begin, filename_end - filename_begin + 1);
    fileformat = filePath.substr(fileformat_begin, fileformat_end - fileformat_begin + 1);
}

void face_lib::StringSplit(const string &src, const char splitchar, vector<string> &dst)
{
    int begin = 1;
    int end = 1;
    int i = 1;
    for (i = 1; i < src.length(); ++i)
    {
        if(src[i] == splitchar)
        {
            end = i - 1;
            dst.push_back(src.substr(begin, end - begin + 1));
            begin = i + 1;
            end = begin;
        }
    }

    //last
    if(i > end)
    {
        end = i - 1;
    }
    dst.push_back(src.substr(begin, end - begin + 1));
}

#define  INPUT_SIZE 400

bool box_vailed(face_box output_box)
{
	if((output_box.x0 >= 0)&&(output_box.x1 >= 0)&&(output_box.y0 >= 0)&&(output_box.y1 >= 0))
		if((output_box.x0 <= INPUT_SIZE)&&(output_box.x1 <= INPUT_SIZE)&&(output_box.y0 <= INPUT_SIZE)&&(output_box.y1 <=INPUT_SIZE))
			return true;
	return false;
}

//依次遍历文件夹中的每一个目录，遇到一个目录新建一个目录，然后遍历该目录的文件
int face_lib::creat_new_lib(char *path,char *save_file)
{
    // Loop over all the images provided on the command line.
    std::vector<string> sudDirfiles;
	std::vector<face_box> face_info_temp;
	FILE * file_fd;
	Face_data fdata;
	int count=0;
	int max_size_index = 0;
	int max_size=0;
	//string lib_path = new std::string(path);
    findAllSubDir(sudDirfiles,path);
	file_fd = fopen(save_file,"w+");
    for (int i = 0; i < sudDirfiles.size(); ++i)
    {
        //cout  << sudDirfiles[i] << endl;
        //遍历当前子目录中所有文件
        std::vector<string> srcfiles;
		string lpath(path);
        string srcSudDir = lpath + "/" + sudDirfiles[i];
        string srcformat = ".jpg";
      //  printf("the current subdir is : %s\n", srcSudDir.c_str());
        readFileList(srcfiles, srcSudDir.c_str(), srcformat);
        for (int j = 0; j < srcfiles.size(); ++j)
        {
            cout << srcfiles[j] << endl;
         //   string filename;
         //   string fileformat;
          //  GetStringFileName(srcfiles[j], filename, fileformat);
          //  string dstfile = dstSudDir + "/" + filename + "." + fileformat;

			cv::Mat image_raw;
		 	cv::Mat image;
			image_raw = cv::imread(srcfiles[j]);
			resize(image_raw, image, cv::Size(INPUT_SIZE, INPUT_SIZE), 0, 0, 3);
			face_info_temp.clear();
			Mtcnn->detect(image, face_info_temp);
			printf("pic[%d]:%s \n",count,sudDirfiles[i].c_str());
			count++;
		#if 0	
			for(unsigned int k = 0; k < face_info_temp.size(); k++)
			{
				face_box& box = face_info_temp[k];
				cv::Mat facenet_image;
				if(box_vailed(box)){
					image(cv::Rect(box.x0,box.y0,box.x1-box.x0,box.y1-box.y0)).copyTo(facenet_image);	
					sprintf(fdata.name,"%s",sudDirfiles[i].c_str());
					Fnet->get_feature_map(facenet_image, fdata.fearure_map);
					fwrite((char*)&fdata, sizeof(fdata), 1, file_fd);
				}
			}
		#else
		for(unsigned int k = 0; k < face_info_temp.size(); k++)
		{
			face_box& box = face_info_temp[k];
			if(box_vailed(box)){
				if(max_size < (box.x1-box.x0)*(box.y1-box.y0)){
					max_size =  (box.x1-box.x0)*(box.y1-box.y0);
					max_size_index = k;
				}
			}
		}
	
		cv::Mat facenet_image;
		face_box& box_max = face_info_temp[max_size_index];
		if(box_vailed(box_max)){
			image(cv::Rect(box_max.x0,box_max.y0,box_max.x1-box_max.x0,box_max.y1-box_max.y0)).copyTo(facenet_image);	
			sprintf(fdata.name,"%s",sudDirfiles[i].c_str());
			Fnet->get_feature_map(facenet_image, fdata.fearure_map);
			fwrite((char*)&fdata, sizeof(fdata), 1, file_fd);
			printf("add a new face :%s \n",sudDirfiles[i].c_str());
		}
		else{
			
			printf("box_vailed failed! %d \n",max_size_index);
		}
			
		#endif

			
        }
    }
	fclose(file_fd);
	return count;
}

int face_lib::load_lib_from_file(char *save_file,std::vector<Face_data> *dface)
{
	FILE * file_fd;
	int count = 0;
	int read_len;
	dface->clear();
	file_fd = fopen(save_file,"rb");
	if(!file_fd) {
	printf("error load lib \n");
	return 0;
	}
	while(1) { //一直读到文件结束
		Face_data  face_d;
		read_len = fread((char *)&face_d,sizeof(face_d),1,file_fd);
		if(read_len > 0){
			count++;
			printf("find face id %s \n",face_d.name);
			dface->push_back(face_d);
		}
		else
			break;
	}
	fclose(file_fd);
	return count;
}



int face_lib::add_to_lib_file(char *lib_file,char *file,char* name)
{
	FILE * file_fd;
	Face_data fdata;
	std::vector<face_box> face_info_temp;
	file_fd = fopen(lib_file,"a");
	cv::Mat image_raw;
	cv::Mat image;
	image_raw = cv::imread(file);
	resize(image_raw, image, cv::Size(INPUT_SIZE, INPUT_SIZE), 0, 0, 3);
	face_info_temp.clear();
	Mtcnn->detect(image, face_info_temp);
	int max_size_index = 0;
	int max_size=0;
	for(unsigned int k = 0; k < face_info_temp.size(); k++)
	{
		face_box& box = face_info_temp[k];
		if(box_vailed(box)){
			if(max_size < (box.x1-box.x0)*(box.y1-box.y0)){
				max_size =  (box.x1-box.x0)*(box.y1-box.y0);
				max_size_index = k;
			}
		}
	}
	
	cv::Mat facenet_image;
	face_box& box_max = face_info_temp[max_size_index];
	if(box_vailed(box_max)){
		printf("%f %f %f %f \n",box_max.x0,box_max.x1,box_max.y0,box_max.y1);
		image(cv::Rect(box_max.x0,box_max.y0,box_max.x1-box_max.x0,box_max.y1-box_max.y0)).copyTo(facenet_image);	
		sprintf(fdata.name,"%s",name);
		Fnet->get_feature_map(facenet_image, fdata.fearure_map);
		fwrite((char*)&fdata, sizeof(fdata), 1, file_fd);
		printf("add a new face :%s \n",name);
	}

	
	fclose(file_fd);
	return 0;
}



