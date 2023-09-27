
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

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void dataReceive(QByteArray data);
private:
    Ui::MainWindow *ui;
    QByteArray ImageBuf;
    QPixmap Imagepixmap;

    QList<QPixmap> ImageVedio;

    const int MaxFrame= 10000;


    QVector<QPolygonF> Lines;
    QImage::Format ImageFormt =  QImage::Format::Format_Mono;
    int ImageWidth =188;
    int ImageHeight =120;


    QByteArray MonoDecode( QByteArray ImageData );



};

#endif // MAINWINDOW_H
