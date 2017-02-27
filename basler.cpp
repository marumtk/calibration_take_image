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



	//�J�����I�u�W�F�N�g�̍쐬
	cout << "Creating Camera..." << endl;
	cout << "Using device,,, " << endl;

	cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;
	camera.Open();

	//fps�̎w��
	//camera.AcquisitionFrameRateEnable.SetValue(true);
	//camera.AcquisitionFrameRate.SetValue(1000.0);

	//�g���K�[���[�hON
	//camera.TriggerSelector.SetValue(Basler_UsbCameraParams::TriggerSelector_FrameStart);
	//camera.TriggerMode.SetValue(Basler_UsbCameraParams::TriggerMode_On);
	//camera.TriggerSource.SetValue(Basler_UsbCameraParams::TriggerSource_Line3);


	cout << "Creating and initialized Camera" << endl;
	//�o�b�t�@�̍ő�T�C�Y��ݒ�
	camera.MaxNumBuffer = 20;


	GenApi::CIntegerPtr _width(camera.GetNodeMap().GetNode("Width"));
	GenApi::CIntegerPtr _height(camera.GetNodeMap().GetNode("Height"));
	//�����ŎB������摜�T�C�Y��ݒ肵�Ă���
	_width->SetValue(CAMERA_WIDTH);
	_height->SetValue(CAMERA_HEIGHT);

	cout << _width->GetValue() << std::endl;
	cout << _height->GetValue() << std::endl;

	//�I�����Ԃ̐ݒ�
	GenApi::CFloatPtr ExposureTime(camera.GetNodeMap().GetNode("ExposureTime"));
	//cout << ExposureTime->GetValue() << endl;
	//double newExposureTime = ExposureTime->GetValue() / 5;//�f�t�H���g5000�Ȃ̂�/5��1000��s
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
	std::cout << "�J�����Ŏg�p����p�����^���ɐݒ�..." << std::endl;

	//�O���u�X�g���e�W�[�Ƃ�΂��t���O������
	//���Ȃ݂ɍ��͎擾���Ă���摜�̒��ŏ�ɍŐV�̂��̂��E���Ă��Ă���
	//GrabStrategy_LatestImages���̃t���O�Ƃ̍������Ԃ���Ƃ��Ɍ���
	//camera.StartGrabbing(GrabStrategy_LatestImageOnly);
	//�����̃t���O�͂��Ȃ�d�v
	camera.StartGrabbing(GrabStrategy_LatestImages);
	//openCV�p��BGR�z��̉摜�ɐݒ��ύX���邽�߂̏���
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;

	GenApi::CIntegerPtr width(camera.GetNodeMap().GetNode("Width"));
	GenApi::CIntegerPtr height(camera.GetNodeMap().GetNode("Height"));
	Mat cv_img(width->GetValue(), height->GetValue(), CV_8UC3);

	img_col = cv_img.clone();
	//camera.StopGrabbing();

}

bool Basler::cap_color(cv::Mat &img_col)
{
	//openCV�p��BGR�z��̉摜�ɐݒ��ύX���邽�߂̏���

	camera.IsGrabbing();
	//�摜�̓]���҂��D(�҂����ԁC�摜���i�[����|�C���^�C�^�C���A�E�g�n���h��)
	camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
	if (ptrGrabResult->GrabSucceeded()) {
		formatConverter.Convert(pylonImage, ptrGrabResult);

		img_col = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
		//RGB�̏��Ԃœ����Ă���̂ŁCopencv������BGR�̏��Ԃɒ���
		//cv::cvtColor(cv_img, cv_img, cv::COLOR_RGB2BGR);
	}
	return true;

}

bool Basler::cap_gray(cv::Mat &img_gray)
{

	if (camera.IsGrabbing())
	{
		//�摜�̓]���҂��D(�҂����ԁC�摜���i�[����|�C���^�C�^�C���A�E�g�n���h��)
		camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);
		if (ptrGrabResult->GrabSucceeded()) {
			formatConverter.Convert(pylonImage, ptrGrabResult);
			img_col = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
			//RGB�̏��Ԃœ����Ă���̂ŁCopencv������BGR�̏��Ԃɒ���
			//cv::cvtColor(cv_img, cv_img, cv::COLOR_RGB2BGR);
			cv::cvtColor(img_col, img_gray, CV_BGR2GRAY);
		}
	}
	//camera.StopGrabbing();
	return true;
}