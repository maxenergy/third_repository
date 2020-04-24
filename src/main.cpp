#include "main.h"
#include <unistd.h>

extern "C" {
extern int sysinit(int* fd);
extern void log_level_set(int level);
extern void init_pola_sdk(int liveless_mode,float detect_threshold);
}

int main()
{
	int fd =0;
	std::cout << "pola sdk init!!! " << std::endl;
	sysinit(&fd);
	log_level_set(1);
	init_pola_sdk(1,0.4);

	FaceRecognitionApi &framework = FaceRecognitionApi::getInstance();
	 if (!framework.init()) {
		 printf("framework init failed \n");
		 return 1;
	 };

	framework.startCameraPreview();
	framework.setCameraPreviewCallBack([&](FaceDetect::Msg bob){
	   process_detectet(bob);
	});

	while(1)
	{
		sleep(1);
	}
}

void process_detectet(FaceDetect::Msg bob)
{
	printf("process_detectret \n");
	return;
}

