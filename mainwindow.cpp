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

    // Setup the timers
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

    connect(&restartTimer_,  &QTimer::timeout, [&] {
        restartGame();
        restartWait_ ++;
    });
    restartTimer_.setInterval(3000);

    // Init the camera
    camera_ = 0;

    // Init the game
    initGame();

    // Init the background matrix
    updateBackground();

    // Set the black background for starting screen
    QPixmap blackScreen(":/img/GUI/blackScreen.PNG");
    blackScreen = blackScreen.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, blackScreen);
    this->setPalette(palette);

    // Start and loop the music
    loopSong_->setLoops(QSound::Infinite);
    loopSong_->play();

    setFixedSize(1280, 720);
}

/** Rhythm of the game
 * @brief MainWindow::updateGame
 */
void MainWindow::updateGame(){
    // Take a picture and update the display
    camera_ >> currentPic_;
    flip(currentPic_,currentPic_,1);
    cvtColor(currentPic_, currentPic_, CV_BGR2RGB);
    ui->cameraLabel->setPixmap(QPixmap::fromImage(QImage(currentPic_.data, currentPic_.cols, currentPic_.rows,currentPic_.step, QImage::Format_RGB888)));

    // In case of death
    if (players_[0].getHP() <= 0)
    {
        // Do death stuffs
        players_[0].setHP(1);
        ui->player1Lifebar->setValue(players_[0].getHP());
        attackTimer.stop();
        players_[1].setScore(players_[1].getScore() + 1);
        animationState_ = 100;
        ui->activePlayer1_timer->setPixmap(QPixmap());
        ui->activePlayer2_timer->setPixmap(QPixmap());

        // If the game is ended
        if (players_[1].getScore() >= 2)
        {
            // Score update
            players_[1].setScore(2);
            QString img2 = ":/img/GUI/P2 - " + QString::fromStdString(to_string(players_[1].getScore())) + "pt.png";
            ui->scorePlayer2Label->setPixmap(QPixmap::fromImage(QImage(img2)));

            // Graphics reset
            ui->activePlayer1->setStyleSheet("");
            ui->activePlayer2->setStyleSheet("");

            // Stop the game and ask for rematch
            ui->playPauseButton->setText("Replay ?");
            gameTimer.stop();

            // Victory display
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/win.png")));
        }
        else
        {
            // Prepare for restart and display the next round
            ui->titleLabel->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Round"+QString::fromStdString(to_string(players_[0].getScore()+players_[1].getScore()+1)) +".png")));
            restartTimer_.start();
        }
    }
    // Same for the second player
    if (players_[1].getHP() <= 0)
    {
        players_[1].setHP(1);
        ui->player2Lifebar->setValue(players_[1].getHP());
        attackTimer.stop();
        players_[0].setScore(players_[0].getScore() + 1);
        animationState_ = 100;
        ui->activePlayer1_timer->setPixmap(QPixmap());
        ui->activePlayer2_timer->setPixmap(QPixmap());
        if (players_[0].getScore() >= 2)
        {
            players_[0].setScore(2);
            QString img1 = ":/img/GUI/P1 - " + QString::fromStdString(to_string(players_[0].getScore())) + "pt.png";
            ui->scorePlayer1Label->setPixmap(QPixmap::fromImage(QImage(img1)));

            ui->activePlayer1->setStyleSheet("");
            ui->activePlayer2->setStyleSheet("");

            ui->playPauseButton->setText("Replay ?");
            gameTimer.stop();

            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/win.png")));
        }
        else
        {
            ui->titleLabel->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/Round"+QString::fromStdString(to_string(players_[0].getScore()+players_[1].getScore()+1)) +".png")));
            restartTimer_.start();
        }
    }
}

/** Swap the two players to swith the turn.
 * @brief MainWindow::switchTurn
 */
void MainWindow::switchTurn(){
    activePlayer_ = !activePlayer_;
    if (activePlayer_ == 1){
            ui->activePlayer1->setStyleSheet("background-color:rgba(116, 116, 116, 220)");
            ui->activePlayer2->setStyleSheet("");
    }else{
            ui->activePlayer2->setStyleSheet("background-color:rgba(116, 116, 116, 220)");
            ui->activePlayer1->setStyleSheet("");
    }
    time_ = -4;
}

/** Init the game
 * @brief MainWindow::initGame
 */
