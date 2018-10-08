#ifndef OPTIONSETTER_H
#define OPTIONSETTER_H
#include <QWidget>
#include <QLineEdit>



class optionSetter:public QLineEdit {
    Q_OBJECT
public:
    explicit optionSetter(QWidget* parent);

    static void gui_setDefaultOpts();


public slots:

    void gui_setFontSize();
    void gui_setFont();
    void gui_setSpaceX();
    void gui_setSpaceY();

    void gui_setInputFile();
    void gui_setOutputFile();



    void gui_setAsciiUsed();
    void gui_setHiraganaUsed();
    void gui_setKatakanaUsed();


    void gui_setUseQuick();
    void gui_setUseAverageReduce();
    void gui_setAutoGenColorScale();
    void gui_setEdgeScoreWeight();
    void gui_setColorScoreWeight();
    void gui_setDistanceWeight();
    void gui_setSaturationScale();
    void gui_setLightnessScale();
    void gui_setHueScale();

    void gui_selectedInputFile(QString newFile);

signals:
    void input_file_changed(char* newFile);
};

#endif // OPTIONSETTER_H
