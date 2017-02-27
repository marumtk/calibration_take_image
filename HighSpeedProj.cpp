//-----------------------------------------------------------------------------
// Class for High-speed Projector
// -- Gaku Narita, 2015/11/06
//-----------------------------------------------------------------------------


#include "HighSpeedProj.h"

//------------------------------------------
// メンバ関数
//------------------------------------------

//---- コンストラクタ
HighSpeedProj::HighSpeedProj() :
	iStop(0),
	iBoardID(0),
	iChannel(0),
	ulSize(0),
	pBuf(NULL),
	ulLEDCtrlDelay(PROJ_LED_DELAY),
	ulFrameTrigDelay(PROJ_TRIG_DELAY),
	ulDataSizeInBuffer(0),
	input_img_count(0)
{
	//---- pPcieLibraryをnewする
	pPcieLibrary = new CStdLibrary();

	//---- Set System Parameters
	stSystemParam.FrameRate = PROJ_FRAME_PER_SEC;
	stSystemParam.ValidBits = PROJ_VALID_BITS;
	stSystemParam.MirrorMode = PROJ_MIRROR_MODE;
	stSystemParam.CompData = PROJ_COMP_DATA;
	stSystemParam.FlipMode = PROJ_FLIP_MODE;

	//---- SetProcessWorkingSetSize
	if (!SetProcessWorkingSetSize(::GetCurrentProcess(), (1000UL * 1024 * 1024), (1500UL * 1024 * 1024))) {
		printf(("SetProcessWorkingSetSize Failed!"));
	}
}

//---- プロジェクタを接続する
bool HighSpeedProj::connectProj() {

	ULONG ulTmp = 0;

	iRet = pPcieLibrary->ConnectToBoard(iBoardID);
	if (iRet != STATUS_SUCCESSFUL)
	{
		// Open Error
		printf("ConnectToBoard Failed!\r\n");
	}

	iRet = pPcieLibrary->ReadBoardRegister(iBoardID, 0x01, 0x00, &ulTmp);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("ReadBoardRegister Error!\r\n"));
		errorCloseProj();
	}
	else {
		printf("Version = %X\r\n", ulTmp);
	}

	iRet = pPcieLibrary->ReadBoardRegister(iBoardID, 0x00, IPCORE_REG_PCIE_LINK_STAT, &ulTmp);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("ReadBoardRegister Error!\r\n"));
		errorCloseProj();
	}
	else {
		printf("Link State = %X\r\n", ulTmp);
	}

	iRet = pPcieLibrary->SetReset(iBoardID);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("SetReset Error!\r\n"));
		errorCloseProj();
	}

	iRet = pPcieLibrary->SetSystemParam(iBoardID, stSystemParam);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("SetSystemParam Error!\r\n"));
		errorCloseProj();
	}

	SYSTEM_STATUS stSystemStatus;
	pPcieLibrary->GetStatus(iBoardID, &stSystemStatus);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("GetStatus Error!"));
		errorCloseProj();
	}
	else {
		printf(("Output dfFrameTime = %.2fus (=%.2ffps), dfMinDarkTime = %.2fus\r\n"), stSystemStatus.dfFrameTime, 1000000.0 / stSystemStatus.dfFrameTime, stSystemStatus.dfMinDarkTime);
	}

	if (stSystemStatus.dfMinDarkTime <= 1.0) {
		printf("~-~ WARNING: dfMinDarkTime is less than 1.0us\r\n");
	}

	iRet = pPcieLibrary->WriteBoardRegister(iBoardID, 0x01, PCIE_LED_ENABLE_DELAY, ulLEDCtrlDelay);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("WriteBoardRegister Error!\r\n"));
		errorCloseProj();
	}

	iRet = pPcieLibrary->WriteBoardRegister(iBoardID, 0x01, PCIE_FRAME_TRIG_DELAY, ulFrameTrigDelay);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("WriteBoardRegister Error!\r\n"));
		errorCloseProj();
	}

	//ここがバッファのサイズ操作するための場所
	iRet = pPcieLibrary->RegisterSGDMA(iBoardID, iChannel, 0, 1 * FRAME_SIZE);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("RegisterSGDMA Error, iRet = %X!\r\n"), iRet);
		errorCloseProj();
	}

	printf("Start DMA:\r\n");

	iRet = pPcieLibrary->StartSGDMA(iBoardID, iChannel, FALSE);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("StartSGDMA Error, iRet = %X!\r\n"), iRet);
		errorCloseProj();
	}

	//---- 初期接続, Strart Output
	bool bOut = false; // 初期接続完了フラグ
	ULONGLONG ullTotolSize = 0;

	while (!bOut) {

		iRet = pPcieLibrary->ReadBoardRegister(iBoardID, 0x00, 0x164, &ulTmp);
		if (iRet != STATUS_SUCCESSFUL) {
			printf(("ReadBoardRegister Error!\r\n"));
			errorCloseProj();
		}

		ulDataSizeInBuffer = ulTmp * 4096;

		if (ullTotolSize - ulDataSizeInBuffer >= FRAME_SIZE) {
			printf(("Output Start, Transfer end: %d Byte\r\n"), ullTotolSize - ulDataSizeInBuffer);

			bOut = true;

			// Start output
			iRet = pPcieLibrary->ReadBoardRegister(iBoardID, 0x01, 0x04, &ulTmp);
			if (iRet != STATUS_SUCCESSFUL) {
				printf(("ReadBoardRegister Error!\r\n"));
				errorCloseProj();
			}

			ulTmp |= OUTPUT_START_VAL;

			iRet = pPcieLibrary->WriteBoardRegister(iBoardID, 0x01, 0x04, ulTmp);
			if (iRet != STATUS_SUCCESSFUL) {
				printf(("WriteBoardRegister Error!\r\n"));
				errorCloseProj();
			}
		}

		iRet = pPcieLibrary->GetBuffer(iBoardID, iChannel, &pBuf, &ulSize);
		if (iRet != STATUS_SUCCESSFUL) {
			printf(("GetBuffer Error!\r\n"));
			errorCloseProj();
		}

		if ((pBuf != NULL) && (ulSize >= FRAME_SIZE)) {
			iRet = pPcieLibrary->PostDataSize(iBoardID, iChannel, FRAME_SIZE);
			if (iRet != STATUS_SUCCESSFUL) {
				printf(("PostDataSize Error!\r\n"));
				errorCloseProj();
			}
			ullTotolSize += FRAME_SIZE;
		}
		else {
			Sleep(1);
		}

	}

	return true;



}


