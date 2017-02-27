#include "Webcam.h"


//////////////////////////�R���X�g���N�^/////////////////////////////////////////
Webcam::Webcam()
{
	cap.open(0); //USB�ڑ������J�������J��

				 // Web�J�������������ڑ�����Ă��邩
	if (!cap.isOpened()) 
	{ 
		std::cout << "Can not open device." << std::endl;
	}
}


/////////////////////�J���[�摜�ŃL���v�`��///////////////////////////////////
bool Webcam::cap_color(cv::Mat &img_col)
{
	cap >> img_col;
	return true;
}


/////////////////////�O���[�X�P�[���ŃL���v�`��///////////////////////////////
bool Webcam::cap_gray(cv::Mat &img_gray)
{
	cv::Mat frameBGR(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC3);
	cap >> frameBGR;
	cv::cvtColor(frameBGR, img_gray, CV_BGR2GRAY);
	return true;
}

//////////////////////////�N���X�m�F�p��main�֐�////////////////////////////////
/*
int main()
{
Webcam obj1;
cv::Mat img(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC1);
for (;;) {
obj1.cap_gray(img);
cv::namedWindow("webcam");
cv::imshow("webcam", img);
if (cv::waitKey(30) >= 0) break;
//cv::waitKey(0);
}
return 1;
}
*/