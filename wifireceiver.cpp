#include "wifireceiver.h"
#include "ui_wifireceiver.h"
#include <QColor>
WifiReceiver::WifiReceiver(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WifiReceiver)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());
    this->setWindowFlags(Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
    g_tcpServer = new(QTcpServer);
    g_tcpSocket = new(QTcpSocket);
    /*背景*/

    ui->WID_customPlot->setBackground(QColor(255, 255, 255));//曲线背景
    //新连接触发
    connect(g_tcpServer,SIGNAL(newConnection()),this,SLOT(newSocketConnect()));
    ui->statusBar->showMessage("未开启监听...");

    /*************曲线绘制区初始化**************/
    //添加曲线
    ui->WID_customPlot->addGraph();
    //设置画笔颜色
    ui->WID_customPlot->graph(0)->setPen(QPen(Qt::black)); // line color black
    //设置标题
    QCPTextElement *title = new QCPTextElement(ui->WID_customPlot);
    title->setText("时域波形曲线");
    title->setFont(QFont("sans",16,QFont::Bold));
    ui->WID_customPlot->plotLayout()->insertRow(0);
    ui->WID_customPlot->plotLayout()->addElement(0,0,title);
 //   ui->WID_customPlot->plotLayout()->insertRow(0);
  //  m_title = new QCPTextElement(ui->WID_customPlot, "时域波形曲线");
 //   ui->WID_customPlot->plotLayout()->addElement(0, 0, m_title);

    //设置坐标轴范围
    ui->WID_customPlot->xAxis->setRange(0,40);//0~40KHz
    ui->WID_customPlot->yAxis->setRange(0,200);

    //设置坐标轴标题
    QFont size_label = font();
    ui->WID_customPlot->xAxis->setLabel("时间t");

    ui->WID_customPlot->xAxis->setLabelFont(size_label);
    ui->WID_customPlot->yAxis->setLabel("强度");
    ui->WID_customPlot->yAxis->setLabelFont(size_label);
    ui->WID_customPlot->xAxis->setVisible(true);
    ui->WID_customPlot->xAxis->setTickLabels(true);
    ui->WID_customPlot->yAxis->setVisible(true);
    ui->WID_customPlot->yAxis->setTickLabels(true);

    //自适应坐标轴
   // ui->WID_customPlot->graph(0)->rescaleAxes();
    //可滚动变化
    //ui->WID_customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    //测试
 /*   for(double i = 0;i < 40.0;i++)
    {
        ui->WID_customPlot->graph(0)->addData(i,cos(i/(101*3.14)));
    }
    ui->WID_customPlot->replot();
    ui->WID_customPlot->graph(0)->rescaleAxes();*/
}

WifiReceiver::~WifiReceiver()
{
    delete g_tcpServer;
    delete g_tcpSocket;
    delete ui;
    this->close();
}
void WifiReceiver::readMeassage()
{
    QByteArray data = g_tcpSocket->readAll();


    //数据分割  S&a,k1,k2,k3,k4,k5,k6&x1,x2...xn|y1,y2....yn
    //S&a,k1,k2,k3,k4,k5,k6&x1,x2...xn|y1,y2....yn 为发送格式,必须按格式发送，否则程序将崩溃，其中S代表数据的帧头
    /*
    S = 0xEE  ---->图像绘制

    a = 0xEF  ---->控件数据显示

    k1         ---->控件1数据
    ...
    k6         ---->控件6数据


    */
    QList<QByteArray> dataTitle = data.split('&');

    QVector<QByteArray> title = dataTitle[0].split(',').toVector();

    QVector<QByteArray> title1 = dataTitle[1].split(',').toVector();

    qDebug()<<"title:"<<title;

    qDebug()<<"title1:"<<title1;
    //图像绘制
    if(title[0].toInt() == 0xEE){

        QList<QByteArray> dataList = dataTitle[2].split('|');

        qDebug()<<"dataList:"<<dataList;

        QVector<QByteArray> tempX = dataList[0].split(',').toVector();
        QVector<QByteArray> tempY = dataList[1].split(',').toVector();


        //转换为double
        QVector<double> posX,posY;
        for(int index = 0;index < tempX.size();index++)
        {
            posX.append(double(tempX[index].toDouble()));
        }
        for(int index = 0;index < tempY.size();index++)
        {
            posY.append(double(tempY[index].toDouble()));
        }

        qDebug()<<"posX:"<<posX;
        qDebug()<<"posY:"<<posY;

        /****************曲线绘制****************/
        ui->WID_customPlot->graph(0)->setData(posX,posY);
    //    ui->WID_customPlot->graph(0)->rescaleAxes();
        ui->WID_customPlot->replot();


     //控件数据
        if(title1[0].toInt() == 0xEF){

        ui->lineEdit->setText(title1[1]);

        ui->lineEdit_2->setText(title1[2]);

        ui->lineEdit_3->setText(title1[3]);

        ui->lineEdit_4->setText(title1[4]);

        ui->lineEdit_5->setText(title1[5]);

        ui->lineEdit_6->setText(title1[6]+"%");

                }
        }



    }









