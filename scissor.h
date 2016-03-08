#ifndef SCISSOR_H
#define SCISSOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <QMouseEvent>
#include <QTime>
#include <queue>


#define INITIAL 0
#define ACTIVE 1
#define EXPANDED 2
#define INF 1000000
#define XOFFSET 12
#define YOFFSET 50
#define CLOSETHRES 40

using namespace cv;

struct Node{
    double linkCost[9];
    int state;
    double totalCost;
    Node *prevNode;
    int prevNodeNum;
    int col,row;
    int num;
    bool operator < (const Node &a) const {
        return totalCost>a.totalCost;
    }
};

namespace Ui {
class scissor;
}

class scissor : public QMainWindow
{
    Q_OBJECT

public:
    explicit scissor(QWidget *parent = 0);
    ~scissor();

private slots:
    void openFile();

    void on_actionOpen_triggered();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_actionOrigin_Map_triggered();

    void on_actionGradient_Map_triggered();

    void on_actionStart_triggered();

    void Zoom_in_out(double index);

    void show_image(cv::Mat const& src,int n);

    void gradient();

    void showNode(struct Node *node );

    //void delaymsec(int msec);

    //void mousePressEvent(QMouseEvent *event);

    //void mouseMoveEvent(QMouseEvent *event);

    bool eventFilter(QObject *obj, QEvent *event);

    void findPathGraph(int x, int y);

    void moveMouse(int x, int y);

    void getSeedImage();

    void initial();


    void on_actionBack_triggered();

    void on_actionRestart_triggered();

    void closeDetect(int x, int y);

    void on_actionDrawNodeVector_triggered();

    void on_actionSave_Image_triggered();

    void getMask(int x,int y);

    void on_actionGet_Mask_triggered();

    void on_actionSave_Mask_triggered();

    void on_actionExit_triggered();

private:
    Ui::scissor *ui;
    int Is_load = 0;
    int Is_grad = 0;
    int Init_seed = 0;
    int Is_start = 0;
    int Is_graphed = 0;
    int Is_closed = 0;
    int Is_showgrad = 0;
    int Is_mask = 0;
    cv::Mat image;
    cv::Mat back_image;
    cv::Mat grad_image;
    cv::Mat seed_image;
    cv::Mat mask_image;
    double zoom_index = 1;
    int seed_num;
    int seed_x;
    int seed_y;

    vector<struct Node> node_vector;
    vector<cv::Point2d> seed_vector;
    vector<vector<cv::Point2d> > fullPath_vector;
    vector<cv::Point2d> shortPath_vector;
};



#endif // SCISSOR_H
