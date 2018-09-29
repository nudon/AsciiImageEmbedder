#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
#include <QLabel>

class imageViewer : public QLabel
{
    Q_OBJECT
public:
    imageViewer();
    imageViewer(QWidget*);

public slots:
    void showNewImage(char* filename);
};

#endif // IMAGEVIEWER_H
