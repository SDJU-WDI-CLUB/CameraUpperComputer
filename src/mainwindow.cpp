#include "mainwindow.h"
#include "forms/ui_mainwindow.h"
#include "iostream"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->UdpServers, &UdpServer::Receivepackage, this, &MainWindow::dataReceive);

    connect(ui->UdpServers, &UdpServer::ServerStart, [&]() {
        ui->ImageHeightEdit->setEnabled(false);
        ui->ImageWidthEdit->setEnabled(false);
        ui->ImageType->setEnabled(false);
        ui->LineNumberEdit->setEnabled(false);
    });

    connect(ui->UdpServers, &UdpServer::ServerEnd, [&]() {
        ui->ImageHeightEdit->setEnabled(true);
        ui->ImageWidthEdit->setEnabled(true);
        ui->ImageType->setEnabled(true);
        ui->LineNumberEdit->setEnabled(true);
    });

    connect(ui->ImageHeightEdit, &QLineEdit::editingFinished, [&]() {
        ImageHeight = ui->ImageHeightEdit->text().toInt();
        ui->Image->SetUpImageHeight(ImageHeight);
    });
    connect(ui->ImageWidthEdit, &QLineEdit::editingFinished, [&]() {
        ImageWidth = ui->ImageWidthEdit->text().toInt();
        ui->Image->SetUpImageWidth(ImageWidth);
    });
    connect(ui->LineNumberEdit, &QLineEdit::editingFinished, [&]() {
        Lines.resize(ui->LineNumberEdit->text().toInt());
    });

    connect(ui->clear, &QPushButton::pressed, [&]() {
        this->ImageVedio.clear();
        ui->frame->setRange(0, 0);
    });

    connect(ui->saveImage, &QPushButton::pressed, [&](){
        int target = ui->frame->value();
        ImageVedio[target].save("image.bmp", "BMP");
    });

    connect(ui->ImageType, &QComboBox::currentIndexChanged, [&](int index) {
        switch (index) {
            case 0:
                ImageFormt = QImage::Format_Mono;
                break;
            case 1:
                ImageFormt = QImage::Format_Indexed8;
                break;
            case 2:
                ImageFormt = QImage::Format_RGB888;
                break;
            default:
                break;
        }
    });

    connect(ui->frame, &QSlider::valueChanged, [&](int val) {
        if (!ImageVedio.empty()) {
            ui->Image->DisPlayImage(ImageVedio[val]);
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}


void MainWindow::dataReceive(const QByteArray& data) {
    QByteArray startMarker = QByteArray::fromHex("48414C46");
    QByteArray endMarker = QByteArray::fromHex("464C4148");

    ImageBuf.push_back(data);

    int nStartIndex = ImageBuf.indexOf(startMarker);
    int nEndIndex = ImageBuf.indexOf(endMarker, nStartIndex + startMarker.size());

    if (nStartIndex != -1 && nEndIndex != -1) {
        //   int ImageWidth = ui-
        QByteArray RawData = ImageBuf.mid(nStartIndex + 4, nEndIndex - nStartIndex - 4);
        ImageBuf.remove(0, nEndIndex + 4);
        QImage::Format RealFormat = QImage::Format_Indexed8;
        int len = ImageWidth * ImageHeight;
        if (ImageFormt == QImage::Format_Mono) {
            len /= 8;
        }
        if (ImageFormt == QImage::Format_RGB888) {
            RealFormat = QImage::Format_RGB888;
            len *= 3;
        }

//        std::cout << len << std::endl;
        QByteArray ImageData = RawData.mid(0, len);
        if (ImageFormt == QImage::Format_Mono) ImageData = MonoDecode(ImageData);

        RawData.remove(0, len);
        QImage Image = QImage((uint8_t *) ImageData.data(), ImageWidth, ImageHeight, RealFormat);
        Image = Image.convertToFormat(QImage::Format_RGB888);

        auto *ImagePaint = new QPainter(&Image);
        Qt::GlobalColor pGlobalColor[3] = {Qt::red, Qt::green, Qt::blue};
        for (int i = 0; i < Lines.count(); ++i) {
            Lines[i].clear();
            QByteArray LineData = RawData.mid(0, ImageHeight);
            RawData.remove(0, ImageHeight);
            for (int y = 0; y < ImageHeight; y++) {
                Lines[i] << QPointF((int) LineData[y], y);
            }
        }
        for (int i = 0; i < Lines.count(); ++i) {
            ImagePaint->setPen(QPen(pGlobalColor[i], 1));
            ImagePaint->drawPoints(Lines[i]);
        }

        delete ImagePaint;
        ImageVedio.push_back(QPixmap::fromImage(Image));

        if (ImageVedio.size() > this->MaxFrame) {
            ImageVedio.pop_front();
        }
        ui->frame->setMaximum(ImageVedio.size() - 1);
        ui->frame->setValue(ImageVedio.size() - 1);
    }
}

QByteArray MainWindow::MonoDecode(const QByteArray& ImageData) {
    QByteArray res;
    for (uint8_t it: ImageData) {
        for (int i = 0x80; i >= 0x01; i >>= 1) {
            if (it & i) {
                res.push_back((char) 255);

            } else {
                res.push_back((char) 0);
            }
        }
    }
    return res;
}
