// Grab.cpp
/*
Note: Before getting started, Basler recommends reading the Programmer's Guide topic
in the pylon C++ API documentation that gets installed with pylon.
If you are upgrading to a higher major version of pylon, Basler also
strongly recommends reading the Migration topic in the pylon C++ API documentation.

This sample illustrates how to grab and process images using the CInstantCamera class.
The images are grabbed and processed asynchronously, i.e.,
while the application is processing a buffer, the acquisition of the next buffer is done
in parallel.

The CInstantCamera class uses a pool of buffers to retrieve image data
from the camera device. Once a buffer is filled and ready,
the buffer can be retrieved from the camera object for processing. The buffer
and additional image data are collected in a grab result. The grab result is
held by a smart pointer after retrieval. The buffer is automatically reused
when explicitly released or when the smart pointer object is destroyed.
*/

#include"basler.h"

Basler::Basler()
	:camera(CTlFactory::GetInstance().CreateFirstDevice())
{



	//カメラオブジェクトの作成
	cout << "Creating Camera..." << endl;
	cout << "Using device,,, " << endl;

	cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
	camera.Open();

	//fpsの指定
	//camera.AcquisitionFrameRateEnable.SetValue(true);
	//camera.AcquisitionFrameRate.SetValue(1000.0);

	//トリガーモードON
	//camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
	//camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
	//camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line3);


	cout << "Creating and initialized Camera" << endl;
	//バッファの最大サイズを設定
	camera.MaxNumBuffer = 20;


	GenApi::CIntegerPtr _width(camera.GetNodeMap().GetNode("Width"));
	GenApi::CIntegerPtr _height(camera.GetNodeMap().GetNode("Height"));
	//ここで撮像する画像サイズを設定している
	_width->SetValue(CAMERA_WIDTH);
	_height->SetValue(CAMERA_HEIGHT);

	cout << _width->GetValue() << std::endl;
	cout << _height->GetValue() << std::endl;

	//露光時間の設定
	GenApi::CFloatPtr ExposureTime(camera.GetNodeMap().GetNode("ExposureTime"));
	//cout << ExposureTime->GetValue() << endl;
	//double newExposureTime = ExposureTime->GetValue() / 5;//デフォルト5000なので/5で1000μs
	ExposureTime->SetValue(10000);






}

Basler::~Basler()
{
	camera.StopGrabbing();
	camera.Close();
	// Releases all pylon resources. 
	//PylonTerminate();
}

void Basler::camera_init()
{
	std::cout << "カメラで使用するパラメタを先に設定..." << std::endl;

	//グラブストラテジーとよばれるフラグを入れる
	//ちなみに今は取得してある画像の中で常に最新のものを拾ってきている
	//GrabStrategy_LatestImagesこのフラグとの差を時間あるときに検証
	//camera.StartGrabbing(GrabStrategy_LatestImageOnly);
	//ここのフラグはかなり重要
	camera.StartGrabbing(GrabStrategy_LatestImages);
	//openCV用のBGR配列の画像に設定を変更するための準備
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;

	GenApi::CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
	GenApi::CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
	Mat cv_img(width->GetValue(), height->GetValue(), CV_8UC3);

	img_col = cv_img.clone();
	//camera.StopGrabbing();

}

bool Basler::cap_color(cv::Mat &img_col)
{
	//openCV用のBGR配列の画像に設定を変更するための準備

	camera.IsGrabbing();
	//画像の転送待ち．(待ち時間，画像を格納するポインタ，タイムアウトハンドラ)
	camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
	if (ptrGrabResult->GrabSucceeded()) {
		formatConverter.Convert(pylonImage, ptrGrabResult);

		img_col = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
		//RGBの順番で入っているので，opencv向けにBGRの順番に直す
		//cv::cvtColor(cv_img, cv_img, cv::COLOR_RGB2BGR);
	}
	return true;

}

bool Basler::cap_gray(cv::Mat &img_gray)
{

	if (camera.IsGrabbing())
	{
		//画像の転送待ち．(待ち時間，画像を格納するポインタ，タイムアウトハンドラ)
		camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
		if (ptrGrabResult->GrabSucceeded()) {
			formatConverter.Convert(pylonImage, ptrGrabResult);
			img_col = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
			//RGBの順番で入っているので，opencv向けにBGRの順番に直す
			//cv::cvtColor(cv_img, cv_img, cv::COLOR_RGB2BGR);
			cv::cvtColor(img_col, img_gray, CV_BGR2GRAY);
		}
	}
	//camera.StopGrabbing();
	return true;
}