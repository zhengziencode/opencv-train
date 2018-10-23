//This is a library made by Mojo.
#ifndef __OPENCVCLASS__
#define __OPENCVCLASS__

#include <stdio.h>
#include <string>
#include "opencv2/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv/cv.h"
#include "opencv2/face.hpp"

#ifdef _DEBUG
#pragma comment(lib,"opencv_world300d.lib")
#pragma comment(lib,"opencv_ts300d.lib")
#pragma comment(lib,"opencv_face300d.lib")
#else
#pragma comment(lib,"opencv_world300.lib")
#pragma comment(lib,"opencv_ts300.lib")
#pragma comment(lib,"opencv_face300.lib")
#endif

using namespace cv;
using namespace cv::face;
CascadeClassifier ccfect;
namespace zze{
	enum Options{ CORRODE = 1, CANNY, BLUR, GRAY,DETECT};
	static int m_count=1;//For Func GetROI_Faces

	class PhotoOperation{
		VideoWriter videoWriter;
		char m_defaultWinName[256];
		char m_cascadeName[256];
		char m_defaultVideoName[256];
		int m_nFlag;
		int m_nFaces;
		int m_nSaveToVideoFlag;
	public:
		PhotoOperation();
		PhotoOperation(char *cascadeName);
		Mat Corrode(Mat srcImage,int Value=15);//Corrode the photo
		Mat Dilate(Mat srcImage, int Value=12);//dilate the photo
		Mat CannyDetect(Mat srcImage);//Operate the canny of pic
		Mat BlurPic(Mat srcImage,int Value=7);//Blur the pic
		Mat CombinePic(Mat srcImage1, Mat srcImage2,float fRate=0.5);//To combine pics 
		Mat TransformToGray(Mat srcImage);//Transform the pic to gray pic
		Mat Detect(Mat srcImage);//Face detcet
		Mat AllCalculateOperation(Mat srcImage);//In order to operate all pic operation and Return the destination of result
		Mat GetROI_Faces(Mat srImage,std::string filePath="noneedwrite");//Get Region of interesting of faces area. The second argument only given the detected file's path rather than any obvious file names
		int GetAmounts();//if flag==1
		void InitWriteToVideo(Size &size,char *FileName = NULL, double rate = 25.0, bool isColor = true);//save to video
		void WriteToVideo(Mat srcImg);//company with InitWriteToVideo Function
		bool PlayVideo(char *cFileName,char *cWindowName=NULL);//FileName,WindowName
		bool PlayVideo(char *cFileName, int nOption ,char *cWindowName=NULL);//FileName,Operate Option,WindowName
		~PhotoOperation();
	};
}
zze::PhotoOperation::PhotoOperation() :m_nFaces(-1),m_nSaveToVideoFlag(-1){
	memset(m_defaultWinName, 0, 256);
	memcpy(m_defaultWinName, "Mojo Opencv Program", 256);
	memcpy(m_defaultVideoName, "Capture.avi", 256);
}
zze::PhotoOperation::PhotoOperation(char *cascadeName):m_nFaces(0),m_nSaveToVideoFlag(-1){
	memset(m_defaultWinName, 0, 256);
	memcpy(m_defaultWinName, "Mojo Opencv Program", 256);
	memcpy(m_cascadeName, cascadeName, strlen(cascadeName) + 1);
	memcpy(m_defaultVideoName, "Capture.avi", 256);
}
Mat zze::PhotoOperation::Corrode(Mat srcImage,int Value){
	Mat dstResult;
	Mat element = getStructuringElement(MORPH_RECT, Size(Value, Value));

	erode(srcImage, dstResult, element);

	return dstResult;
}
Mat zze::PhotoOperation::Dilate(Mat srcImage, int Value){
	Mat dstResult;
	Mat element = getStructuringElement(MORPH_RECT, Size(Value, Value));

	dilate(srcImage, dstResult, element);

	return dstResult;
}
Mat zze::PhotoOperation::CannyDetect(Mat srcImage){
	Mat grayImage, edgeReduce;
	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);
	blur(grayImage, edgeReduce, Size(7, 7));

	Canny(edgeReduce, edgeReduce, 0, 3, 3);

	return edgeReduce;
}
Mat zze::PhotoOperation::BlurPic(Mat srcImage,int Value){
	Mat dstResult;
	blur(srcImage, dstResult, Size(Value, Value));
	return dstResult;
}
Mat zze::PhotoOperation::CombinePic(Mat srcImage1, Mat srcImage2,float fRate){
	resize(srcImage2, srcImage2, Size(srcImage1.cols, srcImage1.rows));
	Mat dstPic;
	addWeighted(srcImage1, fRate, srcImage2, 1-fRate, 0.0, dstPic);
	return dstPic;
}

Mat zze::PhotoOperation::TransformToGray(Mat srcImage){
	Mat dstResult;
	cvtColor(srcImage, dstResult, CV_BGR2GRAY);
	return dstResult;
}

