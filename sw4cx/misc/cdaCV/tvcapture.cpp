#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#define USE_MT_CXSCHEDULER 1
#if USE_MT_CXSCHEDULER
#include "cda_adapter.h"
//#include "mt_cxscheduler.h"
//#include "cda.h"
#endif

using namespace cv;
using namespace std;

int quadrant = 0;
const Rect mainRoi(60,30,580,450);
const Rect roi[] = {Rect(0,0,580,450),Rect(0,0,290,225),Rect(290,0,290,225),Rect(0,225,290,225),Rect(290,225,290,225)};

const int aftershots=3;

const char *channames[4] = {"cur", "evt", "nx1", "nx2"};
#if USE_MT_CXSCHEDULER
//cda_context_t cda_ctx;
//cda_dataref_t cda_refs[4];
static void CreatePipes(void)
{
#if 1
    cda_adapter_init("cxhw:26", 580*450, channames);
#else
    char channame[200];

    mt_sl_start();
    mt_sl_lock();
    cda_ctx = cda_new_context(0, NULL, "cxhw:26", 0, NULL, 0, NULL, NULL);
    for (int i = 0;  i < 4;  i++)
    {
        sprintf(channame, "%s.data", channames[i]);
        cda_refs[i] = cda_add_chan(cda_ctx, NULL,
                                   channame, CDA_DATAREF_OPT_NONE,
                                   CXDTYPE_UINT8, 580*450,
                                   0, NULL, NULL);
    }
    mt_sl_unlock();
#endif
}
#else
FILE *cda_pipes[4];
static void CreatePipes(void)
{
    char cmdline[200];

    fflush(stdout);
    for (int i = 0;  i < 4;  i++)
    {
        sprintf(cmdline, "pipe2cda -C @+b%d:cxhw:26.%s.data",
                580*450, channames[i]);
        errno = 0;
        cda_pipes[i] = popen(cmdline, "w");
        if (cda_pipes[i] == NULL)
            fprintf(stderr, "failed to popen(pipe2cda [%d]:%s): %s\n",
                    i, channames[i], strerror(errno));
        else
            /*setvbuf()? with which mode/size?*/;
    }
}
#endif
static void SendToPipe(IplImage *img, int n)
{
  unsigned char           buf[580*450];
  register unsigned char *src_p;
  register unsigned char *dst_p;
  int            yleft;
  register int   xleft;

    if (1/*cda_pipes[n] != NULL*/)
    {
        // ??? cvtColor(original, gray, CV_BGR2GRAY); -- https://answers.opencv.org/question/89916/how-to-capture-in-grayscale-directly-from-source/
        // or set CV_CAP_MODE_GRAY before capture?

#if 1 // R*0.3+G*0.6+B*0.1; Note: openCV color order is BGR
        for (yleft = 450, dst_p = buf, src_p = (unsigned char *)(img->imageData + 30*img->widthStep + 60*3);
             yleft > 0;
             yleft--,                  src_p += 3*(640-580))
            for (xleft = 580;  xleft > 0;  xleft--)
                *dst_p++ = ((*src_p++) + (*src_p++)*6 + (*src_p++)*3) / 10;
#elif 1 // Green
        for (yleft = 450, dst_p = buf, src_p = (unsigned char *)(img->imageData + 30*img->widthStep + 60*3 + 1);
             yleft > 0;
             yleft--,                  src_p += 3*(640-580))
            for (xleft = 580;  xleft > 0;  xleft--, src_p += 3)
                *dst_p++ = *src_p;
#else // Blue
        for (yleft = 450, dst_p = buf, src_p = (unsigned char *)(img->imageData + 30*img->widthStep + 60*3);
             yleft > 0;
             yleft--,                  src_p += 3*(640-580))
            for (xleft = 580;  xleft > 0;  xleft--, src_p += 3)
                *dst_p++ = *src_p;
#endif
#if USE_MT_CXSCHEDULER
          cda_adapter_send_one(n, 580*450, buf);
//        mt_sl_lock();
//        cda_snd_ref_data(cda_refs[n], CXDTYPE_UINT8, 580*450, buf);
//        mt_sl_unlock();
#else
        fwrite(buf, 1, 580*450,
               cda_pipes[n]);
        fflush(cda_pipes[n]);
#endif
    }
}

int main(int argc, char* argv[])
{

	if(argc>1) quadrant = atoi(argv[1]);

	CreatePipes();

	// получаем любую подключённую камеру
	CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY); //cvCaptureFromCAM( 0 );
	assert(capture);

	//      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 720);//1280); 
	//      cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 576);//960); 

	// узнаем ширину и высоту кадра
	double width = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH);
	double height = cvGetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT);
	printf("[i] %.0f x %.0f\n", width, height );

	IplImage* frame=0;

	cvNamedWindow("Live beam", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Last beam", CV_WINDOW_AUTOSIZE);
	

	for (int i=1; i<aftershots; i++)
	{
		char _name[50];
		sprintf(_name,"Last beam %i",i);
		cvNamedWindow(_name, CV_WINDOW_AUTOSIZE);
	}

	printf("[i] press Enter for capture image and Esc for quit!\n\n");

	
	double prev_avg;
	while(true)
	{
		// получаем кадр
		frame = cvQueryFrame(capture);
		Mat img = cvarrToMat(frame);
		Mat img_1 = img(mainRoi);
		Mat gs = img_1(roi[quadrant]);
		// показываем
		imshow("Live beam", img_1);
                {
                        static int was_printed = 0;
                        if (!was_printed)
                        {
                                fprintf(stderr, "w=%d h=%d d=%d nC=%d step=%d ord=%d\n",
                                        frame->width, frame->height,
                                        frame->depth, frame->nChannels,
                                        frame->widthStep, frame->dataOrder);
                                was_printed = 1;
                        }
                }
		SendToPipe(frame, 0);

		Scalar avg(mean(gs));
//		cout << gs.rows << "\t" << avg.val[0] << "\n";

		if(avg.val[0]/prev_avg>1.10)
		{
			printf("%f %f\n", prev_avg, (double)avg.val[0]);
			imshow("Last beam", gs);
			SendToPipe(frame, 1);
			
			//Цикл показа следующих кадров.	
			for (int i=1;i<aftershots;i++)
			{
				usleep(40000);
				// получаем кадр
				frame = cvQueryFrame(capture);
				Mat img = cvarrToMat(frame);
				Mat img_1 = img(mainRoi);
				Mat gs = img_1(roi[quadrant]);
				// показываем
				char _name[50];
				sprintf(_name,"Last beam %i",i);
				imshow(_name, gs);
				SendToPipe(frame, 1+i);
			}
		}
		prev_avg = avg.val[0];

		char c = cvWaitKey(35);
		if (c == 10) // нажата Enter
		{ 
	   		imshow("Last beam", gs);
			//cvShowImage("Last beam", gs);
		}
		else if (c == 27) // нажата ESC
		{ 
			break;
		}
	}
// освобождаем ресурсы
cvReleaseCapture( &capture );
cvDestroyWindow("Live beam");
cvDestroyWindow("Last beam");
return 0;
}