void MainWindow::initGame() {
    // Init Players
    players_.push_back(Player(100 , 0));
    players_.push_back(Player(100 , 0));
    ui->player1Lifebar->setValue(players_[0].getHP());
    ui->player2Lifebar->setValue(players_[1].getHP());

    // Init Score
    QString img1 = ":/img/GUI/P1 - " + QString::fromStdString(to_string(players_[0].getScore())) + "pt.png";
    ui->scorePlayer1Label->setPixmap(QPixmap::fromImage(QImage(img1)));
    QString img2 = ":/img/GUI/P2 - " + QString::fromStdString(to_string(players_[1].getScore())) + "pt.png";
    ui->scorePlayer2Label->setPixmap(QPixmap::fromImage(QImage(img2)));


    // Ulti and display of players
    ultimateCharge_ = 0;
    ui->ultProgressBar->setValue(ultimateCharge_);
    ui->activePlayer2_timer->setPixmap(QPixmap());
    ui->activePlayer1_timer->setPixmap(QPixmap());

    // Init Attacks
    attacks_.push_back(Attack("Lightning", 22, 2));
    attacks_.push_back(Attack("Bomb", 30, 1));
    attacks_.push_back(Attack("Pingouin", 37, 3));

    // Select the first player and display it
    activePlayer_ = rand()%2;
    switchTurn();

    // Start the game
    gameTimer.start();
    animationTimer_.start();
}

/** Restart the game
 * @brief MainWindow::restartGame
 */
