#include "mainwindow.h"
#include <iostream>
#include "ui_mainwindow.h"
#include <QDebug>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(&gameTimer,  &QTimer::timeout, [&] {
        updateGame();
    });
    gameTimer.setInterval(10);

    connect(&attackTimer,  &QTimer::timeout, [&] {
        timerForAttack();
    });
    attackTimer.setInterval(1000);

    connect(&animationTimer_,  &QTimer::timeout, [&] {
        animateAttacks();
        animationState_ ++;
    });
    animationTimer_.setInterval(100);

    path_ = "C:/Users/Dorian/Desktop/Cours/Semestre 8/Couleur/Projet Couleur/Photos/";
    camera_ = 0;
    initGame();

    // Init Background
    updateBackground();
}

void MainWindow::updateGame(){
    camera_ >> currentPic_;
    flip(currentPic_,currentPic_,1);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(QImage(currentPic_.data, currentPic_.cols, currentPic_.rows,currentPic_.step, QImage::Format_RGB888)));

    if (players_[0].getPv() <= 0)
    {
        //Attendre un instant
        players_[0].setPv(0);
        ui->player1Lifebar->setValue(players_[0].getPv());
        attackTimer.stop();
        players_[1].setScore(players_[1].getScore() + 1);
        if (players_[1].getScore() >= 2)
        {
            players_[1].setScore(2);
            QString img2 = ":/img/GUI/P2 - " + QString::fromStdString(to_string(players_[1].getScore())) + "pt.png";
            ui->scorePlayer2Label->setPixmap(QPixmap::fromImage(QImage(img2)));

            // Arrêt du jeu
            ui->playPauseButton->setText("Replay ?");
            gameTimer.stop();

            // Affichage de victoire
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/youWin.png")));
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/youLose.png")));
        }
        else
        {
            restartGame();
        }
    }
    if (players_[1].getPv() <= 0)
    {
        // Attendre un instant
        players_[1].setPv(0);
        ui->player2Lifebar->setValue(players_[1].getPv());
        attackTimer.stop();
        players_[0].setScore(players_[0].getScore() + 1);
        if (players_[0].getScore() >= 2)
        {
            players_[0].setScore(2);
            QString img1 = ":/img/GUI/P1 - " + QString::fromStdString(to_string(players_[0].getScore())) + "pt.png";
            ui->scorePlayer1Label->setPixmap(QPixmap::fromImage(QImage(img1)));

            // Arrêt du jeu
            ui->playPauseButton->setText("Replay ?");
            gameTimer.stop();

            // Affichage de victoire
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/youWin.png")));
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/youLose.png")));
        }
        else
        {
            restartGame();
        }
    }
}

void MainWindow::switchTurn(){
    activePlayer_ = !activePlayer_;
    if (activePlayer_ == 1){
            //ui->activePlayer1->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/ActivePlayerFrame.png")));
            //ui->activePlayer2->setPixmap(QPixmap());
            ui->activePlayer1->setStyleSheet("background-color:rgba(116, 116, 116, 220)");
            ui->activePlayer2->setStyleSheet("");
    }else{
            //ui->activePlayer2->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/ActivePlayerFrame.png")));
            //ui->activePlayer1->setPixmap(QPixmap());
            ui->activePlayer2->setStyleSheet("background-color:rgba(116, 116, 116, 220)");
            ui->activePlayer1->setStyleSheet("");
    }
    time_ = -2;
}

