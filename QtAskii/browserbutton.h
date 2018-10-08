#ifndef BROWSERBUTTON_H
#define BROWSERBUTTON_H

#include <QObject>
#include <QPushButton>
class browserButton : public QPushButton
{
    Q_OBJECT
public:
    browserButton();
    browserButton(QWidget*);
public slots:
    void browseFiles();
signals:
    void fileSelected(QString selectedFile);
};

#endif // BROWSERBUTTON_H
