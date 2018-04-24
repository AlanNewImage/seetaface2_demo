
#include <opencv2/opencv.hpp>
#include <seeta/FaceDetector2.h>
#include <seeta/Struct_cv.h>
#include <seeta/PointDetector2.h>
#include <seeta/FaceRecognizer.h>
//using namespace cv;


#ifdef _WIN32
#include <winsock2.h>
#include <time.h>
#else
#include <sys/time.h>
#endif

unsigned long long GetCurrentTimeMsec()
{
#ifdef _WIN32
	struct timeval tv;
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tv.tv_sec = clock;
	tv.tv_usec = wtm.wMilliseconds * 1000;
	return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#endif
}

//�������ͼ��
void test_FD_picture() {
	//ģ�ͳ�ʼ��
	seeta::FaceDetector2 FD("../../bindata/SeetaFaceDetector2.0.ats");

	//����ImageDataͼ������
	cv::Mat mat;
	mat = cv::imread("../girls.jpg");
	seeta::cv::ImageData image = mat;

	double fd_start_time = GetCurrentTimeMsec();

	//�������
	int num;    // save the number of detected faces
	SeetaRect *face = FD.Detect(image, &num);

	double fd_end_time = GetCurrentTimeMsec();
	double fd_time = fd_end_time - fd_start_time;
	std::cout <<"face detection::"<<"width:"<<image.width<<" height:"<<image.height<<" num:"<<num<<" cost time:" << fd_time<< "ms" << std::endl;

	//������������ʾ
	for (int i = 0; i < num; i++,face++) {
		cv::Rect face_box = cv::Rect((int)face->x, face->y, face->width, face->height);
		cv::rectangle(mat, face_box, cv::Scalar(0, 0, 255), 2, 8, 0);
	//	std::cout << "Face(" << i << "): " << "(" << face_box.x << ", " << face_box.y << ", " << face_box.width << ", " << face_box.height << ")" << std::endl;
	}
	cv::putText(mat, "face detection::width:"+std::to_string(image.width)+" height:"+std::to_string(image.height)+" num:"+std::to_string(num)+" cost time:"+std::to_string(fd_time)+"ms", cv::Point(10, 20), 0.5, 0.5, cv::Scalar(0, 0, 255));

	cv::imshow("face_detection", mat);
	cv::waitKey(0);

	cv::destroyAllWindows();
	return;
}


//�������ʵʱ��Ƶ��
void test_FD_video() {
	//ģ�ͳ�ʼ��
	seeta::FaceDetector2 FD("../../bindata/SeetaFaceDetector2.0.ats");

	//������ͷ
	cv::VideoCapture videoCapture(0);
	if (!videoCapture.isOpened()) {
		return;
	}

	cv::Mat frame;
	videoCapture >> frame;
	while (cv::waitKey(1) != 'q') {
		videoCapture >> frame;
		if (frame.empty()) {
			break;
		}

		//����ImageDataͼ������
		seeta::cv::ImageData image = frame;

		double fd_start_time = GetCurrentTimeMsec();

		//�������
		int num;    // save the number of detected faces
		SeetaRect *face = FD.Detect(image, &num);

		double fd_end_time = GetCurrentTimeMsec();
		double fd_time = fd_end_time - fd_start_time;
		std::cout << "face detection::" << "width:" << image.width << " height:" << image.height << " num:" << num << " cost time:" << fd_time << "ms" << std::endl;

		//������������ʾ
		for (int i = 0; i < num; i++, face++) {
			cv::Rect face_box = cv::Rect((int)face->x, face->y, face->width, face->height);
			cv::rectangle(frame, face_box, cv::Scalar(0, 0, 255), 2, 8, 0);
			//	std::cout << "Face(" << i << "): " << "(" << face_box.x << ", " << face_box.y << ", " << face_box.width << ", " << face_box.height << ")" << std::endl;
		}
		cv::putText(frame, "face detection::width:" + std::to_string(image.width) + " height:" + std::to_string(image.height) + " num:" + std::to_string(num) + " cost time:" + std::to_string(fd_time) + "ms", cv::Point(10, 20), 0.5, 0.5, cv::Scalar(0, 0, 255));

		cv::imshow("face_detection", frame);
		cv::waitKey(1);

	}

	cv::destroyAllWindows();
	return;
}


