
// Copyright 2010 Isis Innovation Limited
// This video source uses Point Grey's FlyCapture 2 API
// It is based on their example code
// Parameters are hard-coded here - adapt to your specific camera.

#pragma comment(lib,"FlyCapture2_v110.lib")

#include "VideoSource.h"
#include <gvars3/instances.h>
#include <cvd/utility.h>
#define WIN32_MEAN_AND_LEAN
#include <windows.h>

#include "FlyCapture2.h"

using namespace std;
using namespace CVD;
using namespace GVars3;
using namespace FlyCapture2;


int IMAGE_W = (2048);
int IMAGE_H = (2048);

static Camera cam;

static void PrintErrorAndExit(Error error)
{
    error.PrintErrorTrace();
    exit(0);
}

VideoSource::VideoSource(): m_buffer(NULL) 
{
    Error error;
    BusManager busMgr;

    
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK) PrintErrorAndExit(error);

    printf("Number of cameras detected: %u\n", numCameras);
    assert(numCameras > 0);
    PGRGuid guid;
    error = busMgr.GetCameraFromIndex(0, &guid);
    if (error != PGRERROR_OK) PrintErrorAndExit(error);

    error = cam.Connect(&guid);
    if (error != PGRERROR_OK) PrintErrorAndExit(error);

    CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK) PrintErrorAndExit(error);

    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        camInfo.serialNumber,
        camInfo.modelName,
        camInfo.vendorName,
        camInfo.sensorInfo,
        camInfo.sensorResolution,
        camInfo.firmwareVersion,
        camInfo.firmwareBuildTime);

    VideoMode vm;
    FrameRate fr;

    // Start capturing images at 640x480xY8
    // You probably need to change this stuff for your camera
    mirSize = ImageRef(IMAGE_W, IMAGE_H);
    
    bool bStatus;

    int i,x;

    for (i = VIDEOMODE_160x120YUV444; i < NUM_VIDEOMODES-1; i++) {
        for (x = FRAMERATE_1_875; x < NUM_FRAMERATES-1; x++) {
            error = cam.GetVideoModeAndFrameRateInfo((VideoMode)i, (FrameRate)x, &bStatus);
            if (error != PGRERROR_OK)
            {
                PrintErrorAndExit(error);

            }

            if (bStatus == TRUE)
            {
                printf("found at %d %d\n", i, x);

                break;

            }
        }
    }

#if 0 
    error = cam.SetVideoModeAndFrameRate(VIDEOMODE_640x480RGB, FRAMERATE_120);
    if (error != PGRERROR_OK)
    {
        // my PT camera doesnt support these mode changes.
       // PrintErrorAndExit(error);

    }
#endif
    cam.GetVideoModeAndFrameRate(&vm, &fr);

    error = cam.StartCapture();
    if (error != PGRERROR_OK) PrintErrorAndExit(error);
};

void VideoSource::GetAndFillFrameBWandRGB(CVD::Image<CVD::byte> &imBW, CVD::Image<CVD::Rgb<CVD::byte> > &imRGB)
{
    Error error;

    imRGB.resize(mirSize);
    imBW.resize(mirSize);
    static FlyCapture2::Image imCap;
    error = cam.RetrieveBuffer(&imCap);
    if (error != PGRERROR_OK)
    {
        PrintErrorAndExit(error);
  
    }

  
    unsigned char *pImage = imCap.GetData();

    for (int y = 0; y<mirSize.y; y++) {
        for (int x = 0; x<mirSize.x; x++) {
            imRGB[y][x].blue = *pImage;
            pImage++;

            imRGB[y][x].green =*pImage;
            imBW[y][x] = *pImage;
            pImage++;

            imRGB[y][x].red = *pImage;
            pImage++;
        }
    }

}

ImageRef VideoSource::Size()
{
    return mirSize;
}

VideoSource::~VideoSource()
{
    Error error;

    delete[] m_buffer;
    // Stop capturing images
    error = cam.StopCapture();
    if (error != PGRERROR_OK)
    {
        PrintErrorAndExit(error);
        return ;
    }

    // Disconnect the camera
    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintErrorAndExit(error);
        return;
    }

}
