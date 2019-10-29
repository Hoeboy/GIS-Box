#include "dialogcoordinateinvertsolution.h"
#include "ui_dialogcoordinateinvertsolution.h"
#include "sevenparameterdialog.h"
#include "sevenparameterhassistant.h"
#include "selectcolumndialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
DialogCoordinateInvertsolution::DialogCoordinateInvertsolution(QWidget *parent) :
    QDialog (parent),
    ui(new Ui::DialogCoordinateInvertsolution)
{
    setFixedSize(640,425); // ��ֹ�ı䴰�ڴ�С��
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

    ui->label_Trans_Zone->setVisible(false);
    ui->lineEdit_Trans_Zone->setVisible(false);

    pRevZone = new  QRegExpValidator(QRegExp("^(([1-9]|[1-5][0-9]|(60)))([sSnN]?)$")) ;//������ʽ����ƥ��1N(S)-60N(S)
    pRevLatitude = new  QRegExpValidator(QRegExp("^-?(90|[1-8]?\\d(\\.\\d{1,10})?)$")) ;//������ʽγ��ƥ��
    pRevLongitude = new  QRegExpValidator(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d{1,10})?)$")) ;//������ʽ����ƥ��
    pRevDecimal = new  QRegExpValidator(QRegExp("^([0-9]{1,}[.][0-9]*)$")) ;//������ʽС��ƥ��
    ui->lineEdit_Trans_Zone->setValidator(pRevZone);
    ui->lineEdit_Trans_MiddleLon->setValidator(pRevLongitude);
    ui->lineEdit_Trans_FalseEast->setValidator(pRevDecimal);
    ui->lineEdit_Trans_FalseNorth->setValidator(pRevDecimal);
     ui->lineEdit_Trans_Source_X->setValidator(pRevDecimal);
    ui->lineEdit_Trans_Source_Y->setValidator(pRevDecimal);
    ui->lineEdit_Trans_Source_Z->setValidator(pRevDecimal);
    UserFilePath = ".";
    currentProjectIndex = 0;
}

DialogCoordinateInvertsolution::~DialogCoordinateInvertsolution()
{
    delete ui;
    delete pRevZone;
    delete pRevLatitude;
    delete pRevLongitude;
    delete pRevDecimal;
}


void DialogCoordinateInvertsolution::on_pushButton_Trans_TranCoor_clicked()
{
    BasePoint sourcePoint;
    BasePoint targetPoint;

    int iDecimalsG=10;
    int iDecimalsZ=4;

    //Դ����Ϊƽ������
    sourcePoint.x = ui->lineEdit_Trans_Source_X->text().toDouble();
    sourcePoint.y = ui->lineEdit_Trans_Source_Y->text().toDouble();
    sourcePoint.z = ui->lineEdit_Trans_Source_Z->text().toDouble();
    targetPoint = TransCoor(sourcePoint);//��λΪ���ȵĴ������
    ui->lineEdit_Trans_Target_X->setText(QString::number(RadianToDMS(targetPoint.x),10,iDecimalsG));
    ui->lineEdit_Trans_Target_Y->setText(QString::number(RadianToDMS(targetPoint.y),10,iDecimalsG));
    ui->lineEdit_Trans_Target_Z->setText(QString::number(targetPoint.z,10,iDecimalsZ));
}

