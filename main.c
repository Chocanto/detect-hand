#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>





/**********************
 *    helloworld.cpp  *
 **********************/


using namespace std;

int TAILLECROIX = 10;
int baryX = 0, baryY = 0;

IplImage *imgDef;
IplImage *hsv;
IplImage *masque;

CvScalar masqueScal, scal, couleur;

CvScalar blanc;

void dessinerCroix(IplImage* image, int x, int y, int taille, CvScalar 
couleurCroix)
{
    cvLine(image, cvPoint(x - taille / 2, y), cvPoint(x + taille / 2, y), couleurCroix);
    cvLine(image, cvPoint(x, y - taille / 2), cvPoint(x, y + taille / 2), couleurCroix);
}

void updateImg()
{
    blanc.val[0] = 0;
    blanc.val[1] = 0;
    blanc.val[2] = 0;

    IplImage *frame;

    frame = cvCloneImage(imgDef);

    dessinerCroix(frame, baryX, baryY, TAILLECROIX, blanc);

    cvShowImage("Détection main", frame);

    cvReleaseImage(&frame);

}


void baryCentre()
{
    int sommeX = 0, sommeY = 0, nbrPix = 0;
    int x = 0, y = 0;

    for (y = 0; y < masque->height; y++)
    {
        for (x = 0; x < masque->width; x++)
        {
            masqueScal = cvGet2D(masque, y, x);

            if (masqueScal.val[0] == 255)
            {
                sommeX += x;
                sommeY += y;
                nbrPix++;
            }
        }
    }

    if (nbrPix != 0)
    baryX = sommeX / nbrPix;
    else
    baryX = 0;

    if (nbrPix != 0)
    baryY = sommeY / nbrPix;
    else
    baryY = 0;

    cout << "sommeX = " << sommeX << endl << "sommeY = " << sommeY << endl;
    cout << "Barycentre en (" << baryX << ", " << baryY << ")." << endl;
    cout << nbrPix << " pixels trouvés." << endl;

}

void updateMasque(int seuil = -1)
{

    if (seuil == -1)
    {
        seuil = cvGetTrackbarPos("Seuil", "Détection main");
    }

    int x = 0, y = 0;

    for (y = 0; y < imgDef->height; y++)
    {
        for (x = 0; x < imgDef->width; x++)
        {
            scal = cvGet2D(hsv, y, x);

            if (scal.val[0] > (couleur.val[0] - seuil/2) && scal.val[0] < (couleur.val[0] + seuil/2)
                && scal.val[1] > (couleur.val[1] - seuil/2) && scal.val[1] < (couleur.val[1] + seuil/2))
            {
                masqueScal.val[0] = 255;
            }

            else
            {
                masqueScal.val[0] = 0;
            }

            cvSet2D(masque, y, x, masqueScal);
        }
    }

    baryCentre();
    cvShowImage("Masque", masque);
    updateImg();
}

void onClick(int event, int x, int y, int flags, void* param)
{
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        cout << x << ";" << y << endl;
        couleur = cvGet2D(hsv, x, y);
        cout << couleur.val[0] << "," << couleur.val[1] << "," << couleur.val[2] << endl;

        updateMasque();
    }
}

int main()
{
    int seuil = 50;

    couleur.val[0] = 255;
    couleur.val[1] = 100;
    couleur.val[2] = 100;


    imgDef=cvLoadImage("/home/agranger36/Programmation/Detect_hand/main.jpg");

    if (imgDef == NULL)
    {
        cout << "Impossible de charger l'image." << endl;
        exit(EXIT_FAILURE);
    }


    hsv = cvCloneImage(imgDef);

    masque = cvCreateImage(cvSize(imgDef->width, imgDef->height), IPL_DEPTH_8U, 1);

    cvCvtColor(imgDef, hsv, CV_BGR2HSV);


    cvNamedWindow("Détection main", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Masque", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("HSV", CV_WINDOW_AUTOSIZE);

    cvShowImage("HSV", hsv);

    //premier update pour initialiser :
    updateMasque(seuil);

    cvCreateTrackbar("Seuil", "Détection main", &seuil, 255, updateMasque);


    cvSetMouseCallback("Détection main", onClick);

    cvWaitKey(0);


    cvDestroyWindow("Détection main");


    cvReleaseImage(&imgDef);


    return 0;

}
