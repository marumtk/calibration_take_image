#pragma once
//------------------------------------------------------------------
// Class for XimeaXiQ
// - 2015/11/16
// -- gaku narita
//------------------------------------------------------------------


#include <windows.h>
#include <opencv2/opencv.hpp>
#include "xiApi.h"
#pragma comment(lib, "m3apiX64.lib")



class XimeaXiQ{
public:
	XI_IMG xi_img;
	HANDLE xi_handle;
	DWORD xi_camera_num;
	XI_RETURN xi_status;


	const unsigned int CAMERA_WIDTH = 648;
	const unsigned int CAMERA_HEIGHT = 488;
	const unsigned int USE_XI_TRIGGER = 0;
	const unsigned int FRAME_PER_SEC = 100;

	//---- コンストラクタ
	XimeaXiQ();

	//---- デストラクタ
	~XimeaXiQ();

	//---- 画像取得
	bool getFrame(cv::Mat& frame);

};