void MainWindow::initGame() {
    // Init Players
    players_.push_back(Player("Player 1" , 100 , 0 , true, 2));
    players_.push_back(Player("Player 2" , 100 , 0 , true, 2));
    ui->player1Lifebar->setValue(players_[0].getPv());
    ui->player2Lifebar->setValue(players_[1].getPv());

    // Init Score
    QString img1 = ":/img/GUI/P1 - " + QString::fromStdString(to_string(players_[0].getScore())) + "pt.png";
    ui->scorePlayer1Label->setPixmap(QPixmap::fromImage(QImage(img1)));
    QString img2 = ":/img/GUI/P2 - " + QString::fromStdString(to_string(players_[1].getScore())) + "pt.png";
    ui->scorePlayer2Label->setPixmap(QPixmap::fromImage(QImage(img2)));


    // Ulti and display of players
    ultimateCharge_ = 0;
    ui->ultimateBarLabel->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Ultbar - " + QString::fromStdString(to_string(ultimateCharge_))+ ".png")));
    ui->activePlayer2_timer->setPixmap(QPixmap());
    ui->activePlayer1_timer->setPixmap(QPixmap());

    // Init Attacks
    QPixmap animationSpritesLaser;
    attacks_.push_back(Attack("Laser", 70, 1, animationSpritesLaser));
    QPixmap animationSpritesLightning;
    attacks_.push_back(Attack("Lightning", 80, 3, animationSpritesLightning));
    QPixmap animationSpritesRocket;
    attacks_.push_back(Attack("Rocket", 75, 2, animationSpritesRocket));

    // Select the first player and display it
    activePlayer_ = rand()%2;
    switchTurn();

    gameTimer.start();
    animationTimer_.start();

    // Deux tableaux différents seront considérés pour que la position puisse
    // être prise dans les deux configurations possibles
    vector<float> eclair{95.102965265535, 448.482922494281, 224.583064777916, 171.473394437738, 246.269944302787,448.482922494281, -3.91385117102071, -175.463520467813, -171.473394437738, -175.662644102559};
    vector<float> eclair_bis{218.849733728635, 450.482846340409, 2.852105258346991e+02, 3.93130424201746};
    vector<float> fusee{451, 179.958675811976, 276.741174271822, 0, -179.958675811976, 271.952794548342};
}

