#include "imagebuilder.h"

imageBuilder::imageBuilder()
{

}

imageBuilder::imageBuilder(QWidget*) {

}

void imageBuilder::buildAsciiImage() {
    //generate ascii art, maybe emit signals for imageviewers to update once finished
    //will probably have to create a subclass of image viewers which takes a signal w/string as arg
    //and display that file
    ImageGenerator::generateAscii();
    emit imageBuilder::buildComplete(getOutputFile());
    //double a = 5 / 0;

}