void DialogCoordinateInvertsolution::on_comboBox_Trans_Project_currentIndexChanged(int index)
{
    if(index == 5)//ʹ��ת������
    {
        if(SevenParameterhAssistant::isSet == false)
        {
             QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�����������߲���!"), QMessageBox::Ok, QMessageBox::Ok);
             ui->comboBox_Trans_Project->setCurrentIndex(currentProjectIndex);
             return;
        }
        ui->lineEdit_Trans_MiddleLon->setEnabled(false);
        ui->lineEdit_Trans_FalseEast->setEnabled(false);
        ui->lineEdit_Trans_FalseNorth->setEnabled(false);
        ui->label_Trans_Zone->setVisible(false);
        ui->lineEdit_Trans_Zone->setVisible(false);
    }
    else
    {
        if(index == 4)//UTM
        {
            ui->label_Trans_Zone->setVisible(true);
            ui->lineEdit_Trans_Zone->setVisible(true);
            ui->lineEdit_Trans_MiddleLon->setEnabled(false);
            ui->lineEdit_Trans_FalseEast->setEnabled(false);
            ui->lineEdit_Trans_FalseNorth->setEnabled(false);
        }
        else
        {
            ui->lineEdit_Trans_MiddleLon->setEnabled(true);
            ui->lineEdit_Trans_FalseEast->setEnabled(true);
            ui->lineEdit_Trans_FalseNorth->setEnabled(true);
            ui->label_Trans_Zone->setVisible(false);
            ui->lineEdit_Trans_Zone->setVisible(false);
        }
    }
    currentProjectIndex = index;

}
//ѡ����Ҫ����ת�����ļ�
void DialogCoordinateInvertsolution::on_pushButton_Tans_SelectFile_clicked()
{
    QString csvFileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��Դ�ļ�"), UserFilePath, QString::fromLocal8Bit("�ı��ļ�(*.csv)"), 0);
    if (!csvFileName.isEmpty())
    {
        ui->lineEdit_Trans_FilePath->setText(csvFileName);
    }
    else
    {
        return;
    }
    QFileInfo fileInfo (csvFileName);
    UserFilePath = fileInfo.filePath().left( fileInfo.filePath().size() -4) + "G" ;
}
//������������ת��
void DialogCoordinateInvertsolution::on_pushButton_Tans_TansFile_clicked()
{
    QFile sourceFile(ui->lineEdit_Trans_FilePath->text());
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
            qDebug()<<lineList.size();
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
    QString csvfileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("�������ļ���"), UserFilePath, QString::fromLocal8Bit("csv�ļ�(*.csv)"),0);
    QFileInfo fileInfo (csvfileName);
    UserFilePath = fileInfo.filePath();
    QFile csvFile(csvfileName);
    if(!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return ;
    }
    QTextStream txtOutput(&csvFile);

    for(int i=0;i<inputPointVector.size();i++)
    {
        BasePoint sourcePoint;
        BasePoint targetPoint;
        int iDecimalsXY=10;
        //Դ����Ϊ��˹���꣬Ŀ������Ϊ�������
        sourcePoint.x =inputPointVector[i].x;
        sourcePoint.y =inputPointVector[i].y;
        sourcePoint.z =inputPointVector[i].z;
        targetPoint = TransCoor(sourcePoint);
        txtOutput<<inputPointVector[i].PointName
                 <<qSetRealNumberPrecision(iDecimalsXY)<<fixed
                 <<","<<RadianToDMS(targetPoint.x)
                 <<","<<RadianToDMS(targetPoint.y)
                 <<","<<targetPoint.z
                 <<endl;
    }
    csvFile.close();
}
BasePoint DialogCoordinateInvertsolution::TransCoor(BasePoint inputPoint)
{
    Projection currentProject;
    BasePoint outputPoint;
    if(ui->comboBox_Trans_Project->currentText()==ProjectName[0])//����2000
    {
        currentProject = prj_cscg2000;
    }
    if(ui->comboBox_Trans_Project->currentText()==ProjectName[1])//����54
    {
         currentProject = prj_bj54;
    }
    if(ui->comboBox_Trans_Project->currentText()==ProjectName[2])//����80
    {
         currentProject = prj_xian80;
    }
    if(ui->comboBox_Trans_Project->currentText()==ProjectName[3])//wgs84
    {
        currentProject = prj_wgs84;
    }
    if(ui->comboBox_Trans_Project->currentText()==ProjectName[4])//UTM
    {
        currentProject = prj_utm;
    }

    if(ui->comboBox_Trans_Project->currentIndex() == 5)//ʹ��7����
    {
        outputPoint = TransCoorFromSevenParam(inputPoint);
    }
    else
    {
        double middleLon = DMSToRadian(ui->lineEdit_Trans_MiddleLon->text().toDouble());
        double falseEast = ui->lineEdit_Trans_FalseEast->text().toDouble();
        double falseNorth = ui->lineEdit_Trans_FalseNorth->text().toDouble();
        currentProject.SetCentralMeridian(middleLon);
        currentProject.SetFalseEasting(falseEast);
        currentProject.SetFalseNorthing(falseNorth);
        //Դ����Ϊƽ������
        if(ui->comboBox_Trans_Project->currentText() == "UTM")//ͶӰ����ΪUTM
        {

            int zone=ui->lineEdit_Trans_Zone->text().left(ui->lineEdit_Trans_Zone->text().size()-1).toInt();
            bool isSourth = false;
            if(ui->lineEdit_Trans_Zone->text().right(1) == "N")
            {
                isSourth = false;
            }
            else
            {
                isSourth = true;
            }
            outputPoint = CoordinateTransformation::InverseUTM(zone,isSourth,inputPoint);
        }
        else
        {
            outputPoint = CoordinateTransformation::InverseGauss(currentProject,inputPoint);
        }
        outputPoint.z = inputPoint.z;
    }

    return outputPoint;
}
//�߲���ת�������нǶȵ�λΪ����
BasePoint DialogCoordinateInvertsolution::TransCoorFromSevenParam(BasePoint inputPoint)
{
    BasePoint outputPoint;
    sourcePrjoject =SevenParameterhAssistant::sourcePrjoject;
    targetPrjoject = SevenParameterhAssistant::targetPrjoject;
    if(SevenParameterhAssistant::Type == 0)//һ����
    {
        //Դ����Ϊ��˹����

        //����7�����������ƽ������ת���������WGS84ƽ������
       CartesianCoordinate gcp_t = SevenParameterhAssistant::sevenParameter.InverseTransform(inputPoint);

       //�������ƽ������ת��Ϊ�������
       outputPoint = CoordinateTransformation::InverseGauss(sourcePrjoject, gcp_t);

    }
    else//�����߲�
    {
        //Դ����Ϊ�������
        //��������˹����ת��Ϊ�������
        GeocentricCoordinate gdp_s = CoordinateTransformation::InverseGauss(targetPrjoject, inputPoint);
         //�������������ת��Ϊ��������
        GeocentricCoordinate gcp_s =CoordinateTransformation::GeodeticToGeocentric(targetPrjoject, gdp_s);
        //����7������������������ת����WGS84����ϵ�ĵ�������
        GeocentricCoordinate gcp_t = SevenParameterhAssistant::sevenParameter.InverseTransform(gcp_s);
        //��������������ת��ΪWGS84����ϵ�������
        outputPoint = CoordinateTransformation::GeocentricToGeodetic(sourcePrjoject, gcp_t);
    }

    return outputPoint;
}

void DialogCoordinateInvertsolution::on_lineEdit_Trans_MiddleLon_editingFinished()
{
    if(ui->comboBox_Trans_Project->currentText() == "UTM")//ͶӰ����ΪUTM
    {
        if(ui->lineEdit_Trans_MiddleLon->text().right(1) == "S")
        {
            ui->lineEdit_Trans_FalseNorth->setText("10000000");
        }
        else
        {
            ui->lineEdit_Trans_FalseNorth->setText("0");
        }
    }

}

