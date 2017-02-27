


//------------------------------------------------------------------
// Class for XimeaXiQ
// - 2015/11/16
// -- gaku narita
//------------------------------------------------------------------

#include "XimeaXiQ.h"

//---- コンストラクタ
XimeaXiQ::XimeaXiQ(){
#if !USE_XI_TRIGGER //---- フリーランモード
	xiGetNumberDevices(&xi_camera_num);
	if(!xi_camera_num) { std::cout << "error!! after xiGetNumberDevices." << std::endl; }

	xi_status = xiOpenDevice(0, &xi_handle);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiOpenDevice." << std::endl; }
	xi_status = xiSetParamInt(xi_handle, XI_PRM_TRG_SOURCE, XI_TRG_OFF);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiSetParamInt." << std::endl; }
	xi_status = xiSetParamInt(xi_handle, XI_PRM_EXPOSURE, (int)(1000000.0 / FRAME_PER_SEC) - 100);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiSetParamInt." << std::endl; }
	xi_status = xiSetParamInt(xi_handle, XI_PRM_ACQ_TIMING_MODE, XI_ACQ_TIMING_MODE_FRAME_RATE);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiSetParamInt." << std::endl; }
	xi_status = xiSetParamFloat(xi_handle, XI_PRM_FRAMERATE, FRAME_PER_SEC);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiSetParamFloat." << std::endl; }

	xi_status = xiStartAcquisition(xi_handle);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiStartAcquisition." << std::endl; }

	Sleep(1000);
#else //---- トリガモード
	xiGetNumberDevices(&xi_camera_num);
	if(!xi_camera_num) { std::cout << "error!! camera was not connected." << std::endl; }

	xi_status = xiOpenDevice(0, &xi_handle);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiOpenDevice." << std::endl; }
	xi_status = xiSetParamInt(xi_handle, XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiSetParamInt." << std::endl; }
	xi_status = xiSetParamInt(xi_handle, XI_PRM_EXPOSURE, (int)(1000000.0 / FRAME_PER_SEC) - 100);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiSetParamInt." << std::endl; }

	xi_status = xiStartAcquisition(xi_handle);
	if(xi_status!=XI_OK) { std::cout << "error!! after xiStartAcquisition." << std::endl; }
	Sleep(1000);
#endif

}

//---- デストラクタ
XimeaXiQ::~XimeaXiQ()
{
	Sleep(1000);
	xiStopAcquisition(xi_handle);
	xiCloseDevice(xi_handle);
	xi_handle = NULL;
}


//---- 画像取得
bool XimeaXiQ::getFrame(cv::Mat& frame){
	frame.create(CAMERA_HEIGHT, CAMERA_WIDTH, CV_8UC1);
	xi_img.size = sizeof(XI_IMG);
	xiGetImage(xi_handle, 50000, &xi_img);
	memcpy(frame.data, xi_img.bp, xi_img.width*xi_img.height);
	return true;

}