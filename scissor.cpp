#include "scissor.h"
#include "ui_scissor.h"

using namespace cv;
using namespace std;

QImage Mat2QImage(cv::Mat const& src)
{
    /*
    Mat inv_src(src.cols, src.rows,CV_8UC3, Scalar(255,255,255));
    for(int i = 0; i<inv_src.rows;i++)
        for(int j = 0;j<inv_src.cols;j++)
        {
            inv_src.at<Vec3b>(i,j) = src.at<Vec3b>(j,i);
        }
        */


    cv::Mat temp;
    cvtColor(src, temp,CV_BGR2RGB);
    QImage dest((const uchar *) temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    dest.bits(); // enforce deep copy, see documentation
    // of QImage::QImage ( const uchar * data, int width, int height, Format format )
    return dest;
}

cv::Mat QImage2Mat(QImage const& src)
{
    cv::Mat tmp(src.height(),src.width(),CV_8UC3,(uchar*)src.bits(),src.bytesPerLine());
    cv::Mat result; // deep copy just in case (my lack of knowledge with open cv)
    cvtColor(tmp, result,CV_BGR2RGB);

    Mat inv_result(result.cols, result.rows,CV_8UC3, Scalar(255,255,255));
    for(int i; i<result.rows;i++)
        for(int j;j<result.cols;j++)
        {
            inv_result.at<Vec3b>(i,j) = result.at<Vec3b>(j,i);
        }



    return inv_result;
}


scissor::scissor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::scissor)
{
    this->setMouseTracking(true);
    ui->setupUi(this);
    qApp->installEventFilter(this);
    //ui->label->installEventFilter(this);
}

void scissor::initial()
{
    seed_vector.clear();
    fullPath_vector.clear();
    shortPath_vector.clear();
    Is_graphed = 0;
    Init_seed = 0;
    Is_closed = 0;
    Is_showgrad = 0;
    Is_mask = 0;
    back_image = image.clone();
    seed_image = image.clone();
    mask_image = Mat(image.cols, image.rows,CV_8UC3, Scalar(255,255,255));

            ui->label_2->clear();
}

void scissor::openFile()
{
    QString filename = QFileDialog::getOpenFileName(
                this,
                tr("Open Image"),
                "/home/tony-pc/qt",
                tr("Image files (*.jpg *.png );;All files (*.*)") );
    if(filename.length() != 0)
    {
        Is_load = 1;
        Is_grad = 0;
        Init_seed = 0;
        Is_start = 0;
        Is_graphed = 0;
        Is_closed = 0;
        Is_showgrad = 0;
        Is_mask = 0;
        ui->label->clear();
        ui->label_2->clear();


        std::string file = filename.toUtf8().constData();
        image = cv::imread(file,1);
        back_image = image.clone();
        seed_image = image.clone();
        mask_image = Mat(image.cols, image.rows,CV_8UC3, Scalar(255,255,255));
        //cv::namedWindow("image",CV_WINDOW_AUTOSIZE);
        show_image(image,0);
        cout<<"rows * cols"<<image.rows * image.cols<<endl;
    }
}


scissor::~scissor()
{
    delete ui;
}

void scissor::on_actionOpen_triggered()
{
    openFile();
}

void scissor::on_actionZoom_In_triggered()
{
    if(!Is_load)
        return;
    if(Is_start)
        return;
    zoom_index = 2;
    Zoom_in_out(zoom_index);
    std::cout<<"zoom in"<<std::endl;
}

void scissor::on_actionZoom_Out_triggered()
{
    if(Is_start)
        return;
    if(!Is_load)
        return;
    zoom_index = 0.5;
    Zoom_in_out(zoom_index);
    std::cout<<"zoom out"<<std::endl;
}

