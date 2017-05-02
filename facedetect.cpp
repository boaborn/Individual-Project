#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
/// Mat Colour
#define MpixelB(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) ) [(x)*((image).channels())]
#define MpixelG(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) ) [(x)*((image).channels())+1]
#define MpixelR(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) ) [(x)*((image).channels())+2]
/// Mat greyscale
#define Mpixel(image,x,y) ( (uchar *) ( ((image).data) + (y)*((image).step) ) ) [(x)]

using namespace std;
using namespace cv;

static void help()
{
    cout << "\nThis program demonstrates the cascade recognizer. Now you can use Haar or LBP features.\n"
            "This classifier can recognize many kinds of rigid objects, once the appropriate classifier is trained.\n"
            "It's most known use is for faces.\n"
            "Usage:\n"
            "./facedetect [--cascade=<cascade_path> this is the primary trained classifier such as frontal face]\n"
               "   [--nested-cascade[=nested_cascade_path this an optional secondary classifier such as eyes]]\n"
               "   [--scale=<image scale greater or equal to 1, try 1.3 for example>]\n"
               "   [--try-flip]\n"
               "   [filename|camera_index]\n\n"
            "see facedetect.cmd for one call:\n"
            "./facedetect --cascade=\"../../data/haarcascades/haarcascade_frontalface_alt.xml\" --nested-cascade=\"../../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml\" --scale=1.3\n\n"
            "During execution:\n\tHit any key to quit.\n"
            "\tUsing OpenCV version " << CV_VERSION << "\n" << endl;
}
void Tracking();

void detectAndDraw( Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip );

string cascadeName;
string nestedCascadeName;

string imageName("newB.png");
Mat bg;     //make a container for background image
Point faceLocation[2];
Point newfaceLocation[2];
Point initface[2];
Point CurrentR;
Point Cen;
Point eyesLocation[2];
int R;
bool init=false;
bool left=false;
bool right=false;
int main( int argc, const char** argv ){
    bg = imread(imageName.c_str(), IMREAD_COLOR);
    namedWindow( "Tracking window", WINDOW_AUTOSIZE );
    VideoCapture capture;
    Mat frame, image;
    string inputName;
    bool tryflip;
    CascadeClassifier cascade, nestedCascade;
    double scale;

    cv::CommandLineParser parser(argc, argv,
        "{help h||}"
        "{cascade|../../data/haarcascades/haarcascade_frontalface_alt.xml|}"
        "{nested-cascade|../../data/haarcascades/haarcascade_eye_tree_eyeglasses.xml|}"
        "{scale|1|}{try-flip||}{@filename||}"
    );
    if (parser.has("help"))
    {
        help();
        return 0;
    }
    cascadeName = parser.get<string>("cascade");
    nestedCascadeName = parser.get<string>("nested-cascade");
    scale = parser.get<double>("scale");
    if (scale < 1)
        scale = 1;
    tryflip = parser.has("try-flip");
    inputName = parser.get<string>("@filename");
    if (!parser.check())
    {
        parser.printErrors();
        return 0;
    }
    if ( !nestedCascade.load( nestedCascadeName ) )
        cerr << "WARNING: Could not load classifier cascade for nested objects" << endl;
    if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        help();
        return -1;
    }
    if( inputName.empty() || (isdigit(inputName[0]) && inputName.size() == 1) )
    {
        int c = inputName.empty() ? 0 : inputName[0] - '0';
        if(!capture.open(c))
            cout << "Capture from camera #" <<  c << " didn't work" << endl;
    }
    else if( inputName.size() )
    {
        image = imread( inputName, 1 );
        if( image.empty() )
        {
            if(!capture.open( inputName ))
                cout << "Could not read " << inputName << endl;
        }
    }
    else
    {
        image = imread( "../data/lena.jpg", 1 );
        if(image.empty()) cout << "Couldn't read ../data/lena.jpg" << endl;
    }

    if( capture.isOpened() )
    {
        cout << "Video capturing has been started ..." << endl;
        for(;;)
        {
            capture >> frame;
            if( frame.empty() )
                break;

            Mat frame1 = frame.clone();
            Mat bg2=bg.clone();
            detectAndDraw( frame1, cascade, nestedCascade, scale, tryflip);


            int c = waitKey(10);
            if( c == 27 || c == 'q' || c == 'Q' ){
                break;
            }
        }
    }
    else
    {
        cout << "Detecting face(s) in " << inputName << endl;
        if( !image.empty() )
        {
            detectAndDraw( image, cascade, nestedCascade, scale, tryflip);
            waitKey(0);
        }
        else if( !inputName.empty() )
        {
            /* assume it is a text file containing the
            list of the image filenames to be processed - one per line */
            FILE* f = fopen( inputName.c_str(), "rt" );
            if( f )
            {
                char buf[1000+1];
                while( fgets( buf, 1000, f ) )
                {
                    int len = (int)strlen(buf), c;
                    while( len > 0 && isspace(buf[len-1]) )
                        len--;
                    buf[len] = '\0';
                    cout << "file " << buf << endl;
                    image = imread( buf, 1 );
                    if( !image.empty() )
                    {
                        detectAndDraw( image, cascade, nestedCascade, scale, tryflip );
                        c = waitKey(0);
                        if( c == 27 || c == 'q' || c == 'Q' )
                            break;
                    }
                    else
                    {
                        cerr << "Aw snap, couldn't read image " << buf << endl;
                    }
                }
                fclose(f);
            }
        }
    }

    return 0;
}

