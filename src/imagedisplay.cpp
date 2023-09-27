#include "imagedisplay.h"
#include "forms/ui_imagedisplay.h"

ImageDisplay::ImageDisplay(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::ImageDisplay) {
    ui->setupUi(this);
    StartPoint.setX(5);
    StartPoint.setY(5);
}

ImageDisplay::~ImageDisplay() {
    delete ui;
}

void ImageDisplay::resizeEvent(QResizeEvent *event) {
    this->repaint();
}

void ImageDisplay::SetUpImageWidth(size_t width) {
    this->ImageWidth = width;
    this->repaint();
}

void ImageDisplay::SetUpImageHeight(size_t height) {
    this->ImageHeight = height;
    this->repaint();
}

void ImageDisplay::DisPlayImage(QPixmap &Image) {
    if (Image.width() != this->ImageWidth || Image.height() != this->ImageHeight) {
        return;
    }
    displayImage = Image;
    this->repaint();
}

void ImageDisplay::PaindGrid() {
    QPainter painter(this);

    painter.setPen(QPen(Qt::black, 1));
    QList<QLineF> lines;

    for (float x = StartPoint.x(); x <= StartPoint.x() + widthPerPix * this->ImageWidth; x += widthPerPix) {
        lines.append(QLineF(x, StartPoint.y(), x, StartPoint.y() + widthPerPix * this->ImageHeight));
    }
    for (float y = StartPoint.y(); y <= StartPoint.y() + widthPerPix * this->ImageHeight; y += widthPerPix) {
        lines.append(QLineF(StartPoint.x(), y, StartPoint.x() + widthPerPix * this->ImageWidth, y));
    }
    painter.drawLines(lines);
}

void ImageDisplay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(StartPoint.x(), StartPoint.y(), ImageWidth * widthPerPix, ImageHeight * widthPerPix,
                       displayImage);

    DrawPixel(NowMouseImagePos.x(), NowMouseImagePos.y(), QColor(255, 174, 201));
    this->PaindGrid();
}

void ImageDisplay::wheelEvent(QWheelEvent *event) {
    event->angleDelta();
    if (event->angleDelta().y() > 0) {
        Zoom += 0.1;
        widthPerPix = 4 * Zoom;
    } else {
        Zoom -= 0.1;

        if (Zoom < 1.0) {
            Zoom = 1.0;
        }
        widthPerPix = 4 * Zoom;
    }
    this->repaint();
}


void ImageDisplay::mouseMoveEvent(QMouseEvent *event) {
    auto nowpos = event->position();

    auto tmp = this->GetLoct(nowpos);

    if (tmp != NowMouseImagePos) {
        NowMouseImagePos = tmp;

        this->ui->cood->setText(QString().asprintf("图像坐标:(%d,%d)",
                                                   int(NowMouseImagePos.x()),
                                                   int(NowMouseImagePos.y())
        ));
        this->repaint();
    }

    if (LeftMousePressed) {
        StartPoint += nowpos - MousePrePos;
        this->repaint();
        MousePrePos = nowpos;
    }
}

void ImageDisplay::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        LeftMousePressed = true;
        MousePrePos = event->position();
    }
}

void ImageDisplay::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        LeftMousePressed = false;
    }
}

void ImageDisplay::DrawPixel(int x, int y, QColor color) {
    if (x < 0 || y < 0 || x >= ImageWidth || y >= ImageHeight)return;
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(color);

    QRectF PixelArea(StartPoint.x() + x * widthPerPix, StartPoint.y() + y * widthPerPix, widthPerPix, widthPerPix);
    painter.drawRect(PixelArea);
}

QPointF ImageDisplay::GetLoct(QPointF pos) {
    auto tmp = pos - StartPoint;

    return tmp / widthPerPix;

}