void MainWindow::restartGame(){
    players_[0].setPv(100);
    players_[1].setPv(100);
    ui->player1Lifebar->setValue(players_[0].getPv());
    ui->player2Lifebar->setValue(players_[1].getPv());
    ultimateCharge_ = 0;
    ui->ultimateBarLabel->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Ultbar - " + QString::fromStdString(to_string(ultimateCharge_))+ ".png")));
    ui->activePlayer2_timer->setPixmap(QPixmap());
    ui->activePlayer1_timer->setPixmap(QPixmap());
    QString img1 = ":/img/GUI/P1 - " + QString::fromStdString(to_string(players_[0].getScore())) + "pt.png";
    ui->scorePlayer1Label->setPixmap(QPixmap::fromImage(QImage(img1)));
    QString img2 = ":/img/GUI/P2 - " + QString::fromStdString(to_string(players_[1].getScore())) + "pt.png";
    ui->scorePlayer2Label->setPixmap(QPixmap::fromImage(QImage(img2)));
    attackTimer.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateBackground(){
    Mat mat1, mat2, mat3, mat4, mat5;
    camera_ >> mat1;
    camera_ >> mat2;
    camera_ >> mat3;
    camera_ >> mat4;
    camera_ >> mat5;
    background_ = (mat1/5+mat2/5+mat3/5+mat4/5+mat5/5) ;
    imwrite( path_ + "BG/background.jpg", background_);
    cvtColor(background_,background_,COLOR_BGR2GRAY);
}

void MainWindow::moinsFond(){
    Mat currentPic;
    flip(currentPic_,currentPic,1);
    Mat currentPlayer (currentPic, Rect(0 + 320*!activePlayer_, 0, 320, 480));
    Mat currentBG (background_, Rect(0 + 320*!activePlayer_, 0, 320, 480));
    //imwrite( path_ + "currentPic.jpg", currentPlayer );
    cvtColor(currentPlayer,currentPlayer,COLOR_BGR2GRAY);
    subtract(currentBG,currentPlayer,skel_);
    /*threshold(skel_, skel_, 15, 255,THRESH_BINARY );
    Mat element = getStructuringElement(2,Size(7,7));
    morphologyEx( skel_, skel_, 0, element );
    element = getStructuringElement(2,Size(15,15));
    morphologyEx( skel_, skel_, 1, element );
    remove_small_objects(skel_,10);
    //skel(skel_,skel_);
    //imwrite( path_ + "skel_.jpg", skel_ );*/


    threshold(skel_, skel_, 25, 255, cv::THRESH_BINARY);

    Mat element = getStructuringElement(2,Size(18,18));
    morphologyEx( skel_, skel_, 1, element );
    element = getStructuringElement(2,Size(19,19));
    morphologyEx( skel_, skel_, 0, element );

    thinning(skel_, skel_);

    skel_=Mat(skel_, Rect(5, 5, 310, 470));
    qDebug() << skel_.size().height;
    qDebug() << skel_.size().width;
    imshow("displayazea",skel_);
    Mat color_dst;
    cvtColor(skel_,color_dst,CV_GRAY2BGR);
    vector<Vec4i> lines;
    HoughLinesP(skel_,lines,0.5, 0.5*CV_PI/180,20, 30, 30);
    sort(lines.begin(),lines.end(),[&](Vec4i i,Vec4i j) { return sqrt(pow(i[0]-i[2],2)+pow(i[1]-i[3],2)) > sqrt(pow(j[0]-j[2],2)+pow(j[1]-j[3],2));});
    // [DEV] Affichage lines Hough
    for (size_t i = 0;i<lines.size(); i++){
        //qDebug() << sqrt(pow(lines[i][0]-lines[i][2],2)+pow(lines[i][1]-lines[i][3],2));
        line(color_dst,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(0,0,255),3,8);
    }
    /*
    vector<Vec4i> linesSelected;
    for (int i=0;i<4;i++){
        linesSelected[i]=lines[i];
        line(color_dst,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(255,0,0),3,8);
    }
    */
    imshow("display",color_dst);
    attack();
    switchTurn();
}


void MainWindow::attack(){
    // Calcul des dégats et charges
    attacks_[currentAttack_].damage(&players_[!activePlayer_],ultimateCharge_>=10);
    if (ultimateCharge_ >= 10) ultimateCharge_ = 0;
    ultimateCharge_ += attacks_[currentAttack_].getUltCharge();
    if (ultimateCharge_ >= 10) ultimateCharge_ = 10;

    // Animation
    animationState_ = 0;

    //Mise à jour des barres
    ui->ultimateBarLabel->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Ultbar - " + QString::fromStdString(to_string(ultimateCharge_))+ ".png")));
    ui->player1Lifebar->setValue(players_[0].getPv());
    ui->player2Lifebar->setValue(players_[1].getPv());

    // Pour des tests, a virer !
    currentAttack_ ++;
    if (currentAttack_ >=3) currentAttack_= 0;
}

void MainWindow::animateAttacks(){

    if (currentAttack_ == 0) // Meteore
    {
        if (animationState_ == 1){
            (activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu1"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu1")));
        }
        if (animationState_ == 2){
            (activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu2"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu2")));
        }
        if (animationState_ == 3){
            (activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu3"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu3")));
        }
        if (animationState_ == 4){
            (activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu4"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu4")));
        }
        if (animationState_ == 5){
            (activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu5"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/feu5")));
        }
        if (animationState_ == 6){
            (activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap()) : ui->activePlayer2_timer->setPixmap(QPixmap());
        }
        if (animationState_ == 8){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule1"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule1")));
        }
        if (animationState_ == 9){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule2"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule2")));
        }
        if (animationState_ == 10){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule3"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule3")));
        }
        if (animationState_ == 11){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule4"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule4")));
        }
        if (animationState_ == 12){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule5"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Meteore/boule5")));
        }
        if (animationState_ == 13){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap()) : ui->activePlayer1_timer->setPixmap(QPixmap());
        }
    }

    if (currentAttack_ == 1) // Lightning
    {
        if (animationState_ == 1){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair4"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair4")));
        }
        if (animationState_ == 2){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair5"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair5")));
        }
        if (animationState_ == 3){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair6"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair6")));
        }
        if (animationState_ == 4){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair4"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair4")));
        }
        if (animationState_ == 5){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair5"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair5")));
        }
        if (animationState_ == 6){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair6"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair6")));
        }
        if (animationState_ == 7){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair4"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair4")));
        }
        if (animationState_ == 8){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair5"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair5")));
        }
        if (animationState_ == 9){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair6"))) : ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Lightning/eclair6")));
        }
        if (animationState_ == 10){
            (activePlayer_) ? ui->activePlayer2_timer->setPixmap(QPixmap()) : ui->activePlayer1_timer->setPixmap(QPixmap());
        }
    }

}

void MainWindow::timerForAttack(){
    if (!activePlayer_ && time_ >= 1){
        if (time_ == 1) {
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/trois.png")));
        }
        else if (time_ == 2) {
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/deux.png")));
        }
        else if (time_ == 3) {
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/un.png")));
        }
        else {
            ui->activePlayer1_timer->setPixmap(QPixmap());
            moinsFond();
        }
    }
    else if (time_ >= 1){
        if (time_ == 1) {
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/trois.png")));
        }
        else if (time_ == 2) {
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/deux.png")));
        }
        else if (time_ == 3) {
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/un.png")));
        }
        else {
            ui->activePlayer2_timer->setPixmap(QPixmap());
            moinsFond();
        }
    }
    time_++;
}

