#ifndef IMAGEBUILDER_H
#define IMAGEBUILDER_H

#include <QObject>
#include <QPushButton>
#include "imagegenerator.h"

class imageBuilder : public QPushButton {
    Q_OBJECT
public:
    imageBuilder();
    imageBuilder(QWidget*);

public slots:
    void buildAsciiImage();

signals:
    void buildComplete(char* file);
};

#endif // IMAGEBUILDER_H
