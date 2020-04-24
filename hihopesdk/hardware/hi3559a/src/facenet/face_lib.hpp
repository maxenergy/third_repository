#ifndef __FACE_LIB_HPP__
#define __FACE_LIB_HPP__

#include "face_net.hpp"
#include "mtcnn.hpp"
using namespace std;

typedef struct Face_data_{	
	char name[50];
	int fearure_map[128];
}Face_data;


class face_lib
{
public:
		face_lib(face_net *facenet,mtcnn *mt);
		~face_lib();
		int creat_new_lib(char *path,char *save_file);
		int load_lib_from_file(char *save_file,std::vector<Face_data> *dface);
		int add_to_lib_file(char *lib_file,char *file,char* name);
private:
		face_net *Fnet;
		mtcnn *Mtcnn;
protected:
	
void StringSplit(const string &src, const char splitchar, vector<string> &dst);
void GetStringFileName(const string &filePath, string &filename, string &fileformat);
void findDir(string src, string &facefolder, string &facenameindex, string filePath);
int findAllSubDir(std::vector<string> &filelist, const char *basePath);
int readFileList(std::vector<string> &filelist, const char *basePath, string format);
};
#endif

