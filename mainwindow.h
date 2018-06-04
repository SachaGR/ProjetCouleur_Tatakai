#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "attack.h"
#include "player.h"

#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
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

    void initGame();
    Mat findSkel(Mat frame);
    void updateBG();
    void updatePP(Player player);
    Attack compareSkel(Mat Skel);
    void updateBackground();
    void remove_small_objects( cv::Mat& im, double size);
    void skel(Mat &src,Mat &dst);
    void switchTurn();
    void timerForAttack();
    void attack();
    void animateAttacks();
    void restartGame();
    void thinningIteration(cv::Mat& img, int iter);
    void thinning(const cv::Mat& src, cv::Mat& dst);
    vector<float> getDatas(int x1,int y1,int x2,int y2);
    int verdict();

    //Getters & Setters
    void setUltimateCharge(int ultimateCharge) {ultimateCharge_ = ultimateCharge;}
    int getUltimateCharge() {return ultimateCharge_;}
    void setActivePlayer(int activePlayer) {activePlayer_ = activePlayer;}
    int getActivePlayer() {return activePlayer_;}
    void setBackground(Mat background) {background_ = background;}
    Mat getBackground() {return background_;}


private:
    int ultimateCharge_ = 0;
    vector<Player> players_;
    vector<Attack> attacks_;
    vector<vector<float>> dataSelected_;
    int activePlayer_;
    int currentAttack_ = 6;
    int animationState_ = 0;
    VideoCapture camera_;
    QTimer gameTimer;
    QTimer skelTimer;
    QTimer attackTimer;
    QTimer animationTimer_;
    Ui::MainWindow *ui;
    String path_;
    int time_ = -2;
    Mat background_, currentPic_, skel_;

private slots:
    void updateGame();
    void moinsFond();
    void on_updateBGButton_clicked();
    void on_updatePlayer1PicButton_clicked();
    void on_updatePlayer2PicButton_clicked();
    void on_playPauseButton_clicked();
};

#endif // MAINWINDOW_H
