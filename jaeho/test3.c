#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>


#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>


using namespace cv;
using namespace std;


void write_on_web(int , int );

// #####################################
//HSV Color red_signal
/*
#define R_H_LOW 170 //or 0
#define R_H_HIGH 179 // 180 or 10
#define R_S_LOW 50 //50~70
#define R_S_HIGH 255
#define R_V_LOW 50
#define R_V_HIGH 255
*/
#define R_H_LOW 0 //or 0
#define R_H_HIGH 21 // 180 or 10
#define R_S_LOW 50 //50~70
#define R_S_HIGH 255
#define R_V_LOW 150
#define R_V_HIGH 255


//HSV Color green_signal
/*
#define G_H_LOW 45
#define G_H_HIGH 75
#define G_S_LOW 100
#define G_S_HIGH 255
#define G_V_LOW 50
#define G_V_HIGH 255
*/
#define G_H_LOW 45
#define G_H_HIGH 100
#define G_S_LOW 100
#define G_S_HIGH 255
#define G_V_LOW 100
#define G_V_HIGH 255


//HSV Color yellow stone
#define Y_H_LOW 20
#define Y_H_HIGH 30
#define Y_S_LOW 100
#define Y_S_HIGH 255
#define Y_V_LOW 100
#define Y_V_HIGH 255

bool r_flag = true;
bool g_flag = true;
bool y_flag = true;

// #####################################
struct url_data {
    size_t size;
    char* data;
};


size_t write_data(void *ptr, size_t size, size_t nmemb, struct url_data *data) {
    
    size_t index = data->size;
    size_t n = (size * nmemb);
    char* tmp;
    data->size += (size * nmemb);

#ifdef DEBUG

    fprintf(stderr, "data at %p size=%ld nmemb=%ld\n", ptr, size, nmemb);

#endif

    tmp = (char*)realloc(data->data, data->size + 1); /* +1 for '\0' */


    if(tmp) {
        data->data = tmp;
    } else {
        if(data->data) {
            free(data->data);
        }
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy((data->data + index), ptr, n);
    data->data[data->size] = '\0';

    return size * nmemb;
}

char *handle_url() {

    CURL *curl;

    struct url_data data;
    char temp[100] = "0\n1\n0\n1";
 
    data.size = 0;
    data.data = (char*)malloc(4096); /* reasonable size initial buffer */
    if(NULL == data.data) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return NULL;
    }

    data.data[0] = '\0';

    CURLcode res;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "192.168.50.4/index.html");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
		//fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
		if(  !strcmp(curl_easy_strerror(res), "Server returned nothing (no headers, no data)") ) {
			strcpy(data.data, temp);		 	
		//	printf("%s\n", data.data);
			 return data.data;
		}
        }
        curl_easy_cleanup(curl);
    }
    return data.data;
}

