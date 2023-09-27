#include "mainwindow.h"
#include "forms/ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->UdpServers,&UdpServer::Receivepackage,this,&MainWindow::dataReceive);

    connect(ui->UdpServers,&UdpServer::ServerStart,[&](){
        ui->ImageHeightEdit->setEnabled(false);
        ui->ImageWidthEdit->setEnabled(false);
        ui->ImageType->setEnabled(false);
        ui->LineNumberEdit->setEnabled(false);
    });

    connect(ui->UdpServers,&UdpServer::ServerEnd,[&](){
        ui->ImageHeightEdit->setEnabled(true);
        ui->ImageWidthEdit->setEnabled(true);
        ui->ImageType->setEnabled(true);
        ui->LineNumberEdit->setEnabled(true);
    });

    connect(ui->ImageHeightEdit,&QLineEdit::editingFinished,[&](){
        ImageHeight =ui->ImageHeightEdit->text().toInt();
        ui->Image->SetUpImageHeight(ImageHeight);
    });
    connect(ui->ImageWidthEdit,&QLineEdit::editingFinished,[&](){
        ImageWidth =ui->ImageWidthEdit->text().toInt();
        ui->Image->SetUpImageWidth(ImageWidth);
    });
    connect(ui->LineNumberEdit,&QLineEdit::editingFinished,[&](){
        Lines.resize(ui->LineNumberEdit->text().toInt());
    });

    connect(ui->clear,&QPushButton::pressed,[&](){
        this->ImageVedio.clear();
        ui->frame->setRange(0,0);
    });
    connect(ui->ImageType,&QComboBox::currentIndexChanged,[&](int index){
        switch (index) {
        case 0:
            ImageFormt =QImage::Format_Mono;
            break;
        case 1:
            ImageFormt =QImage::Format_Indexed8;
            break;
        case 2:
            ImageFormt =QImage::Format_RGB888;
            break;
        default:
            break;
        }
    });

    connect(ui->frame,&QSlider::valueChanged,[&](int val){
        if(ImageVedio.size())
            ui->Image->DisPlayImage(ImageVedio[val]);
    });



}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::dataReceive(QByteArray data)
{
    QByteArray startMarker = QByteArray::fromHex("0000807F");
    QByteArray endMarker = QByteArray::fromHex("807F0000");

    ImageBuf.push_back(data);

    int nStartIndex = ImageBuf.indexOf(startMarker);
    int nEndIndex = ImageBuf.indexOf(endMarker, nStartIndex + startMarker.size());

    if ( nStartIndex != -1 && nEndIndex != -1) {
        //   int ImageWidth = ui-
        QByteArray RawData = ImageBuf.mid(nStartIndex+4, nEndIndex-nStartIndex-4 );
        ImageBuf.remove(0,nEndIndex+4);
        QImage::Format RealFormat = QImage::Format_Indexed8;
        int imagesize = ImageWidth*ImageHeight;
        if(ImageFormt == QImage::Format_Mono)
        {
            imagesize /= 8;
        }
        if(ImageFormt == QImage::Format_RGB888)
        {
            RealFormat = QImage::Format_RGB888;
            imagesize *=3;
        }

        QByteArray ImageData = RawData.mid(0,imagesize);
        if(ImageFormt == QImage::Format_Mono) ImageData = MonoDecode(ImageData);

        RawData.remove(0,imagesize);
        QImage Image = QImage((uint8_t *)ImageData.data(),ImageWidth,ImageHeight,RealFormat);
        Image = Image.convertToFormat(QImage::Format_RGB888);

        QPainter *ImagePaint = new QPainter(&Image);
        Qt::GlobalColor lincolor[3] = {Qt::red,Qt::green,Qt::blue};
        for (int i = 0; i < Lines.count(); ++i) {
            Lines[i].clear();
            QByteArray LineData = RawData.mid(0,ImageHeight);
            RawData.remove(0,ImageHeight);
            for(int y = 0 ; y <  ImageHeight;y++)
            {
                Lines[i]<<QPointF((int)LineData[y],y);
            }
        }
        for (int i = 0; i < Lines.count(); ++i) {
            ImagePaint->setPen(QPen(lincolor[i],1));
            ImagePaint->drawPoints(Lines[i]);
        }


        ImageVedio.push_back(QPixmap::fromImage(Image));

        if(ImageVedio.size() > this->MaxFrame)
        {
            ImageVedio.pop_front();
        }
        ui->frame->setMaximum(ImageVedio.size()-1);
        ui->frame->setValue(ImageVedio.size()-1);

    }
}

QByteArray MainWindow::MonoDecode(QByteArray ImageData)
{
    QByteArray res;
    for(uint8_t it :ImageData)
    {
        for(int i = 0x80; i >=0x01;i>>=1)
        {
            if(it&i)
            {
                res.push_back((char)255);

            }else
            {
                res.push_back((char)0);
            }
        }
    }
    return res;
}