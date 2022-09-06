#include <iostream>
#include <opencv2/opencv.hpp>
#include <tuple>
#include "histogram.hpp"
#include <string>

const std::string appKeys =
	"{help h usage ?				|      |   }"
	"{@image1								|<none>|   }"
;

const int fps = 30;

cv::VideoWriter* configureVideoWriter(std::string&& filename) {
	int codec = cv::VideoWriter::fourcc('a','v','c','1');
	cv::VideoWriter* videoWriter = new cv::VideoWriter();

	videoWriter->open(
		filename,codec,20.0,cv::Size(640.0,480.0),true
	);
	return videoWriter;
}

int validateInputCLI(cv::CommandLineParser* parser) {
	parser->about("Application v1.0");

	if(parser->has("help"))
	{
		parser->printMessage();
		return 1;
	}

	if(!parser->check())
	{
		parser->printErrors();
		return -1;
	}

	return 0;
}


std::tuple<cv::Mat, cv::Mat> getColorHist(cv::Mat& frame) {
	cv::Mat gray; 
	cv::Mat dst; 
	
	cv::cvtColor(frame,gray, cv::COLOR_BGR2GRAY);

	cv::equalizeHist(gray, dst);
	return std::make_tuple(gray, dst);
}

cv::Mat readImage(std::string&& filename) {
	cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);
	if(img.empty()) {
		throw std::logic_error("img is empty");
	}
	return img;
}

double compareImagesByHisto(cv::Mat& frame,cv::Mat& target){
	cv::Mat hsv_src, hsv_target;
	
	cv::cvtColor(frame, hsv_src, cv::COLOR_BGR2HSV);	
	cv::cvtColor(target, hsv_target, cv::COLOR_BGR2HSV);	

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };
	const float* ranges[] = { h_ranges, s_ranges };
	int h_bins = 50, s_bins = 60;
	int histSize[] = { h_bins, s_bins };
	int channels[] = { 0, 1 };

	cv::Mat hist_src, hist_target;
	cv::calcHist(&hsv_src, 1, channels, cv::Mat(), hist_src, 2, histSize, ranges, true, false);
	normalize( hist_src, hist_src, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
	cv::calcHist(&hsv_target, 1, channels, cv::Mat(), hist_target, 2, histSize, ranges, true, false);
	normalize( hist_target, hist_target, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

	return cv::compareHist(hist_src, hist_target, 0);		
}

int main(int argc, char** argv) {
	cv::CommandLineParser* parser = new cv::CommandLineParser(argc,argv, appKeys);

	switch(validateInputCLI(parser)){
		case 1:
			return 0;
		case -1:
			return -1;
	}

	cv::Mat target = readImage("images/bcp.jpg");
	cv::Mat frame;
	cv::VideoCapture vid(0);

	std::cout << "Loading VideoWriter..." << std::endl;
	cv::VideoWriter* writer = configureVideoWriter("output2.mp4");
	if(!vid.isOpened()) {
		std::cerr << "Video not opened" << std::endl;
		return -1;
	}
	if(!(writer->isOpened())) {
		std::cerr << "Video not opened" << std::endl;
		return -1;
	}

	while(true)
	{
		vid >> frame;

		if(frame.empty()) {
			throw std::logic_error("IMG is empty");
		}

		double result_corr = compareImagesByHisto( frame, target );

		cv::putText(
			frame, 
			std::to_string(result_corr),
			cv::Point(5, frame.rows / 2),
			cv::FONT_HERSHEY_DUPLEX,
			1.0,
			CV_RGB(118,185,0),
			1
		);
		writer->write(frame);

		cv::imshow("Webcam", frame);
		cv::imshow("BCP", target);

		//cv::Mat gray, dst;
		//std::tie(gray, dst) = getColorHist(frame);
		//cv::imshow("Gray", gray);
		//cv::imshow("Equalize", dst);
		
		//Histo::drawHistogram(frame);
		char c = (char)cv::waitKey(1);
		if(c == 27) {
			break;
		}
	}

	vid.release();
	writer->release();
	free(writer);
	cv::destroyAllWindows();

  return 0;
}
