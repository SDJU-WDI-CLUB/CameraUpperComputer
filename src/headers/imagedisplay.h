#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QWheelEvent>

namespace Ui {
class ImageDisplay;
}

class ImageDisplay : public QWidget
{
    Q_OBJECT

public slots:
    void SetUpImageWidth(size_t width);
    void SetUpImageHeight(size_t height);


public:
    explicit ImageDisplay(QWidget *parent = nullptr);
    ~ImageDisplay();
    void DisPlayImage(QPixmap & Image);

private:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QPointF GetLoct(QPointF pos);

    void DrawPixel(int x,int y, QColor color);

    void PaindGrid();
    Ui::ImageDisplay *ui;

    size_t ImageWidth = 188;
    size_t ImageHeight = 120;
    float widthPerPix = 4;

    float Zoom = 1;
    QPointF StartPoint;

    bool LeftMousePressed = false;
    QPointF MousePrePos;
    QPointF NowMouseImagePos;
    QPixmap displayImage;



};

#endif // IMAGEDISPLAY_H
