#pragma once
#include <pylon/PylonIncludes.h>
#include <pylon/usb/BaslerUsbInstantCamera.h>
#ifdef PYLON_WIN_BUILD
#include <pylon/PylonGUI.h>
#endif

#include<opencv2\opencv.hpp>;

using namespace cv;

// Namespace for using pylon objects.
using namespace Pylon;

typedef Pylon::CBaslerUsbInstantCamera Camera_t;

// Namespace for using cout.
using namespace std;

// Number of images to be grabbed.
static const uint32_t c_countOfImagesToGrab = 1000;

// The exit code of the sample application.
//int exitCode = 0;



class Basler
{
public:
	cv::Mat img_col;
	cv::Mat img_gray;

	// Automagically call PylonInitialize and PylonTerminate to ensure 
	// the pylon runtime system is initialized during the lifetime of this object.
	Pylon::PylonAutoInitTerm autoInitTerm;
	CGrabResultPtr ptrGrabResult;
	CImageFormatConverter formatConverter;
	CPylonImage pylonImage;

	Camera_t camera;

	const unsigned int CAMERA_WIDTH = 640;
	const unsigned int CAMERA_HEIGHT = 480;
	const unsigned int FRAME_PER_SEC = 30;




	//�R���X�g���N�^
	Basler();
	//�f�X�g���N�^
	~Basler();

	//�J����������
	void camera_init();

	//�J���[�摜�̎擾
	bool cap_color(cv::Mat &img_col);

	//�O���[�X�P�[���摜�̎擾
	bool cap_gray(cv::Mat &img_gray);

};

//���̕ϐ������info���֐��ɂ�菉�������邽�߂Ɍp���𗘗p
class basler : public Basler
{
public:
	CDeviceInfo info;

	basler(){info.SetDeviceClass(Camera_t::DeviceClass());}
	~basler(){}
};