void scissor::Zoom_in_out(double index)
{
    if(!Is_load)
        return;

    Mat tmp;
    tmp = back_image.clone();
    cout<<index<<endl;

    if(index > 1)
        pyrUp( tmp, image, Size( tmp.cols * index, tmp.rows * index ) );
    else if(index < 1)
        pyrDown( tmp, image, Size( tmp.cols * index, tmp.rows * index ) );

    show_image(image,0);

}


void scissor::on_actionOrigin_Map_triggered()
{
    if(!Is_load)
        return;
    image = back_image.clone();
    show_image(image,0);
}

void scissor::show_image(cv::Mat const& src, int n)
{
    //imshow( "image", src );

    QPixmap new_pixmap = QPixmap::fromImage(Mat2QImage( src ));

    int w = new_pixmap.width();
    int h = new_pixmap.height();
    if(n==0)
    {
        ui->label->resize(w,h);
        ui->label->setPixmap(new_pixmap);

    }
    else if (n==1)
    {
        ui->label_2->resize(w,h);
        ui->label_2->setPixmap(new_pixmap);
    }

    ui->scrollArea->show();
}

void scissor::gradient()
{
    cout<<"gradient()"<<endl;
    Is_grad = 1;
    Mat tmp_image((image.rows - 2) * 3, (image.cols - 2 ) * 3 ,CV_8UC3, Scalar(255,255,255));
    grad_image = tmp_image.clone();
    double maxD = 0;
    double link = 0;
    for(int i = 1 ;i < image.rows - 1; i++)
        for(int j = 1; j < image.cols - 1; j++)
        {

            int x = (i-1) * 3 + 1;
            int y = (j-1) * 3 + 1;

            //x,y
            grad_image.at<Vec3b>(x,y) = image.at<Vec3b>(i,j);

            //Dlink
            //x-1,y-1
            double sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i,j-1)[k] - image.at<Vec3b>(i-1,j)[k]),2);
            }
            link = sqrt(sum / 6.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
            {
                grad_image.at<Vec3b>(x-1,y-1)[k] = link;
            }

            //x+1,y-1
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i,j-1)[k] - image.at<Vec3b>(i+1,j)[k]),2);
            }
            link = sqrt(sum / 6.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x+1,y-1)[k] = link;

            //x-1,y+1
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i-1,j)[k] - image.at<Vec3b>(i,j+1)[k]),2);
            }
            link = sqrt(sum / 6.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x-1,y+1)[k] = link;

            //x+1,y+1
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i,j+1)[k] - image.at<Vec3b>(i+1,j)[k]),2);
            }
            link = sqrt(sum / 6.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x+1,y+1)[k] = link;

            //x,y-1
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i-1,j)[k] + image.at<Vec3b>(i-1,j-1)[k] - image.at<Vec3b>(i+1,j)[k] - image.at<Vec3b>(i+1,j-1)[k])/ 4.0,2) ;
            }
            link = sqrt(sum / 3.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x,y-1)[k] = link;

            //x-1,y
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i-1,j-1)[k] + image.at<Vec3b>(i,j-1)[k] - image.at<Vec3b>(i-1,j+1)[k] - image.at<Vec3b>(i,j+1)[k])/ 4.0,2) ;
            }
            link = sqrt(sum / 3.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x-1,y)[k] = link;

            //x+1,y
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i,j-1)[k] + image.at<Vec3b>(i+1,j-1)[k] - image.at<Vec3b>(i,j+1)[k] - image.at<Vec3b>(i+1,j+1)[k])/ 4.0,2) ;
            }
            link = sqrt(sum / 3.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x+1,y)[k] = link;

            //x,y+1
            sum = 0;
            for(int k = 0; k <= 2 ; k++)
            {
                sum += pow((image.at<Vec3b>(i-1,j)[k] + image.at<Vec3b>(i-1,j+1)[k] - image.at<Vec3b>(i+1,j)[k] - image.at<Vec3b>(i+1,j+1)[k])/ 4.0,2) ;
            }
            link = sqrt(sum / 3.0);
            if (link > maxD)
                maxD = link;
            for(int k = 0; k <= 2; k++)
                grad_image.at<Vec3b>(x,y+1)[k] = link;
        }
    //show_image(grad_image);

    cout<<"gradient maxD"<<maxD<<endl;

    //update cost
    double a = 1.0;
    for(int i = 1 ;i < image.rows - 1; i++)
        for(int j = 1; j < image.cols - 1; j++)
        {
            int x = (i-1) * 3 + 1;
            int y = (j-1) * 3 + 1;
            for(int k = 0; k <= 2 ;k++)
            {
                grad_image.at<Vec3b>(x,y)[k] = 255;
                grad_image.at<Vec3b>(x-1,y-1)[k] = (maxD - grad_image.at<Vec3b>(x-1,y-1)[k]) * 1.414 * a;
                grad_image.at<Vec3b>(x+1,y-1)[k] = (maxD - grad_image.at<Vec3b>(x+1,y-1)[k]) * 1.414 * a;
                grad_image.at<Vec3b>(x-1,y+1)[k] = (maxD - grad_image.at<Vec3b>(x-1,y+1)[k]) * 1.414 * a;
                grad_image.at<Vec3b>(x+1,y+1)[k] = (maxD - grad_image.at<Vec3b>(x+1,y+1)[k]) * 1.414 * a;
                grad_image.at<Vec3b>(x,y-1)[k] = (maxD - grad_image.at<Vec3b>(x,y-1)[k]) * a;
                grad_image.at<Vec3b>(x-1,y)[k] = (maxD - grad_image.at<Vec3b>(x-1,y)[k]) * a;
                grad_image.at<Vec3b>(x+1,y)[k] = (maxD - grad_image.at<Vec3b>(x+1,y)[k]) * a;
                grad_image.at<Vec3b>(x,y+1)[k] = (maxD - grad_image.at<Vec3b>(x,y+1)[k]) * a;
            }
        }

}