//��������ͼ��
void test_PD_picture() {
	//ģ�ͳ�ʼ��
	seeta::FaceDetector2 FD("../../bindata/SeetaFaceDetector2.0.ats");
	seeta::PointDetector2 PD("../../bindata/SeetaPointDetector2.0.pts5.ats");

	//����ImageDataͼ������
	cv::Mat mat;
	mat = cv::imread("../girls.jpg");
	seeta::cv::ImageData image = mat;

	double fd_start_time = GetCurrentTimeMsec();

	//�������
	int num;    // save the number of detected faces
	SeetaRect *face = FD.Detect(image, &num);
	double fd_end_time = GetCurrentTimeMsec();
	double fd_time = fd_end_time - fd_start_time;
	std::cout << "face detection::" << "width:" << image.width << " height:" << image.height << " num:" << num << " cost time:" << fd_time << "ms" << std::endl;

	//������������ʾ
	double fd_pd_time = fd_time;
	for (int i = 0; i < num; i++, face++) {
		//����������
		cv::Rect face_box = cv::Rect((int)face->x, face->y, face->width, face->height);
		cv::rectangle(mat, face_box, cv::Scalar(0, 0, 255), 2, 8, 0);

		double pd_start_time = GetCurrentTimeMsec();
		//�������������
		SeetaPointF *points;
		points = PD.Detect(image, *face);

		double pd_end_time = GetCurrentTimeMsec();
		double pd_time = pd_end_time - pd_start_time;
		std::cout << "points detection::" << "width:" << image.width << " height:" << image.height << " i:" << i << " cost time:" << pd_time << "ms" << std::endl;
		fd_pd_time += pd_time;

		//����������
		if (points) {
			for (int j = 0; j < PD.LandmarkNum(); j++) {
				cv::circle(mat, cvPoint(points[j].x, points[j].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
			}
		}
	}

	cv::putText(mat, "points detection::width:" + std::to_string(image.width) + " height:" + std::to_string(image.height) + " num:" + std::to_string(num) + " cost time:" + std::to_string(fd_pd_time) + "ms", cv::Point(10, 20), 0.5, 0.5, cv::Scalar(0, 0, 255));

	cv::imshow("points_detection", mat);
	cv::waitKey(0);

	cv::destroyAllWindows();
	return;
}


//��������ʵʱ��Ƶ��
void test_PD_video() {
	//ģ�ͳ�ʼ��
	seeta::FaceDetector2 FD("../../bindata/SeetaFaceDetector2.0.ats");
	seeta::PointDetector2 PD("../../bindata/SeetaPointDetector2.0.pts5.ats");

	//������ͷ
	cv::VideoCapture videoCapture(0);
	if (!videoCapture.isOpened()) {
		return;
	}

	cv::Mat frame;
	videoCapture >> frame;
	while (cv::waitKey(1) != 'q') {
		videoCapture >> frame;
		if (frame.empty()) {
			break;
		}

		//����ImageDataͼ������
		seeta::cv::ImageData image = frame;

		double fd_start_time = GetCurrentTimeMsec();

		//�������
		int num;    // save the number of detected faces
		SeetaRect *face = FD.Detect(image, &num);
		double fd_end_time = GetCurrentTimeMsec();
		double fd_time = fd_end_time - fd_start_time;
		std::cout << "face detection::" << "width:" << image.width << " height:" << image.height << " num:" << num << " cost time:" << fd_time << "ms" << std::endl;

		//������������ʾ
		double fd_pd_time = fd_time;
		for (int i = 0; i < num; i++, face++) {
			//����������
			cv::Rect face_box = cv::Rect((int)face->x, face->y, face->width, face->height);
			cv::rectangle(frame, face_box, cv::Scalar(0, 0, 255), 2, 8, 0);

			double pd_start_time = GetCurrentTimeMsec();
			//�������������
			SeetaPointF *points;
			points = PD.Detect(image, *face);

			double pd_end_time = GetCurrentTimeMsec();
			double pd_time = pd_end_time - pd_start_time;
			std::cout << "points detection::" << "width:" << image.width << " height:" << image.height << " i:" << i << " cost time:" << pd_time << "ms" << std::endl;
			fd_pd_time += pd_time;

			//����������
			if (points) {
				for (int j = 0; j < PD.LandmarkNum(); j++) {
					cv::circle(frame, cvPoint(points[j].x, points[j].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
				}
			}
		}

		cv::putText(frame, "points detection::width:" + std::to_string(image.width) + " height:" + std::to_string(image.height) + " num:" + std::to_string(num) + " cost time:" + std::to_string(fd_pd_time) + "ms", cv::Point(10, 20), 0.5, 0.5, cv::Scalar(0, 0, 255));

		cv::imshow("points_detection", frame);
		cv::waitKey(1);
	}

	cv::destroyAllWindows();
	return;
}


//1:1�����ȶ�
void test_FR_picture() {
	//ģ�ͳ�ʼ��
	seeta::FaceDetector2 FD("../../bindata/SeetaFaceDetector2.0.ats");
	seeta::PointDetector2 PD("../../bindata/SeetaPointDetector2.0.pts5.ats");
	seeta::FaceRecognizer2 FR("../../bindata/SeetaFaceRecognizer2.0.ats");

	//����ImageDataͼ������
	cv::Mat left_mat;
	left_mat = cv::imread("../1.jpg");
	seeta::cv::ImageData left_image = left_mat;

	cv::Mat right_mat;
	right_mat = cv::imread("../2.jpg");
	seeta::cv::ImageData right_image = right_mat;

	//�������
	int left_num;   
	SeetaRect *left_face = FD.Detect(left_image, &left_num);
	int right_num;
	SeetaRect *right_face = FD.Detect(right_image, &right_num);

	if (left_num != 0 && right_num != 0) {
		SeetaPointF left_points[5];
		PD.DetectEx(left_image, *left_face, left_points);

		SeetaPointF right_points[5];
		PD.DetectEx(right_image, *right_face, right_points);

		double fr_start_time = GetCurrentTimeMsec();
		//1:1�����ȶ�
		float similar = FR.Compare(left_image, left_points, right_image, right_points);
		std::cout << "similar:" << similar << std::endl;

		double fr_end_time = GetCurrentTimeMsec();
		double fr_time = fr_end_time - fr_start_time;
		std::cout << "1 vs 1 recognition::" << "cost time:" << fr_time << "ms;" << "similar:"<<similar << std::endl;
		cv::putText(left_mat, "similar:" + std::to_string(similar), cv::Point(10, 20), 0.5, 0.5, cv::Scalar(0, 0, 255));
	}

	cv::imshow("left_mat", left_mat);
	cv::imshow("right_mat", right_mat);
	cv::waitKey(0);

	FR.Clear();
	cv::destroyAllWindows();
	return;
}

//����ʶ��ʵʱ��Ƶ��
void test_FR_video() {
	//ģ�ͳ�ʼ��
	seeta::FaceDetector2 FD("../../bindata/SeetaFaceDetector2.0.ats");
	seeta::PointDetector2 PD("../../bindata/SeetaPointDetector2.0.pts5.ats");
	seeta::FaceRecognizer2 FR("../../bindata/SeetaFaceRecognizer2.0.ats");

	//----------------ע��׶�--------------------------//
	//����ImageDataͼ������
	cv::Mat left_mat;
	left_mat = cv::imread("../1.jpg");
	seeta::cv::ImageData left_image = left_mat;

	//�������
	int left_num;
	SeetaRect *left_face = FD.Detect(left_image, &left_num);
	if (left_num == 0) {
		std::cout << "δ��⵽������ע��ʧ��" << std::endl;
		return;
	}

	SeetaPointF *left_points = PD.Detect(left_image, *left_face);
	int flag = FR.Register(left_image, left_points);  // Reture -1 if failed.
	if (flag == -1) {
		std::cout << "����ע��ʧ��" << std::endl;
	}


	//----------------ʶ��׶�--------------------------//
	//������ͷ
	cv::VideoCapture videoCapture(0);
	if (!videoCapture.isOpened()) {
		return;
	}

	cv::Mat frame;
	videoCapture >> frame;
	while (cv::waitKey(1) != 'q') {
		videoCapture >> frame;
		if (frame.empty()) {
			break;
		}

		//����ImageDataͼ������
		seeta::cv::ImageData right_image = frame;

		double fd_start_time = GetCurrentTimeMsec();

		//�������
		int num;    // save the number of detected faces
		SeetaRect *face = FD.Detect(right_image, &num);
		double fd_end_time = GetCurrentTimeMsec();
		double fd_time = fd_end_time - fd_start_time;
		std::cout << "face detection::" << "width:" << right_image.width << " height:" << right_image.height << " num:" << num << " cost time:" << fd_time << "ms" << std::endl;

		//������������ʾ
		double fd_pd_time = fd_time;
		for (int i = 0; i < num; i++, face++) {
			//����������
			cv::Rect face_box = cv::Rect((int)face->x, face->y, face->width, face->height);
			cv::rectangle(frame, face_box, cv::Scalar(0, 0, 255), 2, 8, 0);

			double pd_start_time = GetCurrentTimeMsec();
			//�������������
			SeetaPointF *right_points;
			right_points = PD.Detect(right_image, *face);

			double pd_end_time = GetCurrentTimeMsec();
			double pd_time = pd_end_time - pd_start_time;
			std::cout << "points detection::" << "width:" << right_image.width << " height:" << right_image.height << " i:" << i << " cost time:" << pd_time << "ms" << std::endl;
			fd_pd_time += pd_time;

			double fr_start_time = GetCurrentTimeMsec();
			//����ʶ��
			float similar = 0;
			int index = FR.Recognize(right_image, right_points, &similar);
			cv::putText(frame, "similar:" + std::to_string(similar), cv::Point(10, 30), 0.5, 0.5, cv::Scalar(0, 0, 255));

			//float *similar = FR.RecognizeEx(right_image, right_points);
		/*	for (int i = 0; i < FR.MaxRegisterIndex(); ++i)
			{
				std::cout << "1.jpg vs. db/" << i + 1 << ".jpg: " << similar[i] << std::endl;
			}*/

			double fr_end_time = GetCurrentTimeMsec();
			double fr_time = fr_end_time - fr_start_time;
			std::cout << "face recognition::" << "width:" << right_image.width << " height:" << right_image.height << " i:" << i << " cost time:" << fr_time << "ms" << std::endl;
			fd_pd_time += fr_time;

			//����������
			if (right_points) {
				for (int j = 0; j < PD.LandmarkNum(); j++) {
					cv::circle(frame, cvPoint(right_points[j].x, right_points[j].y), 2, CV_RGB(0, 255, 0), CV_FILLED);
				}
			}
		}

		cv::putText(frame, "points detection::width:" + std::to_string(right_image.width) + " height:" + std::to_string(right_image.height) + " num:" + std::to_string(num) + " cost time:" + std::to_string(fd_pd_time) + "ms", cv::Point(10, 20), 0.5, 0.5, cv::Scalar(0, 0, 255));

		cv::imshow("points_detection", frame);
		cv::waitKey(1);
	}

	FR.Clear();
	cv::destroyAllWindows();
	return;
}

//�˵��б�
void display_menu() {
	printf("------------------����demo��ֻ���Ի�������------------\n");
	printf("ע�⣺ͼ�ν����е��q�����˳�����������\n");
	printf("1 - �������ͼƬ\n");
	printf("2 - �����������ͷ\n");
	printf("3 - ��������ͼƬ\n");
	printf("4 - ������������ͷ\n");
	printf("5 - 1:1ͼƬ����ʶ��\n");
	printf("6 - 1:1ʵʱ����ͷʶ��\n");
	printf("q - ����������\n");
	printf("0 - �˳�\n");
}


/*������*/
int main() {

	while (1) {
		display_menu(); //��ʾ�˵�
		int choice = getchar();   //��ȡ�û�����
		int flag;

		switch (choice) {
		case '1': std::cout << "ͼ��Ŀ¼:../girls.jpg;" << std::endl; test_FD_picture(); break;
		case '2': test_FD_video(); break;
		case '3': std::cout << "ͼ��Ŀ¼:../girls.jpg;" << std::endl; test_PD_picture(); break;
		case '4': test_PD_video(); break;
		case '5': std::cout << "ͼ��Ŀ¼:../1.jpg��2.jpg;" << std::endl; test_FR_picture(); break;
		case '6': std::cout << "ͼ��Ŀ¼:../1.jpg,���滻����ͼ��;" << std::endl; test_FR_video(); break;
		case '0': printf("�������\n"); exit(0);
		default: break;

		}
	}
	return 1;
}

