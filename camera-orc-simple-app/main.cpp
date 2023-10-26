// cconcsole-tess-opencv.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <memory>
#include <stdexcept>
#include <allheaders.h> // leptonica main header for image io
#include <baseapi.h> // tesseract main header
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core/utils/logger.hpp>


using namespace cv;
using namespace std;
using namespace tesseract;


void init_proc() {
    // Initialize the process
    // --- SET OPENCV LOGGING LEVEL TO SILENT
    utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_SILENT);
}

void init_tesseract(TessBaseAPI* tess) {
    // Initialize tesseract API
    cout << "Initialize tesseract API" << endl;
    if (tess->Init("./", "eng"))
    {
        cout << "OCRTesseract: Could not initialize tesseract." << std::endl;
        tess->Clear();
        throw runtime_error("OCRTesseract: Could not initialize tesseract." );
    }
    tess->SetPageSegMode(PageSegMode::PSM_AUTO);
    tess->SetVariable("save_best_choices", "T");
}

void init_vcap(TessBaseAPI* tess, Mat* frame, VideoCapture* cap) {
    // --- INITIALIZE VIDEO CAPTURE
    cout << "Initialize video capture" << endl;
    int deviceID = 0; // default camera
    int apiID = CAP_ANY; // auto detect default API

    // --- OPEN VIDEO
    cout << "Open video" << endl;
    cap->open(deviceID, apiID);
    if (!cap->isOpened()) {
        cerr << "ERROR: Unable to open camera" << endl;
        throw runtime_error("ERROR: Unable to open camera");
    }
}


int main(int argc, char* argv[])
{
    TessBaseAPI tess;
    VideoCapture cap;
    Mat frame;
    int iKey;
    char* pText = NULL;
    int iExitCode = 0;

    try {
        init_proc();
        init_tesseract(&tess);
        init_vcap(&tess, &frame, &cap);

        // --- GRAB FRAMES
        cout << "Start grabbing" << endl
            << "Press 'q' to terminate" << endl
            << "Press spacebar to capture" << endl;
        while (true) {
            // read frame
            cap.read(frame);
            if (frame.empty()) {
                cerr << "ERROR: get blank frame" << endl;
                break;
            }

            // actions
            iKey = waitKey(5);
            if (iKey == 'q') {
                // exit
                break;
            }
            else if (iKey == ' ') {
                // capture
                tess.Clear();
                tess.SetImage((uchar*)frame.data, frame.size().width, frame.size().height, frame.channels(), frame.step1());
                tess.Recognize(0);
                pText = tess.GetUTF8Text();
                tess.Clear();
            }

            // display
            flip(frame, frame, 1);
            //copyMakeBorder(frame, frame, 5, 5, 5, 5, BORDER_CONSTANT);
            putText(frame, "Spacebar to capture", Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 2, false);
            putText(frame, "Q to Exit", Point(10, 60), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 0), 2, false);
            if (pText) {
                putText(frame, pText, Point(10, 80), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1, false);
            }
            imshow("Live", frame);
        }
    }
    catch (...) {
        iExitCode = 1;
    }

    tess.Clear();
    cap.release();
    printf("Exit with %d", iExitCode);
    return iExitCode;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