void scissor::on_actionGradient_Map_triggered()
{
    if(!Is_load)
        return;

    gradient();

    imshow("image",grad_image);
    //imwrite( "Gray_Image.jpg", grad_image );
}

void scissor::on_actionStart_triggered()
{
    cout<<"actionStart()"<<endl;
    if(!Is_load)
        return;
    if(!Is_grad)
        gradient();
    Is_start = 1;

    // Node Struct
    node_vector.clear();
    for(int i = 0 ;i < image.rows ; i++)
        for(int j = 0; j < image.cols ; j++)
        {
            struct Node node;
            node.row = i;
            node.col = j;
            node.state = INITIAL;
            node.prevNode = NULL;
            node.totalCost = INF;
            node.prevNodeNum = 0;
            node.num = i * image.cols + j;
            int x = (i-1) * 3 + 1;
            int y = (j-1) * 3 + 1;
            int cnt = 0;
            if(i == 0 || j == 0 || i == image.rows - 1 || j ==image.cols - 1)
            {
                for(int k = 0 ;k < 9;k++)
                    node.linkCost[k] = INF;
            }
            else
                for(int l = -1; l <= 1; l++)
                    for(int m = -1; m <= 1;m++)
                    {
                        if(l == 0 && m ==0)
                            node.linkCost[cnt]=0;
                        else
                            node.linkCost[cnt]=grad_image.at<Vec3b>(x+l,y+m)[0];
                        cnt++;
                    }
            //cout<<"cnt"<<cnt<<endl;
            node_vector.push_back(node);
        }
}
/*
void scissor::mousePressEvent(QMouseEvent *event)

{

    if (event->button()==Qt::LeftButton)
    {
        int x=event->x();
        int y=event->y();
        cout<<"x  "<<x<<"y   "<<y<<endl;
    }


}
*/