// Function to remove small blobs from the binary image
void MainWindow::remove_small_objects( cv::Mat& im, double size )
{
    // Only accept CV_8UC1
        if (im.channels() != 1 || im.type() != CV_8U)
            return;

        // Find all contours
        std::vector<std::vector<cv::Point> > contours;
        cv::findContours(im.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

        for (int i = 0; i < contours.size(); i++)
        {
            // Calculate contour area
            double area = cv::contourArea(contours[i]);

            // Remove small objects by drawing the contour with black color
            if (area > 0 && area <= size)
                cv::drawContours(im, contours, i, CV_RGB(0, 0, 0), -1);
        }
}

void MainWindow::thinningIteration(cv::Mat& img, int iter)
{
    CV_Assert(img.channels() == 1);
    CV_Assert(img.depth() != sizeof(uchar));
    CV_Assert(img.rows > 3 && img.cols > 3);

    cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1);

    int nRows = img.rows;
    int nCols = img.cols;

    if (img.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    int x, y;
    uchar *pAbove;
    uchar *pCurr;
    uchar *pBelow;
    uchar *nw, *no, *ne;    // north (pAbove)
    uchar *we, *me, *ea;
    uchar *sw, *so, *se;    // south (pBelow)

    uchar *pDst;

    // initialize row pointers
    pAbove = NULL;
    pCurr  = img.ptr<uchar>(0);
    pBelow = img.ptr<uchar>(1);

    for (y = 1; y < img.rows-1; ++y) {
        // shift the rows up by one
        pAbove = pCurr;
        pCurr  = pBelow;
        pBelow = img.ptr<uchar>(y+1);

        pDst = marker.ptr<uchar>(y);

        // initialize col pointers
        no = &(pAbove[0]);
        ne = &(pAbove[1]);
        me = &(pCurr[0]);
        ea = &(pCurr[1]);
        so = &(pBelow[0]);
        se = &(pBelow[1]);

        for (x = 1; x < img.cols-1; ++x) {
            // shift col pointers left by one (scan left to right)
            nw = no;
            no = ne;
            ne = &(pAbove[x+1]);
            we = me;
            me = ea;
            ea = &(pCurr[x+1]);
            sw = so;
            so = se;
            se = &(pBelow[x+1]);

            int A  = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
                     (*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
                     (*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
                     (*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
            int B  = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
            int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
            int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                pDst[x] = 1;
        }
    }

    img &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * Parameters:
 * 		src  The source image, binary with range = [0,255]
 * 		dst  The destination image
 */
void MainWindow::thinning(const cv::Mat& src, cv::Mat& dst)
{
    dst = src.clone();
    dst /= 255;         // convert to binary image

    cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
    cv::Mat diff;

    do {
        thinningIteration(dst, 0);
        thinningIteration(dst, 1);
        cv::absdiff(dst, prev, diff);
        dst.copyTo(prev);
    }
    while (cv::countNonZero(diff) > 0);

    dst *= 255;
}


void MainWindow::on_updateBGButton_clicked()
{
    updateBackground();
}

void MainWindow::on_updatePlayer1PicButton_clicked()
{
    Mat tmp;
    camera_ >> tmp;
    tmp = Mat(tmp,Rect(320,0,320,480));
    cv::flip(tmp,tmp,1);
    ui->player1Label->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows,tmp.step, QImage::Format_RGB888)));
}

void MainWindow::on_updatePlayer2PicButton_clicked()
{
    Mat tmp;
    camera_ >> tmp;
    tmp = Mat(tmp,Rect(0,0,320,480));
    cv::flip(tmp,tmp,1);
    ui->player2Label->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows,tmp.step, QImage::Format_RGB888)));
}

void MainWindow::on_playPauseButton_clicked()
{
    if (ui->playPauseButton->text() == "Replay ?"){
        players_.clear();
        attacks_.clear();
        initGame();
        attackTimer.start();
        ui->playPauseButton->setText("Press to pause !");
    }
    else if (attackTimer.isActive()) {
        attackTimer.stop();
        ui->playPauseButton->setText("Press to play !");
    }
    else {
        attackTimer.start();
        ui->playPauseButton->setText("Press to pause !");
    }
}
