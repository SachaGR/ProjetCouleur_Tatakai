#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    VideoCapture cam;
    QTimer camTimer;
    Ui::MainWindow *ui;
    int Edges();
    int detectPeople();
    int pythonLike();
    void moinsFond();
    Mat remove_small_objects( Mat img_in, int size );
    void skel(Mat &src,Mat &dst);
private slots:
    void getCamPic();
};

#endif // MAINWINDOW_H