bool scissor::eventFilter(QObject *obj, QEvent *event)
{
    if(qobject_cast<QLabel*>(obj)==ui->label &&event->type() == QEvent::MouseButtonPress)
    {
        cout<<"click"<<endl;
        if(Is_start)
        {
            Is_graphed = 0;
            if(!Init_seed)
            {
                seed_vector.clear();
                fullPath_vector.clear();
                shortPath_vector.clear();
                cout<<"init_seed"<<endl;
                Init_seed = 1;
            }
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            int x = mouseEvent->pos().y();
            int y =mouseEvent->pos().x();
            cout<<"mouse click "<<x<<"  "<<y<<endl;
            if(x < image.rows && y < image.cols)
                if(!Is_closed)
                    findPathGraph(x,y);
                else
                    if(Is_mask)
                        getMask(x,y);
            closeDetect(x,y);

        }
    }
    if (qobject_cast<QLabel*>(obj)==ui->label &&event->type() == QEvent::MouseMove)
    {

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);


        statusBar()->showMessage(QString("Mouse move (%1,%2)").arg(mouseEvent->pos().y()).arg(mouseEvent->pos().x()));
        if(Is_graphed == 1)
        {
            int x = mouseEvent->pos().y();
            int y = mouseEvent->pos().x();
            //cout<<"x  "<<x<<"y   "<<y<<endl;
            if(x < image.rows && y < image.cols && !Is_closed)
            {
                moveMouse(x,y);
                cout<<"move mouse"<<x<<"  " <<y <<endl;
            }
        }
    }
    return false;
}
/*
                void scissor::delaymsec(int msec)
                {
                    QTime dieTime = QTime::currentTime().addMSecs(msec);

                    while( QTime::currentTime() < dieTime )

                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                }


                void scissor::delaymsec(int msec)
                {

                    QTime n=QTime::currentTime();

                    QTime now;
                    do{
                      now=QTime::currentTime();
                    }while (n.msecsTo(now)<=msec);

                }
                */
void scissor::findPathGraph(int x, int y)
{
    cout<<" findPathGraph seed   "<<x<<"   "<<y<<endl;
    /*
                    for(int i = 20 ;i < node_vector.size();i++)
                    {
                        cout<<"up"<<i<<endl;

                            cout<<node_vector[i].linkCost[7]<<endl;

                        cout<<"down"<<i + image.cols - 2 +1<<endl;

                            cout<<node_vector[i + image.cols - 2 +1].linkCost[0]<<endl;
                        if(i>26)
                            break;
                    }
                    */

    cv::Point2d point(y,x);
    seed_vector.push_back(point);
    fullPath_vector.push_back(shortPath_vector);
    getSeedImage();
    //build edge
    //int node_num = node_vector.size();
    //int deges[node_num][node_num];

    //init total cost
    //cout<<"node "<<node_vector[x * image.cols + y].row<<"  "<<node_vector[x * image.cols + y].col;

    //vector initiate
    for(int i = 0 ;i<node_vector.size();i++)
    {
        node_vector[i].state = INITIAL;
        node_vector[i].totalCost = INF;
        node_vector[i].prevNodeNum = 0;
        node_vector[i].prevNode = NULL;
    }
    seed_x = x;
    seed_y = y;
    seed_num = x * image.cols + y;
    node_vector[seed_num].state = INITIAL;
    node_vector[seed_num].totalCost = 0;
    node_vector[seed_num].prevNodeNum = seed_num;
    priority_queue<Node> que;
    que.push(node_vector[seed_num]);
    while(!que.empty())
    {
        Node q = que.top();
        que.pop();
        if(node_vector[q.num].state == EXPANDED)
            continue;
        q.state = EXPANDED;
        node_vector[q.num].state = EXPANDED;
        for(int i = -1 ;i <= 1;i++)
            for(int j = -1 ;j <= 1;j++)
            {
                //q.row q.col = boundry
                if(q.row == 0 || q.col == 0 || q.row == image.rows - 1 || q.col == image.cols - 1)
                    continue;
                if(i == 0 && j==0)
                    continue;
                Node r = node_vector[(q.row + i) * image.cols + (q.col + j)];
                if(r.state == INITIAL)
                {
                    //cout<<"DEbug i "<<i<<"j  "<<j<<endl;
                    //cout<<"linkcost"<<(i+1) * 3 + j + 1<<endl;
                    r.prevNode = &q;
                    r.totalCost = q.totalCost + q.linkCost[(i+1) * 3 + j + 1];
                    r.state = ACTIVE;
                    r.prevNodeNum = q.num;
                    node_vector[r.num].prevNode = &q;
                    node_vector[r.num].prevNodeNum = q.num;
                    node_vector[r.num].totalCost = q.totalCost + q.linkCost[(i+1) * 3 + j + 1];
                    node_vector[r.num].state = ACTIVE;
                    que.push(r);
                }
                else if(r.state == ACTIVE)
                {
                    if(q.totalCost + q.linkCost[(i+1) * 3 + j + 1] < r.totalCost)
                    {
                        r.prevNode = &q;
                        r.totalCost = q.totalCost + q.linkCost[(i+1) * 3 + j + 1];
                        r.prevNodeNum = q.num;
                        node_vector[r.num].prevNode = &q;
                        node_vector[r.num].prevNodeNum = q.num;
                        node_vector[r.num].totalCost = q.totalCost + q.linkCost[(i+1) * 3 + j + 1];
                        //update que
                        que.push(r);


                    }

                }

            }
    }
    Is_graphed = 1;
    cout<<"find path Graph "<<endl;

    //showNode(&node_vector[seed_num]);
}

