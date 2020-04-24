#include "facerecognitionapi.h"
#include <string>
#include <vector>
#include <iostream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

void testDB() {
    //UserInfo
    for (int i = 0; i < 10; i++) {
        UserInfo user;
        user.mUserID = i;
        user.mUserName = "xxx";
        FaceRecognitionApi::getInstance().updateUserInfo(user);
    }
    for (int i = 0; i < 5; i++) {
        UserInfo user;
        user.mUserID = i;
        user.mUserName = "asas";
        FaceRecognitionApi::getInstance().updateUserInfo(user);
    }
    for (int i = 0; i < 5; i++) {
        UserInfo user;
        FaceRecognitionApi::getInstance().getUserInfo(i, user);
        std::cout << user.mUserID << ","  << user.mUserName << std::endl;
    }

    //PermissionInfo
    for (int i = 0; i < 10; i++) {
        PermissionInfo permission;
        permission.mUserID = i;
        permission.mDeviceID = 100;
        permission.mPermission = 1;
        FaceRecognitionApi::getInstance().updatePermissionInfo(permission);
    }
    for (int i = 0; i < 3; i++) {
        PermissionInfo permission;
        permission.mUserID = i;
        permission.mDeviceID = 100;
        permission.mPermission = 3;
        FaceRecognitionApi::getInstance().updatePermissionInfo(permission);
    }
    for (int i = 3; i < 6; i++) {
        PermissionInfo permission;
        permission.mUserID = i;
        permission.mDeviceID = 10;
        permission.mPermission = 2;
        FaceRecognitionApi::getInstance().updatePermissionInfo(permission);
    }
    for (int i = 0; i < 10; i++) {
        PermissionInfo permission;
        FaceRecognitionApi::getInstance().getCurDevicePermissionInfo(i, permission);
        std::cout << permission.mUserID << ","  << permission.mDeviceID << "," << permission.mPermission << std::endl;
    }

    //FaceInfo
    for (int i = 0; i < 10; i++) {
        FaceInfo face;
        face.mUserID = i;
        face.mFeatureMap[0] = 721;
        face.mFeatureMap[1] = 113;
        face.mFeatureMap[2] = 125;
        face.mFeatureMap[3] = 457;
        FaceRecognitionApi::getInstance().updateFaceInfo(face);
    }
    for (int i = 0; i < 5; i++) {
        FaceInfo face;
        face.mUserID = i;
        face.mFeatureMap[0] = 71;
        face.mFeatureMap[1] = 13;
        face.mFeatureMap[2] = 12;
        face.mFeatureMap[3] = 45;
        FaceRecognitionApi::getInstance().updateFaceInfo(face);
    }
    for (int i = 0; i < FaceRecognitionApi::getInstance().mDBCache->mNumber; i++) {
        std::cout <<
                     FaceRecognitionApi::getInstance().mDBCache->mUserIDs[i] << "," <<
                     FaceRecognitionApi::getInstance().mDBCache->mFeatureMaps[i*128 + 0] << "," <<
                     FaceRecognitionApi::getInstance().mDBCache->mFeatureMaps[i*128 + 1] << "," <<
                     FaceRecognitionApi::getInstance().mDBCache->mFeatureMaps[i*128 + 2] << "," <<
                     FaceRecognitionApi::getInstance().mDBCache->mFeatureMaps[i*128 + 3] << std::endl;
    }
}



vector<string> getFiles(string cate_dir) {
    vector<string> files;
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(cate_dir.c_str())) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            files.push_back(ptr->d_name);
        else if(ptr->d_type == 10)    ///link file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
            continue;
        else if(ptr->d_type == 4)    ///dir
        {
            files.push_back(ptr->d_name);
            /*
                memset(base,'\0',sizeof(base));
                strcpy(base,basePath);
                strcat(base,"/");
                strcat(base,ptr->d_nSame);
                readFileList(base);
            */
        }
    }
    closedir(dir);

    //排序，按从小到大排序
    sort(files.begin(), files.end());
    return files;
}


void addOneFaceInfo(std::string file) {
    static int sUserIdIdx = 0;

    std::cout << file << std::endl;
    cv::Mat faceImage = cv::imread(file);
    if (faceImage.empty()) {
        std::cout << "face image empty" << std::endl;
        return;
    }

    if (!FaceRecognitionApi::getInstance().updateFaceInfo(sUserIdIdx, faceImage)) {
        std::cout << "face info update failed" << std::endl;
        return;
    }

    UserInfo info;
    info.mUserID = sUserIdIdx;
    info.mUserName = file;
    if (!FaceRecognitionApi::getInstance().updateUserInfo(info) ) {
        std::cout << "user info update failed" << std::endl;
        return;
    }
    std::cout << "add {" << sUserIdIdx << ", "<< file << "} ok" << std::endl;
    sUserIdIdx ++;
    return;
}

void testFaceInfoAdd() {
    vector<string> files = getFiles("images/facerecogination");
    for(std::string file : files) {
        addOneFaceInfo("images/facerecogination/" + file);
    }
}