void image_hsv_mjpg(Mat input, int x, int y, int w, int h)
{
	time_t a_r, a_g, a_y;
	time_t b_r, b_g, b_y;
	struct tm a_r_tm, a_g_tm, a_y_tm;
	struct tm b_r_tm, b_g_tm, b_y_tm;


	// 관심영역 설정 (set ROI (X, Y, W, H)).
		Rect rect(x,y,w,h);

		// 관심영역 자르기 (Crop ROI).
		Mat img_input = input(rect);

		// show
		imshow("image", img_input);
		waitKey(30);
	//웹캠에서 캡처되는 속도 30 fps  

		Mat img_hsv_r, img_binary_r;
		Mat img_hsv_g, img_binary_g;
		Mat img_hsv_y, img_binary_y;

		


		//HSV로 변환
		cvtColor(img_input, img_hsv_r, COLOR_BGR2HSV);
		cvtColor(img_input, img_hsv_g, COLOR_BGR2HSV);
		cvtColor(img_input, img_hsv_y, COLOR_BGR2HSV);

		//지정한 HSV 범위를 이용하여 영상을 이진화
		inRange(img_hsv_r, Scalar(R_H_LOW, R_S_LOW, R_V_LOW), Scalar(R_H_HIGH, R_V_HIGH, R_S_HIGH), img_binary_r);

		inRange(img_hsv_g, Scalar(G_H_LOW, G_S_LOW, G_V_LOW), Scalar(G_H_HIGH, G_V_HIGH, G_S_HIGH), img_binary_g);

		inRange(img_hsv_y, Scalar(Y_H_LOW, Y_S_LOW, Y_V_LOW), Scalar(Y_H_HIGH, Y_V_HIGH, Y_S_HIGH), img_binary_y);

		
/*
		//원본이미지에 잡을들을 제거, 팽창을 통해 객체의 빈부분이 채짐워
		//morphological opening 작은 점들을 제거
		//침식 erode : 필터영역내 픽셀 중 최소 픽셀값을 현재 픽셀값에 대입
		erode(img_binary_r, img_binary_r, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		erode(img_binary_g, img_binary_g, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		erode(img_binary_y, img_binary_y, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//dilate : 영상팽창
		//팽창 : 필터 영역 내 픽셀 중 최대 픽셀값을 현재 픽셀값에 대입
		dilate(img_binary_r, img_binary_r, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(img_binary_g, img_binary_g, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(img_binary_y, img_binary_y, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));



		//morphological closing 영역의 구멍 메우기 
		dilate(img_binary_r, img_binary_r, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_binary_r, img_binary_r, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(img_binary_g, img_binary_g, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_binary_g, img_binary_g, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		dilate(img_binary_y, img_binary_y, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(img_binary_y, img_binary_y, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

*/
		//라벨링 : 이진화 한 이미지엣 형체를 알아보기 위해 같은 픽셀값들끼리
		//그룹화해 번호를 매긴 것을 라벨링이라고함
		Mat img_labels_r, stats_r, centroids_r;
		Mat img_labels_g, stats_g, centroids_g;
		Mat img_labels_y, stats_y, centroids_y;

		int numOfLables_r = connectedComponentsWithStats(img_binary_r, img_labels_r, stats_r, centroids_r, 8, CV_32S);

		int numOfLables_g = connectedComponentsWithStats(img_binary_g, img_labels_g, stats_g, centroids_g, 8, CV_32S);

		int numOfLables_y = connectedComponentsWithStats(img_binary_y, img_labels_y, stats_y, centroids_y, 8, CV_32S);


		//영역박스 그리기
		int max_r = -1, idx_r = 0;
		for (int j = 1; j < numOfLables_r; j++) {
			int area = stats_r.at<int>(j, CC_STAT_AREA);
			int left_r = stats_r.at<int>(j, CC_STAT_LEFT);
			int top_r = stats_r.at<int>(j, CC_STAT_TOP);
			int width_r = stats_r.at<int>(j, CC_STAT_WIDTH);
			int height_r = stats_r.at<int>(j, CC_STAT_HEIGHT);

			if (max_r < area)
			{
				max_r = area;
				idx_r = j;
			}


			//rectangle( img_input, Point(left_r,top_r), Point(left_r+width_r,top_r+height_r), Scalar(0,0,255),1 );  

			//putText(img_input, to_string(j), Point(left_r + 20, top_r + 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);

			// putText(img_input, to_string(width_r*height_r), Point(left_r + 20, top_r -30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);

					// printf("red area : %d\n",width_r*height_r);


		}


		//영역박스 그리기
		int max_g = -1, idx_g = 0;
		for (int j = 1; j < numOfLables_g; j++) {
			int area = stats_g.at<int>(j, CC_STAT_AREA);
			int left_g = stats_g.at<int>(j, CC_STAT_LEFT);
			int top_g = stats_g.at<int>(j, CC_STAT_TOP);
			int width_g = stats_g.at<int>(j, CC_STAT_WIDTH);
			int height_g = stats_g.at<int>(j, CC_STAT_HEIGHT);

			if (max_r < area)
			{
				max_g = area;
				idx_g = j;
			}

			//rectangle( img_input, Point(left_g,top_g), Point(left_g+width_g,top_g+height_g), Scalar(0,0,255),1 );  
			//putText(img_input, to_string(j*10), Point(left_g + 20, top_g + 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
			//printf("green  area : %d\n",width_g*height_g);
		}


		//영역박스 그리기
		int max_y = -1, idx_y = 0;
		for (int j = 1; j < numOfLables_y; j++) {
			int area = stats_y.at<int>(j, CC_STAT_AREA);
			int left_y = stats_y.at<int>(j, CC_STAT_LEFT);
			int top_y = stats_y.at<int>(j, CC_STAT_TOP);
			int width_y = stats_y.at<int>(j, CC_STAT_WIDTH);
			int height_y = stats_y.at<int>(j, CC_STAT_HEIGHT);

			if (max_y < area)
			{
				max_y = area;
				idx_y = j;
			}

			//rectangle( img_input, Point(left_y,top_y), Point(left_y+width_y,top_y+height_y), Scalar(0,0,255),1 );  
		//	putText(img_input, to_string(j*100), Point(left_y + 20, top_y + 20), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 0, 0), 2);
			//printf("yellow area : %d\n",width_y*height_y);
		}


		int left_r = stats_r.at<int>(idx_r, CC_STAT_LEFT);
		int top_r = stats_r.at<int>(idx_r, CC_STAT_TOP);
		int width_r = stats_r.at<int>(idx_r, CC_STAT_WIDTH);
		int height_r = stats_r.at<int>(idx_r, CC_STAT_HEIGHT);
		
		if( width_r*height_r < 70000 )	
		rectangle(img_input, Point(left_r, top_r), Point(left_r + width_r, top_r + height_r), Scalar(0, 0, 255), 1);

		int left_y = stats_y.at<int>(idx_y, CC_STAT_LEFT);
		int top_y = stats_y.at<int>(idx_y, CC_STAT_TOP);
		int width_y = stats_y.at<int>(idx_y, CC_STAT_WIDTH);
		int height_y = stats_y.at<int>(idx_y, CC_STAT_HEIGHT);

		if( width_y*height_y < 70000 )  
		rectangle(img_input, Point(left_y, top_y), Point(left_y + width_y, top_y + height_y), Scalar(0, 255, 0), 1);

		int left_g = stats_g.at<int>(idx_g, CC_STAT_LEFT);
		int top_g = stats_g.at<int>(idx_g, CC_STAT_TOP);
		int width_g = stats_g.at<int>(idx_g, CC_STAT_WIDTH);
		int height_g = stats_g.at<int>(idx_g, CC_STAT_HEIGHT);

		if( width_g*height_g < 70000 )  
		rectangle(img_input, Point(left_g, top_g), Point(left_g + width_g, top_g + height_g), Scalar(255, 0, 0), 1);



		imshow("binary_r", img_binary_r);
		imshow("binary_g", img_binary_g);
		imshow("binary_y", img_binary_y);
		imshow("origin", img_input);


		if (r_flag) {
			if (width_r*height_r > 4000 && width_r*height_r < 90000) {

				a_r = time(NULL);
				a_r_tm = *localtime(&a_r);

				//playSound("/home/jaeho/jaeho/red.wav");

				printf("Red Detected!!");
				printf("\n");
				r_flag = false; //빨강 사각형의 크기를 설정해주면 좋은점 신호등이 멀리있는데서 빨간색 인식하자마자 소리나오면 안좋음
						//예를 들어 전방 1m에 들어오면 소리 출력해준다고 할때 보이는 신호등의 사각형크기를 if에 넣어주면 좋을듯
				
				write_on_web(1,0); // 0717 추가  			
			}
		}
		if (!r_flag) {
			b_r = time(NULL);
			b_r_tm = *localtime(&b_r);

			int d_diff = difftime(b_r, a_r);
			int tm_day = d_diff / (60 * 60 * 24);
			d_diff = d_diff - (tm_day * 60 * 60 * 24);

			int tm_hour = d_diff / (60 * 60);
			d_diff = d_diff - (tm_hour * 60 * 60);

			int tm_min = d_diff / 60;
			d_diff = d_diff - (tm_min * 60);

			int tm_sec = d_diff;

			if (tm_sec > 20) {
				printf("Red 20초 경과\n");
				r_flag = true;
			}
		}

		if (g_flag) {
			if (width_g*height_g > 4000 && width_g*height_g < 90000) {
				a_g = time(NULL);
				a_g_tm = *localtime(&a_g);


				printf("Green Detected!!\n");
				//playSound("/home/jaeho/jaeho/green.wav");
				printf("\n");
				g_flag = false;

				write_on_web(0,1);
			}
		}

		if (!g_flag) {
			b_g = time(NULL);
			b_g_tm = *localtime(&b_g);

			int d_diff = difftime(b_g, a_g);
			int tm_day = d_diff / (60 * 60 * 24);
			d_diff = d_diff - (tm_day * 60 * 60 * 24);

			int tm_hour = d_diff / (60 * 60);
			d_diff = d_diff - (tm_hour * 60 * 60);

			int tm_min = d_diff / 60;
			d_diff = d_diff - (tm_min * 60);

			int tm_sec = d_diff;

			if (tm_sec > 20) {
				printf("Green : 20초 경과\n");
				g_flag = true;
			}
		}



		if (y_flag) {
			if (width_y*height_y > 4000 && width_y*height_y < 288000) {

				a_y = time(NULL);
				a_y_tm = *localtime(&a_y);

				printf("Yellow Detected!!\n");
				//playSound("/home/jaeho/jaeho/green.wav");
				printf("\n");
				y_flag = false;
			}
		}
		if (!y_flag) {
			b_y = time(NULL);
			b_y_tm = *localtime(&b_y);

			int d_diff = difftime(b_y, a_y);
			int tm_day = d_diff / (60 * 60 * 24);
			d_diff = d_diff - (tm_day * 60 * 60 * 24);

			int tm_hour = d_diff / (60 * 60);
			d_diff = d_diff - (tm_hour * 60 * 60);

			int tm_min = d_diff / 60;
			d_diff = d_diff - (tm_min * 60);

			int tm_sec = d_diff;

			if (tm_sec > 20) {
				printf("Yellow 20초 경과\n");
				y_flag = true;
			}
		}

}