void scissor::showNode(struct Node *node )
{
    cout<<"num  "<<node->num<<endl;
    cout<<"position  "<<node->row<<"  "<<node->col<<endl;
    cout<<"prevNodeNum  "<<node->prevNodeNum<<endl;
    cout<<"total cost  "<<node->totalCost<<endl;
    for(int i = 0 ;i<3;i++)
        cout<<"  "<<node->linkCost[i];
    cout<<endl;
    for(int i = 3 ;i<6;i++)
        cout<<"  "<<node->linkCost[i];
    cout<<endl;
    for(int i = 6 ;i<9;i++)
        cout<<"  "<<node->linkCost[i];
    cout<<endl;
}

void scissor::moveMouse(int x, int y)
{
    if(!Is_graphed)
        return;
    Node des_node = node_vector[x * image.cols + y];
    Mat line_image = seed_image.clone();
    shortPath_vector.clear();
    shortPath_vector.push_back(Point2f(des_node.col,des_node.row));
    while(des_node.prevNodeNum != seed_num)
    {
        cv::Point2d pointa(des_node.col,des_node.row);
        int next_num = des_node.prevNodeNum;
        des_node = node_vector[next_num];
        cv::Point2d pointb(des_node.col,des_node.row);
        cv::line(line_image,pointa,pointb,cv::Scalar(255, 0, 255),2,8,0);
        shortPath_vector.push_back(pointb);

    }
    shortPath_vector.push_back(Point2d(seed_y,seed_x));
    show_image(line_image,0);
}