void detectAndDraw( Mat& img, CascadeClassifier& cascade,CascadeClassifier& nestedCascade,double scale, bool tryflip){
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[]={
        Scalar(255,0,0),
        Scalar(255,128,0),
        Scalar(255,255,0),
        Scalar(0,255,0),
        Scalar(0,128,255),
        Scalar(0,255,255),
        Scalar(0,0,255),
        Scalar(255,0,255)
    };

    Mat gray, smallImg;

    cvtColor( img, gray, COLOR_BGR2GRAY );
    double fx = 1 / scale;
    resize( gray, smallImg, Size(), fx, fx, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    t = (double)cvGetTickCount();
    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CASCADE_FIND_BIGGEST_OBJECT
                                 //|CASCADE_DO_ROUGH_SEARCH
                                 |CASCADE_SCALE_IMAGE,
                                 Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    t = (double)cvGetTickCount() - t;
    //printf( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
    for ( size_t i = 0; i < faces.size(); i++ )
    {

        Rect r = faces[i];
        Mat smallImgROI;
        vector<Rect> eyes;//vetor for eyes
        Point center;
        Scalar color = colors[i%8];
        int radius;

        double aspect_ratio = (double)r.width/r.height;
        //draw rect for face
        if( 0.75 < aspect_ratio && aspect_ratio < 1.3 )
        {

            center.x = cvRound((r.x + r.width*0.5)*scale);
            center.y = cvRound((r.y + r.height*0.5)*scale);
            radius = cvRound((r.width + r.height)*0.25*scale);
            circle( img, center, radius, color, 1, 8, 0 );
            circle( img, center, 5, Scalar(0,255,255), -1, 8, 0 );
            //                             top  left x            top   left y               button     left x                         button    right y
           rectangle( img, cvPoint(cvRound((r.x+10)*scale), cvRound(r.y*scale)),cvPoint(cvRound((r.x + r.width-10)*scale), cvRound((r.y + r.height-1)*scale)),color, 1, 8, 0);  //thickness = 1, int lineType = LINE_8, int shift = 0
           faceLocation[0]=cvPoint(cvRound((r.x+10)*scale), cvRound(r.y*scale));
           faceLocation[1]=cvPoint(cvRound((r.x + r.width-10)*scale), cvRound((r.y + r.height-1)*scale));
           CurrentR=center;

           R=radius;
           int k;
           k = waitKey(10);
           if(k==' '){
                initface[0]=faceLocation[0];
                initface[1]=faceLocation[1];
                Cen=center;
                init=true;

           }
        }

        if(nestedCascade.empty()) continue;
        //Change ROI to only a small area on top of the face detection
        // r is the rectangle for the face, ROI is based on the rectangle from the smallImg
        // r.x, r,y, r.width, r,height
        // take 30% off the top, 40% off the bottom
        Rect eyeROI;
        eyeROI.y=r.y+0.2*r.height;
        eyeROI.x = r.x;
        eyeROI.height=0.35*r.height;
        eyeROI.width=r.width;
        //now make smallImgROI the size of the rectangle
        smallImgROI = smallImg(eyeROI);//( r );
        nestedCascade.detectMultiScale( smallImgROI, eyes,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE,
            Size(30, 30) );
            //draw rect for eyes
        Point minAveragePoint;
        for ( size_t j = 0; j < eyes.size(); j++ ){
            //cout<<"eyes size is: "<<eyes.size();
            //if(eyes.size()!=2){continue;}
            Rect nr = eyes[j];
            Point a,b;
            a.x=(r.x+nr.width);
            a.y=(r.y+nr.height);
            b.x=r.x-10;
            b.y=r.y;
            nr.x=(eyeROI.x + nr.x)*scale;
            nr.y=(eyeROI.y + nr.y)*scale;
            nr.width*=scale;
            nr.height*=scale;
            rectangle(img,nr,color,1,8,0);
            Mat image_1=Mat(img,nr);
            cvtColor(image_1,image_1,COLOR_BGR2GRAY);
            int minAverage = 1000000;
            int newAverage;
            int start=2;
            for(int x=start;x+start<=image_1.cols-1;++x){
                for(int y=start;y+start<=image_1.rows-1;++y){
                    newAverage=(
                                Mpixel(image_1,(x-2),(y-2)) + Mpixel(image_1,(x-2),(y-1)) + Mpixel(image_1,(x-2),(y)) + Mpixel(image_1,(x-2),(y+1)) + Mpixel(image_1,(x-2),(y+2)) +
                                Mpixel(image_1,(x-1),(y-2)) + Mpixel(image_1,(x-1),(y-1)) + Mpixel(image_1,(x-1),(y)) + Mpixel(image_1,(x-1),(y+1)) + Mpixel(image_1,(x-1),(y+2)) +
                                Mpixel(image_1,(x),(y-2))   + Mpixel(image_1,(x),(y-1))   + Mpixel(image_1,(x),(y))   + Mpixel(image_1,(x),(y+1))   + Mpixel(image_1,(x),(y+2))   +
                                Mpixel(image_1,(x+1),(y-2)) + Mpixel(image_1,(x+1),(y-1)) + Mpixel(image_1,(x+1),(y)) + Mpixel(image_1,(x+1),(y+1)) + Mpixel(image_1,(x+1),(y+2)) +
                                Mpixel(image_1,(x+2),(y-2)) + Mpixel(image_1,(x+2),(y-1)) + Mpixel(image_1,(x+2),(y)) + Mpixel(image_1,(x+2),(y+1)) + Mpixel(image_1,(x+2),(y+2))
                                )/25;
                    if (newAverage <= minAverage) {// if the new average is lower than any previous average
                        minAverage = newAverage;//it becomes the new minimum average
                        minAveragePoint.x = x;//remember which point gave that average
                        minAveragePoint.y = y;//remember which point gave that average
                    }
                }
            }

            /*3x3 not working very well
            for(int x=1;x+1<=image_1.cols-1;++x){
                for(int y=1;y+1<=image_1.rows-1;++y){
                    newAverage=(Mpixel(image_1,(x-1),(y-1)) + Mpixel(image_1,(x-1),(y))  + Mpixel(image_1,(x-2),(y+1))+
                                Mpixel(image_1,(x),(y-1))   + Mpixel(image_1,(x-1),(y))  + Mpixel(image_1,(x-1),(y+1))+
                                Mpixel(image_1,(x+1),(y-1)) + Mpixel(image_1,(x+1),(y))  + Mpixel(image_1,(x+1),(y+1))
                                )/9;
                    if (newAverage <= minAverage) {// if the new average is lower than any previous average
                            minAverage = newAverage;//it becomes the new minimum average
                            minAveragePoint.x = x;//remember which point gave that average
                            minAveragePoint.y = y;//remember which point gave that average
                    }

                }
            }
            */
            minAveragePoint.x = minAveragePoint.x + nr.x;
            minAveragePoint.y = minAveragePoint.y + nr.y;
            circle( img, minAveragePoint, 2, Scalar(255,255,0), -1, 8, 0 );
            if(j<2){
                eyesLocation[j]=minAveragePoint;
            }
        }
        Tracking();
        imshow( "result", img );
    }

}

void Tracking(){
    Mat bg2=bg.clone();
    float width=1280;
    float height=960;
    //current is 2*2, you can change here to get 3*3 etc
    int horizontal_line=1;
    int vertical_line=1;
    float x,y;
    x=width/(horizontal_line+1);
    y=height/(vertical_line+1);
    Point startPoint,endPoint;
    startPoint.x=x;
    startPoint.y=0.0;
    endPoint.x=startPoint.x;
    endPoint.y=height;
    for(int i=0;i<horizontal_line;i++){
        line(bg2,startPoint,endPoint,(225,225,225),1,8,0);
        startPoint.x+=x;
        endPoint.x=startPoint.x;
    }

    startPoint.x=0.0;
    startPoint.y=y;
    endPoint.x=width;
    endPoint.y=startPoint.y;
    for(int i=0;i<vertical_line;i++){
        line(bg2,startPoint,endPoint,(225,225,225),1,8,0);
        startPoint.y+=y;
        endPoint.y=startPoint.y;
    }
    int H;
    int W;
    W=faceLocation[1].x-faceLocation[0].x;
    H=faceLocation[1].y-faceLocation[0].y;
    int orX=640;
    int orY=480;
    int scaleX=width/orX;
    int scaleY=height/orY;
    Point center;
    center.x=640;
    center.y=480;
    Point A=center;
    Point L,R;

    if(init==true){
        circle(bg2,center,30,(100,100,100),-1,8,0);
        int diffX=CurrentR.x-Cen.x;
        int diffY=CurrentR.y-Cen.y;
        A.x=A.x-diffX*25;
        A.y=A.y+diffY*18;
        diffX=CurrentR.x-eyesLocation[0].x;
        //Left eyes
        L.x=A.x-diffX;
        diffY=CurrentR.y-eyesLocation[0].y;
        L.y=A.y-diffY;
        //Right eyes
        diffX=CurrentR.x-eyesLocation[1].x;
        R.x=A.x-diffX;
        diffY=CurrentR.y-eyesLocation[1].y;
        R.y=A.y-diffY;

    }
    circle(bg2, A,5,(0,255,255), -1, 8, 0 );
    circle(bg2, L, 5,Scalar(0,255,0), -1, 8, 0 );
    circle(bg2, R, 5,Scalar(0,255,0), -1, 8, 0 );
    eyesLocation[0].x=640;
    eyesLocation[0].y=480;
    eyesLocation[1].x=640;
    eyesLocation[1].y=480;
    imshow( "Tracking window", bg2 );

}