Mat zze::PhotoOperation::Detect(Mat srcImage){
	Mat grayImage;
	Mat dstImg=srcImage;


	std::vector<Rect> rect;

	cvtColor(srcImage, grayImage, CV_BGR2GRAY);//to gray pic to improve speed of recg
	equalizeHist(grayImage, grayImage);//直方图均衡
	bool ret = ccfect.load(m_cascadeName);//加载级联分类器
	if (!ret)
	{
		return srcImage;
	}
	ccfect.detectMultiScale(grayImage, rect, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
	m_nFaces = rect.size();
	//printf("Detected %d human faces\n", rectFaces.size());
	for (int i = 0; i < rect.size();i++)
	{
		Point center(rect[i].x + rect[i].width*0.5, rect[i].y + rect[i].height*0.5);
		ellipse(dstImg, center, Size(rect[i].width*0.5, rect[i].height*0.5), 0, 0, 360, Scalar(255, 0, 255), 4, 8, 0);
	}
	return dstImg;
}
Mat zze::PhotoOperation::AllCalculateOperation(Mat srcImage){
	Mat dstImage;
	Mat tempImg = this->Corrode(srcImage);
	Mat tempImg2 = this->BlurPic(srcImage);
	dstImage = tempImg - tempImg2;
	return dstImage;
}
Mat zze::PhotoOperation::GetROI_Faces(Mat srcImage,std::string filePath){//using detect func first
	Mat grayImage, dstImage;
	std::vector<Rect> rect;
	if (m_nFaces==0)
	{
		printf("Have no Detect source file!Please load cascade file first!");
		return srcImage;
	}

	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);//turn to gray pic to improve speed
	equalizeHist(grayImage, grayImage);
	CascadeClassifier ccfect;
	ccfect.load(m_cascadeName);
	ccfect.detectMultiScale(grayImage, rect, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	char buf[10] = { 0 };
	printf("%d detected\n", rect.size());
	for (int i = 0; i < rect.size();i++)
	{
		std::string fileNamae = filePath + buf;
		dstImage = srcImage(Range(rect[i].y, rect[i].y + rect[i].height), Range(rect[i].x, rect[i].x + rect[i].width));
		if (fileNamae!="noneedwrite")
		{
			sprintf_s(buf, "\\%d.jpg", m_count);
			imwrite(fileNamae, dstImage);
		}
		m_count++;
	}
	return dstImage;
}
int zze::PhotoOperation::GetAmounts(){
	if (m_nFlag==-1)
	{
		return -1;
	}
	int temp = m_nFaces;
	m_nFaces = 0;
	return temp;
}
void zze::PhotoOperation::InitWriteToVideo(Size &size,char *FileName,double rate,bool isColor){
	if (m_nSaveToVideoFlag==1)
	{
		return;
	}
	if (!FileName)
	{
		FileName = m_defaultVideoName;
	}
	m_nSaveToVideoFlag = 1;
	if (!videoWriter.isOpened())
	{
		videoWriter.open(FileName, VideoWriter::fourcc('X', 'V', 'I', 'D'), rate, size, isColor);
	}
}
void zze::PhotoOperation::WriteToVideo(Mat srcImg){
	if (m_nSaveToVideoFlag==1)
	{
		videoWriter << srcImg;
	}
}
bool zze::PhotoOperation::PlayVideo(char *cFileName,char *cWindowName){
	bool ret = false;
	if (cWindowName==NULL)
	{
		cWindowName = m_defaultWinName;
	}
	do 
	{
		VideoCapture vVid(cFileName);
		if (!vVid.isOpened())
		{
			break;
		}
		double fps;
		int spf = 0;

		fps = vVid.get(CV_CAP_PROP_FPS);
		spf = (int)1000 / fps;

		while (true)
		{
			Mat Frame;
			vVid >> Frame;
			imshow(cWindowName, Frame);
			int Key = waitKey(spf);
			if (Key==27)
			{
				break;
			}
		}
		ret = false;
	} while (false);

	return ret;
}
bool zze::PhotoOperation::PlayVideo(char *cFileName, int nOption, char *cWindowName){
	bool ret = false;
	if (cWindowName == NULL)
	{
		cWindowName = m_defaultWinName;
	}
	do 
	{
		if (nOption<CORRODE ||nOption>DETECT)
		{
			break;
		}
		VideoCapture vVid(cFileName);
		int t;
		if (!(t=vVid.isOpened()))
		{
			break;
		}
		double fps;
		int spf = 0;

		fps = vVid.get(CV_CAP_PROP_FPS);
		spf = (int)1000 / fps;

		while (true)
		{
			Mat Frame,OperateFrame;
			vVid >> Frame;
			if (Frame.empty())
			{
				break;
			}
			switch (nOption)
			{
			case CORRODE:
				OperateFrame = this->Corrode(Frame);
				break;
			case CANNY:
				OperateFrame = this->CannyDetect(Frame);
				break;
			case BLUR:
				OperateFrame = this->BlurPic(Frame);
				break;
			case GRAY:
				OperateFrame = this->TransformToGray(Frame);
				break;
			case DETECT:
				if (m_nFlag==1)
				{
					OperateFrame = this->Detect(Frame);
				}
				else
				{
					printf("Error,Input CascadeName!");
				}
				break;
			default:
				printf("A serious Error!\n");
				break;
			}
			namedWindow(cWindowName, WINDOW_NORMAL);
			imshow(cWindowName, OperateFrame);
			int Key = waitKey(spf);
			if (Key == 27)
			{
				break;
			}
		}
		ret = true;
	} while (false);
	return ret;
}
zze::PhotoOperation::~PhotoOperation(){
	videoWriter.release();
}
#endif