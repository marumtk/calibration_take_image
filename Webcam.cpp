#include "Webcam.h"


//////////////////////////コンストラクタ/////////////////////////////////////////
Webcam::Webcam()
{
	cap.open(0); //USB接続したカメラを開く

				 // Webカメラが正しく接続されているか
	if (!cap.isOpened()) 
	{ 
		std::cout << "Can not open device." << std::endl;
	}
}


/////////////////////カラー画像でキャプチャ///////////////////////////////////
bool Webcam::cap_color(cv::Mat &img_col)
{
	cap >> img_col;
	return true;
}


/////////////////////グレースケールでキャプチャ///////////////////////////////
bool Webcam::cap_gray(cv::Mat &img_gray)
{
	cv::Mat frameBGR(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC3);
	cap >> frameBGR;
	cv::cvtColor(frameBGR, img_gray, CV_BGR2GRAY);
	return true;
}

//////////////////////////クラス確認用のmain関数////////////////////////////////
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