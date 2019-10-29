#ifndef QTRANSWINDOW_H
#define QTRANSWINDOW_H

#include "SurveyMath/SurveyMath.h"
#include "SurveyMath/BasePoint.h"
#include "SurveyMath/CoordinateTransformation.h"
#include "SurveyMath/Ellipsoid.h"
#include "SurveyMath/Projection.h"
#include "SurveyMath/SevenParameter.h"
#include "SurveyPoint.h"
#include "ui_dialogchangezone.h"
#include <QValidator>
#include <QFileInfo>
#include <QDialog>
using namespace SurveyMath;
namespace Ui {
class DialogChangeZone;
}

class DialogChangeZone : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChangeZone(QWidget *parent = nullptr);
    ~DialogChangeZone();

private slots:


    void on_pushButton_ChangeZone_Point_clicked();

    void on_pushButton_ChangeZone_SelectFile_clicked();

    void on_pushButton_ChangeZone_File_clicked();

private:
    BasePoint ChangeZone(BasePoint inputPoint);
    Ui::DialogChangeZone *ui;
    Projection prj_bj54;
    Projection prj_xian80;
    Projection prj_cscg2000;
    Projection prj_wgs84;
    Projection prj_utm;
    Projection sourcePrjoject;
    Projection targetPrjoject;
    SevenParameter sevenParameter;
    QString ProjectName[5];
    QString CoordinateType[2];
    QRegExpValidator *pRevZone;//������ʽ����ƥ��
    QRegExpValidator *pRevLatitude;//������ʽγ��ƥ��
    QRegExpValidator *pRevLongitude;//������ʽ����ƥ��
    QRegExpValidator *pRevDecimal;//������ʽС��ƥ��
    QString UserFilePath;

};

#endif // QTRANSWINDOW_H
