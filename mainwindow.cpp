#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QLabel>

//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


//gemini-generated QT->OpenCV image types
cv::Mat QImage2CvMat(const QImage& image) {
    cv::Mat mat;
    switch (image.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
        // Convert from ARGB to BGR if needed
        cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_RGB2BGR);
        break;
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
        break;
    default:
        // Handle other formats if needed
        return cv::Mat();
    }
    return mat;
}

//gemini-generated OpenCV->QT image types
QImage CvMat2Qimage(const cv::Mat& mat) {
    if (mat.empty()) {
        return QImage();
    }

    cv::Mat temp;
    cv::cvtColor(mat, temp, cv::COLOR_BGR2RGB);
    QImage image(temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
    return image.copy();
}


// check if an integer is odd
bool isOdd_modulo(int number) {
    return number % 2 != 0;
}


//applies the blur to the second pane
//useful function to call whenever the kernel slider is changed
void MainWindow::updateBlurredImage(){

    cv::Mat blurred_image;
    cv::Mat x1 = QImage2CvMat(image.toImage());
    if (x1.empty()) {
        qDebug() << "cv2 conversion failed";

    } else {
        // Kernel size for the Gaussian blur (must be odd)
        int ksize = ui->horizontalScrollBar->value();
        if(isOdd_modulo(ksize)){
            //apply the blur and update the window
            cv::GaussianBlur(x1, blurred_image, Size(ksize, ksize), 0);
            QPixmap x2 = QPixmap::fromImage(CvMat2Qimage(blurred_image));
            ui->imageLabel_2->setPixmap(x2);
            ui->imageLabel_2->setScaledContents( true );
            ui->imageLabel_2->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
            ui->kernelLabel->setText( QString("%1").arg(ksize));
        }
    }
}

void MainWindow::on_filePickerButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        nullptr,
        "Open File",
        "",
        "All Files (*.*)"
        );

    //load the image and display it in the left pane
    if (!fileName.isEmpty()) {
        qDebug() << "Selected file:" << fileName;
        image.load(fileName);
        if (image.isNull()) {
            qDebug() << "image is null";
        } else{
            ui->imageLabel->setPixmap(image);
            ui->imageLabel->setScaledContents( true );
            ui->imageLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
            updateBlurredImage();
        }
    } else {
        qDebug() << "No file selected";
    }
}



void MainWindow::on_horizontalScrollBar_valueChanged(int value)
{
    updateBlurredImage();
}