/*新的客户端接入*/
void WifiReceiver::newSocketConnect()
{
    //得到连入的socket
    g_tcpSocket = g_tcpServer->nextPendingConnection();
    //有可读的消息触发读函数
    connect(g_tcpSocket,SIGNAL(readyRead()),this,SLOT(readMeassage()));
    //对方解除连接
    (g_tcpSocket,SIGNAL(disconnected()),this,SLOT(on_BTN_disconnect_clicked()));
    QString tempString = "已连接："+g_tcpSocket->peerAddress().toString() + " "+QString::number(g_tcpSocket->peerPort());
    ui->statusBar->showMessage(tempString);
}

/*连接按钮*/
void WifiReceiver::on_BTN_connect_clicked()
{
    g_tcpIp = ui->ED_ip->text();
    g_tcpPort = ui->ED_port->text().toInt();
    if((g_tcpPort != 0))
    {
        //开始监听ip port
        if(g_tcpIp.isEmpty())//ip为空
        {
            if(!g_tcpServer->listen(QHostAddress::Any,g_tcpPort))
                return;
        }
        else
        {
            if(!g_tcpServer->listen(QHostAddress(g_tcpIp),g_tcpPort))
                return;
        }
        ui->BTN_connect->setEnabled(false);
        ui->BTN_disconnect->setEnabled(true);
        ui->ED_ip->setEnabled(false);
        ui->ED_port->setEnabled(false);

        /*  底部提示 */
        QString tempString = "正在监听：" + g_tcpServer->serverAddress().toString() + " 端口：" + QString::number(g_tcpServer->serverPort());
        ui->statusBar->showMessage(tempString);
    }
}

/*断开按钮*/
void WifiReceiver::on_BTN_disconnect_clicked()
{
    g_tcpSocket->disconnectFromHost();
    g_tcpServer->close();
    ui->BTN_connect->setEnabled(true);//使能'连接'按钮
    ui->BTN_disconnect->setEnabled(false);//失能'断开'按钮
    ui->ED_ip->setEnabled(true);//使能ip编辑框
    ui->ED_port->setEnabled(true);//使能port编辑框
    ui->statusBar->showMessage("未开启监听......");
}

/*保存曲线图片*/
bool WifiReceiver::on_saveButton_clicked()
{
    QString filename = QFileDialog::getSaveFileName();
    if(filename =="")//文件名为空
    {
        QMessageBox::information(this,"fail","保存失败！");
        return false;
    }
    if(filename.endsWith(".png"))
    {
        QMessageBox::information(this,"success","保存成功！");
        return ui->WID_customPlot->savePng(filename,ui->WID_customPlot->width(),ui->WID_customPlot->height());
    }
    if( filename.endsWith(".jpg")||filename.endsWith(".jpeg") ){
        QMessageBox::information(this,"success","成功保存为jpg文件");
        return ui->WID_customPlot->saveJpg(filename, ui->WID_customPlot->width(), ui->WID_customPlot->height() );

    }
    if( filename.endsWith(".bmp") ){
        QMessageBox::information(this,"success","成功保存为bmp文件");
        return ui->WID_customPlot->saveBmp( filename, ui->WID_customPlot->width(), ui->WID_customPlot->height() );

    }
    if( filename.endsWith(".pdf") ){
        QMessageBox::information(this,"success","成功保存为pdf文件");
        return ui->WID_customPlot->savePdf( filename, ui->WID_customPlot->width(), ui->WID_customPlot->height() );
  }
   else{
    //否则追加后缀名为.png保存文件
        QMessageBox::information(this,"success","保存成功,已默认保存为png文件");
    return ui->WID_customPlot->savePng(filename.append(".png"), ui->WID_customPlot->width(), ui->WID_customPlot->height() );

   }
}