void MainWindow::restartGame(){
    tatakaiSound_->play();
    // Clear the display
    ui->titleLabel->setPixmap(QPixmap());
    restartTimer_.stop();
    //Give HP back
    players_[0].setHP(100);
    players_[1].setHP(100);
    ui->player1Lifebar->setValue(players_[0].getHP());
    ui->player2Lifebar->setValue(players_[1].getHP());
    // Reset ultimate
    ultimateCharge_ = 0;
    ui->ultProgressBar->setValue(ultimateCharge_);
    // Reset score
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

/** Update the background using the mean of last images
 * @brief MainWindow::updateBackground
 */
void MainWindow::updateBackground(){
    Mat mat1, mat2, mat3, mat4, mat5;
    camera_ >> mat1;
    camera_ >> mat2;
    camera_ >> mat3;
    camera_ >> mat4;
    camera_ >> mat5;
    background_ = (mat1/5+mat2/5+mat3/5+mat4/5+mat5/5) ;
    cvtColor(background_,background_,COLOR_BGR2GRAY);
}

/** Take a picture and tell which attack it is
 * @brief MainWindow::imageProcessing
 */
void MainWindow::imageProcessing(){
    // Take the right picture depending on the active player
    Mat currentPic;
    flip(currentPic_,currentPic,1);
    Mat currentPlayer (currentPic, Rect(0 + 320*!activePlayer_, 0, 320, 480));
    Mat currentBG (background_, Rect(0 + 320*!activePlayer_, 0, 320, 480));
    cvtColor(currentPlayer,currentPlayer,COLOR_BGR2GRAY);

    // Substract the background
    subtract(currentBG,currentPlayer,skel_);

    // Do some image processing
    threshold(skel_, skel_, 25, 255, cv::THRESH_BINARY);
    Mat element = getStructuringElement(2,Size(18,18));
    morphologyEx( skel_, skel_, 1, element );
    element = getStructuringElement(2,Size(19,19));
    morphologyEx( skel_, skel_, 0, element );

    // Do a skeletonization
    thinning(skel_, skel_);

    // Do hough transform on the image
    skel_=Mat(skel_, Rect(5, 5, 310, 470));
    Mat color_dst;
    cvtColor(skel_,color_dst,CV_GRAY2BGR);
    vector<Vec4i> lines;
    HoughLinesP(skel_,lines,0.5, 0.5*CV_PI/180,20, 30, 30);
    sort(lines.begin(),lines.end(),[&](Vec4i i,Vec4i j) { return sqrt(pow(i[0]-i[2],2)+pow(i[1]-i[3],2)) > sqrt(pow(j[0]-j[2],2)+pow(j[1]-j[3],2));});
    // [DEV] Affichage lines Hough
    for (size_t i = 0;i<lines.size(); i++){
       line(color_dst,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(0,0,255),3,8);
    }
    vector<Vec4i> linesSelected;
    int mini = min((size_t)4,lines.size());
    for (size_t i=0;i<mini;i++){
        linesSelected.push_back(lines[i]);
        dataSelected_.push_back(getDatas(lines[i][0],lines[i][1],lines[i][2],lines[i][3]));
        line(color_dst,Point(lines[i][0],lines[i][1]),Point(lines[i][2],lines[i][3]),Scalar(255,0,0),3,8);
    }
    int i = 0;
    while (i < dataSelected_.size()){
        if (dataSelected_[i][0] > 4*dataSelected_[i][1]) dataSelected_.erase(dataSelected_.begin()+i);
        else i++;
    }
    // Get the attack detecterd
    currentAttack_ = verdict();
    attack();
    switchTurn();
}

/** Extract the data from hough transform
 * @brief MainWindow::getDatas
 * @param x1 lower edge of hough line
 * @param y1 lower edge of hough line
 * @param x2 upper edge of hough line
 * @param y2 upper edge of hough line
 * @return
 */
vector<float> MainWindow::getDatas(int x1,int y1,int x2,int y2){
    if (x1 == x2) x2 += 0.1*x2;
    float a=(y2-y1)/(x2-x1);
    if (a == 0) a = 0.01;
    float a2=-1/a;
    float b=y2-a*x2;
    float x=b/(a2-a);
    float y=a2*x;
    vector<float> datas{x,y};
    return datas;
}

/** Tell which is the attack according to hough lines.
 * @brief MainWindow::verdict
 * @return
 */
int MainWindow::verdict(){
    int verdict = 6;
    // References of the positions
    vector<float> eclair{-70,-100,-150,-200,-250,70,100,150,200,250};
    vector<float> eclair_bis{70,100,150,200,250,70,100,150,200,250};
    vector<float> meteor{70,100,150,200,250,-70,-100,-150,-200,-250,70,100,150,200,250,70,100,150,200,250};
    vector<float> pingouin{-3,-2,-4,300,200,400};

    // Lightning attack
    int m = dataSelected_.size();
    int m_ref = eclair.size()/2;
    vector<float> delta_eclair(m);
    int somme_delta_eclair = 0;
    for(int i = 0; i < m ; i++) delta_eclair[i] = sqrt(pow(dataSelected_[i][0]-eclair[0],2)+pow(dataSelected_[i][1]-eclair[eclair.size()/2],2));
    for(int i = 0 ; i < m ; i++){
        for(int j= 1; j < m_ref; j++){
            if( sqrt(pow(dataSelected_[i][0]-eclair[j],2)+pow(dataSelected_[i][1]-eclair[eclair.size()/2+j],2)) < delta_eclair[i]) {
                delta_eclair[i] = sqrt(pow(dataSelected_[i][0]-eclair[j],2)+pow(dataSelected_[i][1]-eclair[eclair.size()/2+j],2));
            }
        }
        somme_delta_eclair = somme_delta_eclair + delta_eclair[i];
    }
    m_ref = eclair_bis.size()/2;
    vector<float> delta_eclair_bis(m);
    int somme_delta_eclair_bis = 0;
    for(int i = 0; i < m; i++) delta_eclair_bis[i] = sqrt(pow(dataSelected_[i][0]-eclair_bis[0],2)+pow(dataSelected_[i][1]-eclair_bis[eclair_bis.size()/2],2));
    for(int i = 0 ; i < m; i++){
        for(int j= 1 ; j < m_ref; j++){
            if ( sqrt(pow(dataSelected_[i][0]-eclair_bis[j],2)+pow(dataSelected_[i][1]-eclair_bis[eclair_bis.size()/2+j],2)) < delta_eclair_bis[i]) {
                delta_eclair_bis[i] = sqrt(pow(dataSelected_[i][0]-eclair_bis[j],2)+pow(dataSelected_[i][1]-eclair_bis[eclair_bis.size()/2+j],2));
            }
        }
        somme_delta_eclair_bis = somme_delta_eclair_bis + delta_eclair_bis[i];
    }

    // Bomb attack
    m_ref = meteor.size()/2;
    vector<float> delta_meteor(m);
    int somme_delta_meteor = 0;
    for(int i = 0; i < m; i++)delta_meteor[i] = sqrt(pow(dataSelected_[i][0]-meteor[0],2)+pow(dataSelected_[i][1]-meteor[meteor.size()/2],2));
    for(int i = 0; i < m; i++){
        for(int j= 1; j < m_ref; j++){
            if( sqrt(pow(dataSelected_[i][0]-meteor[j],2)+pow(dataSelected_[i][1]-meteor[meteor.size()/2+j],2)) < delta_meteor[i]) {
                delta_meteor[i] = sqrt(pow(dataSelected_[i][0]-meteor[j],2)+pow(dataSelected_[i][1]-meteor[meteor.size()/2+j],2));
            }
        }
        somme_delta_meteor = somme_delta_meteor + delta_meteor[i];
    }

    // Pingouin attack
    m_ref = pingouin.size()/2;
    vector<float> delta_pingouin(m);
    int somme_delta_pingouin = 0;
    for(int i = 0; i < m; i++)delta_pingouin[i] = sqrt(pow(dataSelected_[i][0]-pingouin[0],2)+pow(dataSelected_[i][1]-pingouin[pingouin.size()/2],2));
    for(int i = 0; i < m; i++){
        for(int j= 1; j < m_ref; j++){
            if( sqrt(pow(dataSelected_[i][0]-pingouin[j],2)+pow(dataSelected_[i][1]-pingouin[pingouin.size()/2+j],2)) < delta_pingouin[i]) {
                delta_pingouin[i] = sqrt(pow(dataSelected_[i][0]-pingouin[j],2)+pow(dataSelected_[i][1]-pingouin[pingouin.size()/2+j],2));
            }
        }
        somme_delta_pingouin = somme_delta_pingouin + delta_pingouin[i];
    }

    // The closest from the reference is chosed
    vector<int> sommes{somme_delta_eclair,somme_delta_eclair_bis,somme_delta_meteor,somme_delta_pingouin};
    if (somme_delta_eclair == *min_element(sommes.begin(), sommes.end()) && somme_delta_eclair < 600) verdict = 0;
    else if (somme_delta_eclair_bis == *min_element(sommes.begin(), sommes.end()) && somme_delta_eclair_bis <600) verdict = 0;
    else if (somme_delta_meteor == *min_element(sommes.begin(), sommes.end()) && somme_delta_meteor < 600) verdict = 1;
    else if (somme_delta_pingouin == *min_element(sommes.begin(), sommes.end()) && somme_delta_pingouin < 600) verdict = 2;
    else verdict = 3;
    dataSelected_.clear();
    return verdict;
}

/** Perform the attack
 * @brief MainWindow::attack
 */
void MainWindow::attack(){
    // Sounds
    if (currentAttack_ == 0) lightningSound_->play();
    if (currentAttack_ == 1) bombSound_->play();
    if (currentAttack_ == 2) pingouinSound_->play();

    // Damages and ultimate charge
    attacks_[currentAttack_].damage(&players_[!activePlayer_],ultimateCharge_>=10);
    if (ultimateCharge_ >= 10) ultimateCharge_ = 0;
    ultimateCharge_ += attacks_[currentAttack_].getUltCharge();
    if (ultimateCharge_ >= 10) ultimateCharge_ = 10;

    // Animation
    animationState_ = 0;

    // HealthBar updates
    ui->ultProgressBar->setValue(ultimateCharge_);
    ui->player1Lifebar->setValue(players_[0].getHP());
    ui->player2Lifebar->setValue(players_[1].getHP());
}

/** Attacks animation
 * @brief MainWindow::animateAttacks
 */
void MainWindow::animateAttacks(){

    if (currentAttack_ == 0) // Lightning
    {
        if (animationState_ == 1) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair1"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair1")));
        if (animationState_ == 2) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair2"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair2")));
        if (animationState_ == 3) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair3"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair3")));
        if (animationState_ == 4) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair4"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair4")));
        if (animationState_ == 5) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair5"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair5")));
        if (animationState_ == 6) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair6"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair6")));
        if (animationState_ == 7) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair7"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair7")));
        if (animationState_ == 8) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair8"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair8")));
        if (animationState_ == 9) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair9"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair9")));
        if (animationState_ == 10) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair10"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair10")));
        if (animationState_ == 11) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair11"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair11")));
        if (animationState_ == 12) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair12"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair12")));
        if (animationState_ == 13) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair13"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair13")));
        if (animationState_ == 14) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair14"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair14")));
        if (animationState_ == 15) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair15"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair15")));
        if (animationState_ == 16) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair16"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair16")));
        if (animationState_ == 17) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair17"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair17")));
        if (animationState_ == 18) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair18"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair18")));
        if (animationState_ == 19) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair19"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair19")));
        if (animationState_ == 20) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair20"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair20")));
        if (animationState_ == 21) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair21"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair21")));
        if (animationState_ == 22) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair22"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair22")));
        if (animationState_ == 23) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair23"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair23")));
        if (animationState_ == 24) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair24"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair24")));
        if (animationState_ == 25) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair25"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair25")));
        if (animationState_ == 26) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair26"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair26")));
        if (animationState_ == 27) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair27"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair27")));
        if (animationState_ == 28) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair28"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair28")));
        if (animationState_ == 29) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair29"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair29")));
        if (animationState_ == 30) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair30"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair30")));
        if (animationState_ == 31) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair31"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair31")));
        if (animationState_ == 32) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair32"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair32")));
        if (animationState_ == 33) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair33"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair33")));
        if (animationState_ == 34) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair34"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair34")));
        if (animationState_ == 35) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair35"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair35")));
        if (animationState_ == 36) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair36"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair36")));
        if (animationState_ == 37) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair37"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/storm/eclair37")));
        if (animationState_ == 38) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap()) : ui->activePlayer2_timer->setPixmap(QPixmap());
    }

    if (currentAttack_ == 1) //Bomb
    {
       if (animationState_ == 1) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb1"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb1")));
       if (animationState_ == 2) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb2"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb2")));
       if (animationState_ == 3) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb3"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb3")));
       if (animationState_ == 4) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb4"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb4")));
       if (animationState_ == 5) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb5"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb5")));
       if (animationState_ == 6) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb6"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb6")));
       if (animationState_ == 7) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb7"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb7")));
       if (animationState_ == 8) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb8"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb8")));
       if (animationState_ == 9) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb9"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb9")));
       if (animationState_ == 10) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb10"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb10")));
       if (animationState_ == 11) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb11"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb11")));
       if (animationState_ == 12) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb12"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb12")));
       if (animationState_ == 13) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb13"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb13")));
       if (animationState_ == 14) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb14"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb14")));
       if (animationState_ == 15) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb15"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb15")));
       if (animationState_ == 16) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb16"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb16")));
       if (animationState_ == 17) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb17"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb17")));
       if (animationState_ == 18) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb18"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb18")));
       if (animationState_ == 19) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb19"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb19")));
       if (animationState_ == 20) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb20"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb20")));
       if (animationState_ == 21) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb21"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb21")));
       if (animationState_ == 22) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb22"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb22")));
       if (animationState_ == 23) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb23"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb23")));
       if (animationState_ == 24) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb24"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb24")));
       if (animationState_ == 25) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb25"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb25")));
       if (animationState_ == 26) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb26"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb26")));
       if (animationState_ == 27) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb27"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb27")));
       if (animationState_ == 28) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb28"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb28")));
       if (animationState_ == 29) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb29"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb29")));
       if (animationState_ == 30) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb30"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb30")));
       if (animationState_ == 31) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb31"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb31")));
       if (animationState_ == 32) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb32"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb32")));
       if (animationState_ == 33) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb33"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb33")));
       if (animationState_ == 34) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb34"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb34")));
       if (animationState_ == 35) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb35"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb35")));
       if (animationState_ == 36) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb36"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb36")));
       if (animationState_ == 37) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb37"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb37")));
       if (animationState_ == 38) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb38"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb38")));
       if (animationState_ == 39) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb39"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb39")));
       if (animationState_ == 40) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb40"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb40")));
       if (animationState_ == 41) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb41"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb41")));
       if (animationState_ == 42) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb42"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb42")));
       if (animationState_ == 43) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb43"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/bomb/bomb43")));
       if (animationState_ == 44) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap()) : ui->activePlayer2_timer->setPixmap(QPixmap());
    }

    if (currentAttack_==2){ // Pingouin
        if (animationState_ == 1) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall1"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall1")));
        if (animationState_ == 2) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall2"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall2")));
        if (animationState_ == 3) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall3"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall3")));
        if (animationState_ == 4) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall4"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall4")));
        if (animationState_ == 5) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall5"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall5")));
        if (animationState_ == 6) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall6"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall6")));
        if (animationState_ == 7) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall7"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall7")));
        if (animationState_ == 8) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall8"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall8")));
        if (animationState_ == 9) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall9"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall9")));
        if (animationState_ == 10) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall10"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall10")));
        if (animationState_ == 11) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall11"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall11")));
        if (animationState_ == 12) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall12"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall12")));
        if (animationState_ == 13) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall13"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall13")));
        if (animationState_ == 14) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall14"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall14")));
        if (animationState_ == 15) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall15"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall15")));
        if (animationState_ == 16) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall16"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall16")));
        if (animationState_ == 17) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall17"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall17")));
        if (animationState_ == 18) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall18"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall18")));
        if (animationState_ == 19) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall19"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall19")));
        if (animationState_ == 20) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall20"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall20")));
        if (animationState_ == 21) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall21"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall21")));
        if (animationState_ == 22) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall22"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall22")));
        if (animationState_ == 23) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall23"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall23")));
        if (animationState_ == 24) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall24"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall24")));
        if (animationState_ == 25) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall25"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall25")));
        if (animationState_ == 26) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall26"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall26")));
        if (animationState_ == 27) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall27"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall27")));
        if (animationState_ == 28) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall28"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall28")));
        if (animationState_ == 29) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall29"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/pingouin/fall29")));
        if (animationState_ == 30) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap()) : ui->activePlayer2_timer->setPixmap(QPixmap());
    }

    if(currentAttack_ == 3){ // Flop
        if (animationState_ == 1) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté1"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté1")));
        if (animationState_ == 2) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté2"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté2")));
        if (animationState_ == 3) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté3"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté3")));
        if (animationState_ == 4) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté4"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté4")));
        if (animationState_ == 5) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté5"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté5")));
        if (animationState_ == 6) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté6"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté6")));
        if (animationState_ == 7) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté7"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté7")));
        if (animationState_ == 8) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté8"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté8")));
        if (animationState_ == 9) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté9"))) : ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/flop/raté9")));
        if (animationState_ == 10) (!activePlayer_) ? ui->activePlayer1_timer->setPixmap(QPixmap()) : ui->activePlayer2_timer->setPixmap(QPixmap());

    }
}

