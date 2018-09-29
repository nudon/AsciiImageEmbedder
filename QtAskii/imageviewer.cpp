#include "imageviewer.h"

imageViewer::imageViewer()
{

}

imageViewer::imageViewer(QWidget*) {

}

void imageViewer::showNewImage(char* filename) {
    this->setVisible(false);
    const QString qFilename(filename);
    const QPixmap* oldPic = this->pixmap();
    const QPixmap* newPic = new QPixmap(qFilename);

    int w = this->width() - this->frameWidth() * 2;
    int h = this->height() - this->frameWidth() * 2;
    //ui->label->setPixmap (pix.scaled (w,h,Qt::KeepAspectRatio));
    const QPixmap newScaled = newPic->scaled(w,h,Qt::KeepAspectRatio);

    //delete newPic;
    this->setPixmap(newScaled);
    if (oldPic) {
        //delete oldPic;
    }
    delete newPic;
    //this->setPixmap(newPic->scaled(w,h,Qt::KeepAspectRatio));
    this->setVisible(true);
}
