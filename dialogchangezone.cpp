#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "ui_dialogchangezone.h"
#include "dialogchangezone.h"
DialogChangeZone::DialogChangeZone(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangeZone)
{
    ui->setupUi(this);
    Ellipsoid ell_wgs84(6378137.0, 1/298.257223563);
    Ellipsoid ell_bj54(6378245.0, 1/298.3);
    Ellipsoid ell_xian80(6378140.0, 1/298.257);
    Ellipsoid ell_cscg2000(6378137.0, 1/298.257222101);
    prj_bj54.SetDatum(ell_bj54);
    prj_utm.SetDatum(ell_wgs84);
    prj_xian80.SetDatum(ell_xian80);
    prj_cscg2000.SetDatum(ell_cscg2000);


    ProjectName[0]=QString::fromLocal8Bit("����2000");
    ProjectName[1]=QString::fromLocal8Bit("����54");
    ProjectName[2]=QString::fromLocal8Bit("����80");
    ProjectName[3]=QString::fromLocal8Bit("WGS84");
    ProjectName[4]=QString::fromLocal8Bit("UTM");

    CoordinateType[0]=QString::fromLocal8Bit("�������");
    CoordinateType[1]=QString::fromLocal8Bit("ƽ������");


    pRevZone = new  QRegExpValidator(QRegExp("^(([1-9]|[1-5][0-9]|(60)))([sSnN]?)$")) ;//������ʽ����ƥ��1N(S)-60N(S)
    pRevLatitude = new  QRegExpValidator(QRegExp("^-?(90|[1-8]?\\d(\\.\\d{1,10})?)$")) ;//������ʽγ��ƥ��
    pRevLongitude = new  QRegExpValidator(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d{1,10})?)$")) ;//������ʽ����ƥ��
    pRevDecimal = new  QRegExpValidator(QRegExp("^([0-9]{1,}[.][0-9]*)$")) ;//������ʽС��ƥ��
    ui->lineEdit_ChangeZone_BeforeMiddleLon->setValidator(pRevLongitude);
    ui->lineEdit_ChangeZone_AfterMiddleLon->setValidator(pRevLongitude);
    ui->lineEdit_ChangeZone_BeforeX->setValidator(pRevDecimal);
    ui->lineEdit_ChangeZone_BeforeY->setValidator(pRevDecimal);
    ui->lineEdit_ChangeZone_AfterX->setValidator(pRevDecimal);
    ui->lineEdit_ChangeZone_AfterY->setValidator(pRevDecimal);
     UserFilePath = ".";
}

DialogChangeZone::~DialogChangeZone()
{
    delete ui;
}

void DialogChangeZone::on_pushButton_ChangeZone_Point_clicked()
{
    BasePoint sourcePoint;
    BasePoint targetPoint;
    int iDecimalsXY=4;
    sourcePoint.x =ui->lineEdit_ChangeZone_BeforeX->text().toDouble();
    sourcePoint.y =ui->lineEdit_ChangeZone_BeforeY->text().toDouble();
    sourcePoint.z =100;
    targetPoint = ChangeZone(sourcePoint);
    ui->lineEdit_ChangeZone_AfterX->setText(QString::number(targetPoint.x,4,iDecimalsXY));
    ui->lineEdit_ChangeZone_AfterY->setText(QString::number(targetPoint.y,4,iDecimalsXY));
}
BasePoint DialogChangeZone::ChangeZone(BasePoint inputPoint)
{
    Projection currentProject;

    if(ui->comboBox_ChangeZone_Project->currentIndex() == 0)//����2000
    {
        currentProject = prj_cscg2000;
    }
    if(ui->comboBox_ChangeZone_Project->currentIndex() == 1)//����54
    {
         currentProject = prj_bj54;
    }
    if(ui->comboBox_ChangeZone_Project->currentIndex() == 2)//����80
    {
         currentProject = prj_xian80;
    }

    double beforeMiddleLon = DMSToRadian(ui->lineEdit_ChangeZone_BeforeMiddleLon->text().toDouble());
    double afterMiddleLon = DMSToRadian(ui->lineEdit_ChangeZone_AfterMiddleLon->text().toDouble());
    double falseEast = ui->lineEdit_ChangeZone_FalseEast->text().toDouble();
    double falseNorth = ui->lineEdit_ChangeZone_FalseNorth->text().toDouble();

    currentProject.SetFalseEasting(falseEast);
    currentProject.SetFalseNorthing(falseNorth);

    //����ǰ����������
    currentProject.SetCentralMeridian(beforeMiddleLon);
    //����������ת��Ϊ�������,��˹���귴��
    BasePoint tempPoint = CoordinateTransformation::InverseGauss(currentProject,inputPoint);
    //����������������
    currentProject.SetCentralMeridian(afterMiddleLon);
    //�ı����������ߣ����������ת��Ϊ��������,��˹��������
    BasePoint outputPoint = CoordinateTransformation::PositiveGauss(currentProject,tempPoint);
    return outputPoint;
}

void DialogChangeZone::on_pushButton_ChangeZone_SelectFile_clicked()
{
    QString csvFileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����ת�����ļ�"), ".", QString::fromLocal8Bit("���ŷָ����ļ�(*.csv)"), 0);
    if (!csvFileName.isEmpty())
    {
       ui->lineEdit_ChangeZone_FilePath->setText(csvFileName);
       QFileInfo fileInfo (csvFileName);
       UserFilePath = fileInfo.filePath().left( fileInfo.filePath().size() -4) + "Z" ;
    }
    else
    {
       return;
    }
}

void DialogChangeZone::on_pushButton_ChangeZone_File_clicked()
{
    QFile sourceFile(ui->lineEdit_ChangeZone_FilePath->text());
    QTextStream inSourceFile(&sourceFile);
    QVector<SurveyPoint> inputPointVector;
    if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       return;
    }
    //��ȡԴ����
    while(!inSourceFile.atEnd())
    {
        QStringList lineList;
        QString strLine = inSourceFile.readLine();
        lineList = strLine.split(",", QString::SkipEmptyParts);
        if(lineList.size() < 3)
        {
            continue;
        }
        SurveyPoint tP;
        tP.PointName = lineList.at(0);
        tP.x=lineList.at(1).toDouble();
        tP.y=lineList.at(2).toDouble();
        if(lineList.size() == 4)
        {
             tP.z=lineList.at(3).toDouble();
        }
        else
        {
            tP.z = 0;
        }
        inputPointVector.push_back(tP);

    }
    QString csvfileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("�������ļ���"), "/", QString::fromLocal8Bit("csv�ļ�(*.csv)"),0);


    QFile csvFile(csvfileName);
    if(!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtOutput(&csvFile);
    for(int i=0;i<inputPointVector.size();i++)
    {
        BasePoint targetPoint = ChangeZone(inputPointVector[i]);
        txtOutput<<inputPointVector[i].PointName
                 <<qSetRealNumberPrecision(3)<<fixed
                 <<","<<targetPoint.x
                 <<","<<targetPoint.y
                 <<","<<targetPoint.z
                 <<endl;
    }
    csvFile.close();
}