//---- エラー時にプロジェクタの接続を解除する
void HighSpeedProj::errorCloseProj() {

	iRet = pPcieLibrary->SetFloat(iBoardID, 0);
	if (iRet != STATUS_SUCCESSFUL)
	{
		printf(("SetFloat Error!"));
		errorCloseProj();
	}

	iRet = pPcieLibrary->DisconnectFromBoard(iBoardID);
	if (iRet != STATUS_SUCCESSFUL)
	{
		// Close Error
	}

	delete pPcieLibrary;
}

//---- デストラクタ
HighSpeedProj::~HighSpeedProj() {

	iRet = pPcieLibrary->StopSGDMA(iBoardID, iChannel);
	if (iRet != STATUS_SUCCESSFUL)
	{
		printf(("StopSGDMA Error!"));
		errorCloseProj();
	}

	iRet = pPcieLibrary->UnregisterSGDMA(iBoardID, iChannel);
	if (iRet != STATUS_SUCCESSFUL)
	{
		printf(("UnregisterSGDMA Error!"));
		errorCloseProj();
	}

	iRet = pPcieLibrary->SetFloat(iBoardID, 0);
	if (iRet != STATUS_SUCCESSFUL)
	{
		printf(("SetFloat Error!"));
		errorCloseProj();
	}

	iRet = pPcieLibrary->DisconnectFromBoard(iBoardID);
	if (iRet != STATUS_SUCCESSFUL)
	{
		// Close Error
	}

	delete pPcieLibrary;
}

//---- 映像を送る
void HighSpeedProj::sendImage(cv::Mat& img) {

	iRet = pPcieLibrary->GetBuffer(iBoardID, iChannel, &pBuf, &ulSize);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("GetBuffer Error!\r\n"));
		errorCloseProj();
	}
	input_img_count++; //カウント

	if ((pBuf != NULL) && (ulSize >= FRAME_SIZE)) {

		std::memcpy(pBuf, img.data, FRAME_SIZE);

		iRet = pPcieLibrary->PostDataSize(iBoardID, iChannel, FRAME_SIZE);
		if (iRet != STATUS_SUCCESSFUL)
		{
			printf(("PostDataSize Error!\r\n"));
			errorCloseProj();
		}
	}
	else { //---- やり直し
		   //std::cout << "test" << std::endl;
		sendImageloop(img);
	}
}

//---- 再帰用の関数
void HighSpeedProj::sendImageloop(cv::Mat& img) {

	iRet_loop = pPcieLibrary->GetBuffer(iBoardID, iChannel, &pBuf, &ulSize);
	if (iRet_loop != STATUS_SUCCESSFUL) {
		printf(("GetBuffer Error!\r\n"));
		errorCloseProj();
	}

	if ((pBuf != NULL) && (ulSize >= FRAME_SIZE)) {

		std::memcpy(pBuf, img.data, FRAME_SIZE);

		iRet_loop = pPcieLibrary->PostDataSize(iBoardID, iChannel, FRAME_SIZE);
		if (iRet_loop != STATUS_SUCCESSFUL)
		{
			printf(("PostDataSize Error!\r\n"));
			errorCloseProj();
		}
	}
	else {
		sendImageloop(img);
	}
}

//---- GetStatus
void HighSpeedProj::getstatus(SYSTEM_STATUS &_stSystemStatus)
{
	iRet = pPcieLibrary->GetStatus(0,&_stSystemStatus);
	if (iRet != STATUS_SUCCESSFUL) {
		printf(("GetStatus Error!\r\n"));
		errorCloseProj();
	}
}

void HighSpeedProj::GetInputImgNum(std::size_t &_input_frame_num)
{
	_input_frame_num = input_img_count;
}
