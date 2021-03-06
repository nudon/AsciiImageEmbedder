#include "optionsetter.h"
#include "imagegenerator.h"

#include <QFileDialog>
extern "C" {
void setFontSize(int);

void setSpaceX(int);

void setSpaceY(int);

void setInputFile(char* file);

void setOutputFile(char* file);

void mySetFont(char* newFont);

char* myGetFont();


void setAsciiUsed(int);
void setHiraganaUsed(int);
void setKatakanaUsed(int);


void setUseQuick(int);
void setUseAverageReduce(int);
void setAutoGenColorScale(int);
void setEdgeScoreWeight(float);
void setColorScoreWeight(float);
void setDistanceWeight(float);
void setSaturationScale(float);
void setLightnessScale(float);
void setHueScale(float);


void setDefaultOpts();
}

char* QStringToCharp(QString q) {
    char* newChar;
    newChar = q.toLocal8Bit().data();
    return newChar;
}

int QStringToInt(QString q) {
    char* text = QStringToCharp(q);
    int val = std::stoi(text);
    //free(text);
    //delete(text);
    return val;
}

double QStringToDouble(QString q) {
    char* text = QStringToCharp(q);
    double val = std::stod(text);
    //free(text);
    //delete(text);
    return val;
}

char* pathWarper(char* path) {
    const char* constPrependPath = "/../../";
    char* prependPath = strdup(constPrependPath);
    int prependLen = strlen(prependPath);
    int fontLen = strlen(path);
    int totLen = prependLen + fontLen;
    char* correctPath = (char*)malloc(sizeof(char) * (totLen+ 1));
    memcpy(correctPath, prependPath, prependLen);
    strcat(correctPath, path);
    return correctPath;
}

void optionSetter::gui_setDefaultOpts() {
    //might be able to make guiSetOptions static
    //could replace all these with gui_* varianats
    //that way no way for initialized QlineEdit fields and actual set options to diverge
    //no cause then I can't access QlineEdit text, dummy
    const char* defOut = "output.png";
    const char* defFont = "DejaVuSansMono.ttf";
    char* charOut = strdup(defOut);
    char* charFont = strdup(defFont);
    char* correctPath;
    setDefaultOpts();
    /*setFontSize(10);
    setSpaceX(0); // 1 on my terminal
    setSpaceY(0); // 3 on my terminal
    setUseQuick(0);
    setUseAverageReduce(0);
    setAsciiUsed(1);
    setHiraganaUsed(0);
    setKatakanaUsed(0);
    setEdgeScoreWeight(1);
    setColorScoreWeight(1);
    setSaturationScale(1);
    setLightnessScale(1);
    setHueScale(0);
    setDistanceWeight(1);
    //strcpy(outputFileName, "output.jpg");
    setOutputFile(charOut);
    mySetFont(charFont);
    */
    setOutputFile(charOut);
    mySetFont(charFont);
    free(charOut);
    free(charFont);
}

optionSetter::optionSetter(QWidget* parent) {
    this->setParent(parent);
}

void optionSetter::gui_setInputFile() {
    ImageGenerator::setRebuildBasePicture(true);
    QString textVal = QLineEdit::text();
    char* charVal = QStringToCharp(textVal);
    setInputFile(charVal);
    emit optionSetter::input_file_changed(charVal);
}

void optionSetter::gui_setOutputFile() {
    QString textVal = QLineEdit::text();
    char* charVal = QStringToCharp(textVal);
    setOutputFile(charVal);
}

void optionSetter::gui_setFont() {
    ImageGenerator::setRebuildBasePicture(true);
    ImageGenerator::setRebuildCharacterSet(true);
    QString textVal = QLineEdit::text();
    char* charVal = QStringToCharp(textVal);
    mySetFont(charVal);
}

void optionSetter::gui_setFontSize() {
    ImageGenerator::setRebuildBasePicture(true);
    ImageGenerator::setRebuildCharacterSet(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setFontSize(intVal);
}

void optionSetter::gui_setSpaceX() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setSpaceX(intVal);
}
void optionSetter::gui_setSpaceY() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setSpaceY(intVal);
}


void optionSetter::gui_setAsciiUsed() {
    ImageGenerator::setRebuildCharacterSet(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setAsciiUsed(intVal);
}
void optionSetter::gui_setHiraganaUsed() {
    ImageGenerator::setRebuildCharacterSet(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setHiraganaUsed(intVal);
}
void optionSetter::gui_setKatakanaUsed() {
    ImageGenerator::setRebuildCharacterSet(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setKatakanaUsed(intVal);
}


void optionSetter::gui_setUseQuick() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setUseQuick(intVal);
}
void optionSetter::gui_setUseAverageReduce() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setUseAverageReduce(intVal);
}
void optionSetter::gui_setAutoGenColorScale() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    int intVal = QStringToInt(textVal);
    setAutoGenColorScale(intVal);
}
void optionSetter::gui_setEdgeScoreWeight(){
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    float numVal = QStringToDouble(textVal);
    setEdgeScoreWeight(numVal);
    }
void optionSetter::gui_setColorScoreWeight() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    float numVal = QStringToDouble(textVal);
    setColorScoreWeight(numVal);
}

void optionSetter::gui_setSaturationScale() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    float numVal = QStringToDouble(textVal);
    setSaturationScale(numVal);
}
void optionSetter::gui_setLightnessScale() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    float numVal = QStringToDouble(textVal);
    setLightnessScale(numVal);
}
void optionSetter::gui_setHueScale() {
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    float numVal = QStringToDouble(textVal);
    setHueScale(numVal);
}

void optionSetter::gui_setDistanceWeight(){
    ImageGenerator::setRebuildTiledPicture(true);
    QString textVal = QLineEdit::text();
    float numVal = QStringToDouble(textVal);
    setDistanceWeight(numVal);
}


void optionSetter::gui_selectedInputFile(QString newFile) {
    this->setText(newFile);
    char* charVal = QStringToCharp(newFile);
    printf("new file is %s\n", charVal);
    setInputFile(charVal);
    emit optionSetter::input_file_changed(charVal);
}

