
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QPixmap>
#include <QImage>
#include <QPolygonF>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

public slots:

    void dataReceive(const QByteArray &data);

private:
    Ui::MainWindow *ui;
    QByteArray ImageBuf;
    QPixmap Imagepixmap;

    QList<QPixmap> ImageVedio;

    const int MaxFrame = 10000;
    int ImageWidth = 188;
    int ImageHeight = 120;

    QVector<QPolygonF> Lines;
    QImage::Format ImageFormt = QImage::Format::Format_Mono;

    QByteArray MonoDecode(const QByteArray &ImageData);
};

#endif // MAINWINDOW_H