void scissor::getSeedImage()
{
    cout<<"getSeedImage()"<<endl;
    seed_image = image.clone();
    mask_image = Mat(image.cols, image.rows,CV_8UC3, Scalar(255,255,255));
    if(!shortPath_vector.empty())
    {
        for(int i = 0;i < seed_vector.size();i++)
        {
            if(i==0)
                circle(seed_image,seed_vector[i],2, cv::Scalar(0, 255, 255 ), 2);
            else
                circle(seed_image,seed_vector[i],2, cv::Scalar(0, 255, 0 ), 2);
            circle(grad_image,Point2d(((seed_vector[i].x-1)*3+1),(seed_vector[i].y-1)*3+1),2, cv::Scalar(255, 0, 255 ), 2);
            //cout<<"i"<<i<<endl;
            //cout<<"size"<<fullPath_vector.size()<<endl;
            if(fullPath_vector[i].size() < 2)
                continue;
            for(int j = 0; j < fullPath_vector[i].size()-1; j++)
            {
                if(Is_closed)
                    cv::line(seed_image,fullPath_vector[i][j],fullPath_vector[i][j+1],cv::Scalar(255, 255, 255),2,8,0);
                else
                    cv::line(seed_image,fullPath_vector[i][j],fullPath_vector[i][j+1],cv::Scalar(255, 0, 0),2,8,0);
                cv::line(mask_image,fullPath_vector[i][j],fullPath_vector[i][j+1],cv::Scalar(0, 0, 0),1,8,0);
                cv::line(grad_image,Point2d(fullPath_vector[i][j].x*3-2,fullPath_vector[i][j].y*3-2),Point2d(fullPath_vector[i][j+1].x*3-2,fullPath_vector[i][j+1].y*3-2),cv::Scalar(255, 0, 0),2,8,0);

            }

        }

    }
    show_image(seed_image,0);
    if(Is_showgrad)
        imshow("image",grad_image);
    //imshow("image",grad_image);

}

void scissor::on_actionBack_triggered()
{
    if(Is_closed)
        return;
    cout<<"actionBack()"<<endl;
    if(seed_vector.size() > 0 )
    {
        seed_vector.pop_back();
        fullPath_vector.pop_back();


        if(seed_vector.size() > 0)
        {
            Point2d point = seed_vector.back();
            findPathGraph((int)point.y, (int)point.x);
            seed_vector.pop_back();
            fullPath_vector.pop_back();
            if(seed_vector.empty())
            {
                Is_graphed = 0;
                shortPath_vector.clear();
            }

        }
        else
        {
            Is_graphed = 0;
            shortPath_vector.clear();
        }
        getSeedImage();
    }

}

void scissor::on_actionRestart_triggered()
{
    if(!Is_load)
        return;

    initial();
    getSeedImage();
}

void scissor::closeDetect(int x, int y)
{
    cout<<"closeDetect"<<endl;
    if(seed_vector.size() <= 1)
        return;
    else
    {
        int distance ;
        distance = (x - seed_vector[0].y) * (x - seed_vector[0].y) + (y - seed_vector[0].x) * (y -seed_vector[0].x);
        if( distance < CLOSETHRES)
        {
            Is_closed = 1;
            cout<<"close"<<endl;
            moveMouse(seed_vector[0].y,seed_vector[0].x);
            findPathGraph(seed_vector[0].y,seed_vector[0].x);
            //getMask();
        }
    }

}

void scissor::getMask(int x,int y)
{
    cout<<"get MASK"<<endl;
    //imshow("mask",mask_image);
    Rect ccomp;
    floodFill(mask_image,Point2d(y,x),Scalar(0, 0, 0), &ccomp, Scalar(20, 20, 20),Scalar(20, 20, 20));
    //imshow("mask",mask_image);
    show_image(mask_image,1);



}

void scissor::on_actionDrawNodeVector_triggered()
{
    Is_showgrad = ~Is_showgrad;

}

void scissor::on_actionSave_Image_triggered()
{
    if(!Is_start)
        return;
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save File"),"../iamge.jpg","Image files (*.jpg)");
    std::string file = fileName.toUtf8().constData();
    cout<<"image "<<file<<endl;
    if(!file.empty())
    {
        imwrite( file, seed_image );
    }
}

void scissor::on_actionGet_Mask_triggered()
{
    Is_mask = 1;
}

void scissor::on_actionSave_Mask_triggered()

{
    if(!Is_mask)
        return;
    QString fileName=QFileDialog::getSaveFileName(this,tr("Save File"),"../mask_iamge.jpg","Image files (*.jpg)");
    std::string file = fileName.toUtf8().constData();
    cout<<"mask saved "<<file<<endl;
    imwrite( file, mask_image );



}

void scissor::on_actionExit_triggered()
{
    this->close();
}
