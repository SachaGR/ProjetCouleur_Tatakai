#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSound>
#include <QSoundEffect>
#include <windows.h>

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
    void updateBG();
    void updatePP(Player player);
    void updateBackground();
    void switchTurn();
    void timerForAttack();
    void attack();
    void animateAttacks();
    void restartGame();
    void thinningIteration(cv::Mat& img, int iter);
    void thinning(const cv::Mat& src, cv::Mat& dst);
    vector<float> getDatas(int x1,int y1,int x2,int y2);
    int verdict();

private:
    vector<Player> players_;
    vector<Attack> attacks_;
    vector<vector<float>> dataSelected_;
    int ultimateCharge_ = 0;
    int activePlayer_;
    int currentAttack_ = 6;
    int animationState_ = 0;
    int restartWait_ = 0;
    int time_ = -2;
    VideoCapture camera_;
    QTimer gameTimer,skelTimer,attackTimer,animationTimer_,restartTimer_;
    Ui::MainWindow *ui;
    Mat background_, currentPic_, skel_;
    QSound *lightningSound_ = new QSound(":/img/GUI/Sons/éclair.wav");
    QSound *bombSound_ = new QSound(":/img/GUI/Sons/Bomb2.wav");
    QSound *pingouinSound_ = new QSound(":/img/GUI/Sons/Pingu.wav");
    QSound *tatakaiSound_ = new QSound(":/img/GUI/Sons/TATAKAI.wav");
    // Path is not in ressource file for memory purposes
    QSound *loopSong_ = new QSound("C:/Users/Dorian/Desktop/Cours/Semestre 8/Couleur/Projet Couleur/GUI/Sons/loopSong2.wav");

private slots:
    void updateGame();
    void imageProcessing();
    void on_updateBGButton_clicked();
    void on_updatePlayer1PicButton_clicked();
    void on_updatePlayer2PicButton_clicked();
    void on_playPauseButton_clicked();
    void on_startingButton_clicked();
};

#endif // MAINWINDOW_H