/** Update the timer of the attack
 * @brief MainWindow::timerForAttack
 */
void MainWindow::timerForAttack(){
    if (!activePlayer_ && time_ >= 1){
        if (time_ == 1) {
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/3.png")));
        }
        else if (time_ == 2) {
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/2.png")));
        }
        else if (time_ == 3) {
            ui->activePlayer1_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/1.png")));
        }
        else {
            ui->activePlayer1_timer->setPixmap(QPixmap());
            imageProcessing();
        }
    }
    else if (time_ >= 1){
        if (time_ == 1) {
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/3.png")));
        }
        else if (time_ == 2) {
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/2.png")));
        }
        else if (time_ == 3) {
            ui->activePlayer2_timer->setPixmap(QPixmap::fromImage(QImage(":/img/GUI/1.png")));
        }
        else {
            ui->activePlayer2_timer->setPixmap(QPixmap());
            imageProcessing();
        }
    }
    time_++;
}

/** Do the thinning operations
 * @brief MainWindow::thinningIteration
 * @param img the image to thin
 * @param iter the number of iteration
 */
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

/** Function for thinning the given binary image
 * @brief MainWindow::thinning
 * @param src The source image, binary with range = [0,255]
 * @param dst The destination image
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
    cvtColor(tmp, tmp, CV_BGR2RGB);
    ui->player1Label->setPixmap(QPixmap::fromImage(QImage(tmp.data, tmp.cols, tmp.rows,tmp.step, QImage::Format_RGB888)));
}

void MainWindow::on_updatePlayer2PicButton_clicked()
{
    Mat tmp;
    camera_ >> tmp;
    tmp = Mat(tmp,Rect(0,0,320,480));
    cv::flip(tmp,tmp,1);
    cvtColor(tmp, tmp, CV_BGR2RGB);
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
        ui->titleLabel->setPixmap(QPixmap());
    }
}

void MainWindow::on_startingButton_clicked()
{
    ui->startingButton->hide();
    ui->startingScreenBG->hide();
    tatakaiSound_->play();
    QPixmap bkgnd(":/img/GUI/BG1.PNG");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
    ui->titleLabel->setPixmap(QPixmap());
}
