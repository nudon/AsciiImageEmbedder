#include "browserbutton.h"
#include "imagegenerator.h"
#include <QFileDialog>
browserButton::browserButton()
{

}

browserButton::browserButton(QWidget*)
{

}

void browserButton::browseFiles(){
    static QString openedDir = "./";
    QString selectedFile = QFileDialog::getOpenFileName(this,
          tr("Select Input Image"), openedDir, tr("Image Files (*.png *.jpg)"));
    openedDir.chop(openedDir.size());
    openedDir.append(selectedFile);
    ImageGenerator::setRebuildBasePicture(true);
    emit browserButton::fileSelected(selectedFile);

}