void write_on_web(int red, int green)
{
    FILE* html_fd;
    char context[32];
    int bBox = 0;

    // 파일 내용 초기화  
    if (html_fd = fopen("yolo3/darknet/www/html/index2.html", "r"))
    {
	int index = 0;
	printf("file reset\n");
	while((context[index++] = fgetc(html_fd)) != EOF); // 파일 내용 읽어서 context에 저장
	
	fclose(html_fd);

	if (html_fd = fopen("yolo3/darknet/www/html/index2.html", "w")) // 쓰기 모드로 열기
	{
             fprintf(html_fd, "%d", 0);
             fclose(html_fd);
	}
        else
             printf("file open fail(W)\n");
	
	
    }else
	     printf("file open fail(R)\n"); // 파일 내용 초기화 끝.
 
   
    if( html_fd = fopen("yolo3/darknet/www/html/index2.html", "w"))
    {
	printf("file open success\n");
	// if 빨간불 일 때
	if( red == 1 ){
		fprintf(html_fd, "%d", 1);
		fclose(html_fd);
	}
    	// if 초록불 일 때
	if( green == 1 ){
		fprintf(html_fd, "%d", 2);
        	fclose(html_fd);
	}
    }
    else
	printf("file open fail(2)\n");



}

int main(){
	
	char* data;
	
	//printf("%s\n",data);
	// 문자열 배열에 저장.
	char* ptr;
	char* sArr[10] = { NULL, };
	int i = 0;


	int x, y, w, h;
	
	//VideoCapture capture("http:/172.19.87.109:8081/video?dummy=param.mjpg");
	VideoCapture capture("http:/192.168.50.15:8080./?action=stream");
	Mat frame; 

	while(1) {
        
		data = handle_url();
		
		ptr = strtok(data,"\n");

		while (ptr != NULL)
		{	
			sArr[i] = ptr;
			i++;
			ptr = strtok(NULL,"\n");
		}

		x = atoi( sArr[0] );
		y = atoi( sArr[2] );
		w = atoi( sArr[1] ) - atoi( sArr[0] );
		h = atoi( sArr[3] ) - atoi( sArr[2] );
		printf("%d %d %d %d \n", x, y, w, h);




		if (!capture.isOpened()) {
			 //Error 
			printf("Error0\n");
		}

	
		if (!capture.read(frame)) { 
			//Error
			printf("Error1\n");
 		} 
 		//imshow("image", frame);
		//waitKey(30);

		if ( !frame.data ) {
			return -1;
		}

		image_hsv_mjpg(frame, x, y, w, h);		
/*
		// 관심영역 설정 (set ROI (X, Y, W, H)).
		Rect rect(x,y,w,h);

		// 관심영역 자르기 (Crop ROI).
		Mat subImage = frame(rect);

		// show
		imshow("image", subImage);
		waitKey(30);
*/
		i=0;
		ptr = NULL;
		sArr[10] = { NULL, };
	}

	

}
