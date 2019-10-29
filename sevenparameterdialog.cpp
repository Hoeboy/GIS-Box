#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include "sevenparameterdialog.h"
#include "ui_sevenparameterdialog.h"
#include "sevenparameterhassistant.h"
SevenParameterDialog::SevenParameterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SevenParameterDialog)
{
    ui->setupUi(this);
    Ellipsoid ell_wgs84(6378137.0, 1/298.257223563);
    Ellipsoid ell_bj54(6378245.0, 1/298.3);
    Ellipsoid ell_xian80(6378140.0, 1/298.257);
    Ellipsoid ell_cscg2000(6378137.0, 1/298.257222101);
    prj_bj54.SetDatum(ell_bj54);
    prj_wgs84.SetDatum(ell_wgs84);
    prj_xian80.SetDatum(ell_xian80);
    prj_cscg2000.SetDatum(ell_cscg2000);

    ProjectName[0]=QString::fromLocal8Bit("WGS84");
    ProjectName[1]=QString::fromLocal8Bit("����2000");
    ProjectName[2]=QString::fromLocal8Bit("����54");
    ProjectName[3]=QString::fromLocal8Bit("����80");


    CoordinateType[0]=QString::fromLocal8Bit("�������");
    CoordinateType[1]=QString::fromLocal8Bit("��������");

    isCaculate=false;
    pRevLongitude = new  QRegExpValidator(QRegExp("^-?(180|1?[0-7]?\\d(\\.\\d{1,10})?)$")) ;//������ʽ����ƥ��
    ui->lineEdit_MiddleLon->setValidator(pRevLongitude);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//��񲻿ɱ༭
}

SevenParameterDialog::~SevenParameterDialog()
{
    delete ui;
    delete pRevLongitude;
}


void SevenParameterDialog::on_pushButton_Caculate_clicked()
{

    if(ui->tableWidget->rowCount() < 3)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("�����߲�����������������!"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }
    ui->pushButton_Enter->setEnabled(true);
    ui->pushButton_View->setEnabled(true);

    inputPointVector.clear();
    outputPointVector.clear();

    //Դ����ϵͳ
    sourcePrjoject = prj_wgs84;

     //Ŀ������ϵͳ
    if(ui->comboBox_Project->currentIndex()==0)//����2000
    {
        targetPrjoject = prj_cscg2000;
    }
    if(ui->comboBox_Project->currentIndex()==1)//����54
    {
        targetPrjoject = prj_bj54;
    }
    if(ui->comboBox_Project->currentIndex()==2)//����80
    {
        targetPrjoject = prj_xian80;
    }

    //һ����
    if(ui->comboBox_CaculateMethod->currentIndex() == 1)
    {
        double dMiddle = 0;
        int iCount = sourcePointVector.size();
        for(int i=0;i<iCount;i++)
        {
            dMiddle = dMiddle + DMSToRadian(sourcePointVector[i].y);
        }
        dMiddle = dMiddle / iCount;//��������������
        sourcePrjoject.SetFalseEasting(500000);
        sourcePrjoject.SetFalseNorthing(0);
        sourcePrjoject.SetCentralMeridian(dMiddle);
        targetPrjoject = sourcePrjoject;//һ����ʱ�趨Դ����ϵͳ��Ŀ������ϵͳ��ΪWGS84
        //һ����ʱ��WGS84�������ת����WGS84ƽ������,Ȼ������WGS84ƽ�������Ŀ������ƽ��������ת������
        for(int i=0;i<iCount;i++)
        {
            //���ȷ���ת��Ϊ����
            BasePoint gP(DMSToRadian(sourcePointVector[i].x),DMSToRadian(sourcePointVector[i].y),sourcePointVector[i].z);
           //��WGS84�������ת����WGS84ƽ������
            BasePoint tP = CoordinateTransformation::PositiveGauss(sourcePrjoject,gP);

            inputPointVector.push_back(tP);
        }
        //Ŀ������Ϊƽ������
        outputPointVector = targetPointVector;


    }
    if(ui->comboBox_CaculateMethod->currentIndex() == 0)//�����߲�
    {
        double  middleLongitude=DMSToRadian( ui->lineEdit_MiddleLon->text().toDouble());
        sourcePrjoject.SetCentralMeridian(middleLongitude);
        targetPrjoject.SetCentralMeridian(middleLongitude);

        //��ȡWGS84�������
        for(int i=0;i<sourcePointVector.size();i++)
        {
            //���ȷ���ת��Ϊ����
            BasePoint gP(DMSToRadian(sourcePointVector[i].x),DMSToRadian(sourcePointVector[i].y),sourcePointVector[i].z);
           //�����ȵ�λ�Ĵ������ת���ɵ���ֱ������
            BasePoint tP = CoordinateTransformation::GeodeticToGeocentric(sourcePrjoject,gP);
            inputPointVector.push_back(tP);
        }

        //Ŀ������ƽ������
        for(int i=0;i<targetPointVector.size();i++)
        {
           //��ƽ������ת���ɴ������
            BasePoint gP = CoordinateTransformation::InverseGauss(targetPrjoject,targetPointVector[i]);
           //�����ȵ�λ�Ĵ������ת���ɵ���ֱ������
            BasePoint tP = CoordinateTransformation::GeodeticToGeocentric(targetPrjoject,gP);
            outputPointVector.push_back(tP);
        }
     }

    int iCount = inputPointVector.size();
    sevenParameter.Calculate(inputPointVector.data(),outputPointVector.data(),iCount);//�����߲���
    iCount = 0;
    //��ȡԴ�������ֱ������
    for(BasePoint sP:inputPointVector)
    {


        //ͨ��ת��������WGS84����ֱ������ת����Ŀ������ϵ����ֱ������(�����߲�ʱ����ƽ������ת����Ŀ������ƽ�����꣨һ����ʱ��
       BasePoint cP = sevenParameter.Transform(sP);
       BasePoint tP;
       if(ui->comboBox_CaculateMethod->currentIndex() == 0)//�����߲�
       {
           //��Ŀ�����ֱ������ϵת���ɴ������
           BasePoint gP = CoordinateTransformation::GeocentricToGeodetic(targetPrjoject,cP);
           //��Ŀ��������ϵת���ɸ�˹ֱ������
           tP = CoordinateTransformation::PositiveGauss(targetPrjoject,gP);
       }
       else//һ����
       {
            tP = cP;
       }

       QTableWidgetItem *tItemX = new QTableWidgetItem(QString::number(targetPointVector[iCount].x - tP.x,'f',3));
       tItemX->setTextAlignment(Qt::AlignCenter);
       QTableWidgetItem *tItemY = new QTableWidgetItem(QString::number(targetPointVector[iCount].y - tP.y,'f',3));
       tItemY->setTextAlignment(Qt::AlignCenter);
       QTableWidgetItem *tItemZ = new QTableWidgetItem(QString::number(targetPointVector[iCount].z - tP.z,'f',3));
       tItemZ->setTextAlignment(Qt::AlignCenter);
       ui->tableWidget->setItem(iCount, 7,tItemX);
       ui->tableWidget->setItem(iCount, 8,tItemY);
       ui->tableWidget->setItem(iCount, 9,tItemZ);
       iCount++;
    }


    QMessageBox::about(this, QString::fromLocal8Bit("������"),
                       QString::fromLocal8Bit("DX(��) = ") + QString::number(sevenParameter.DX,10,10)
                       + QString::fromLocal8Bit("\r\nDY(��) = " )+ QString::number(sevenParameter.DY,10,10)
                       + QString::fromLocal8Bit("\r\nDZ(��) = ") + QString::number(sevenParameter.DZ,10,10)
                       + QString::fromLocal8Bit("\r\nRX(��) = ") + QString::number(sevenParameter.GetSecondRX(),10,10)
                       + QString::fromLocal8Bit("\r\nRY(��) = ") + QString::number(sevenParameter.GetSecondRY(),10,10)
                       + QString::fromLocal8Bit("\r\nRZ(��) = ") + QString::number(sevenParameter.GetSecondRZ(),10,10)
                       + QString::fromLocal8Bit("\r\nPPM = ") + QString::number(sevenParameter.GetPPM(),10,10));
}

void SevenParameterDialog::on_pushButton_Close_clicked()
{
    close();
}


void SevenParameterDialog::on_pushButton_Target_File_clicked()
{
    QString csvFileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��Ŀ���ļ�"), ".", QString::fromLocal8Bit("�ı��ļ�(*.csv)"), 0);
    if (!csvFileName.isEmpty())
    {
        ui->lineEdit_GCP_FilePath->setText(csvFileName);

        QFile sourceFile(ui->lineEdit_GCP_FilePath->text());
        if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
        {
           return;
        }

        QTextStream inSourceFile(&sourceFile);


        ui->tableWidget->verticalHeader()->setVisible(false); // ����ˮƽheader
        //��ȡԴ����
        while(!inSourceFile.atEnd())
        {
            QStringList lineList;
            QString strLine = inSourceFile.readLine();
            lineList = strLine.split(",", QString::SkipEmptyParts);
            BasePoint sP;
            sP.x=lineList.at(1).toDouble();
            sP.y=lineList.at(2).toDouble();
            sP.z=lineList.at(3).toDouble();
            sourcePointVector.push_back(sP);
            BasePoint tP;
            tP.x=lineList.at(4).toDouble();
            tP.y=lineList.at(5).toDouble();
            tP.z=lineList.at(6).toDouble();
            targetPointVector.push_back(tP);
            int rowIndex =  ui->tableWidget->rowCount();
             ui->tableWidget->setRowCount(rowIndex + 1);//����������1

            for(int i = 0; i <  ui->tableWidget->columnCount(); i++)
            {

               if(i<7)
               {
                   QTableWidgetItem *tItem=new QTableWidgetItem(lineList[i]);
                   tItem->setTextAlignment(Qt::AlignCenter);
                   ui->tableWidget->setItem(rowIndex, i,tItem);
               }
               else
               {
                   QTableWidgetItem *tItem=new QTableWidgetItem(QString::fromLocal8Bit("δ����"));
                   tItem->setTextAlignment(Qt::AlignCenter);
                   ui->tableWidget->setItem(rowIndex, i,tItem);
               }
               // ui->tableWidget->item(rowIndex, i)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            }

        }
    }
    else
    {
        return;
    }
}


void SevenParameterDialog::on_pushButton_View_clicked()
{
    QMessageBox::about(this, QString::fromLocal8Bit("�鿴7����"),
                       QString::fromLocal8Bit("DX(��) = ") + QString::number(sevenParameter.DX,10,10)
                       + QString::fromLocal8Bit("\r\nDY(��) = " )+ QString::number(sevenParameter.DY,10,10)
                       + QString::fromLocal8Bit("\r\nDZ(��) = ") + QString::number(sevenParameter.DZ,10,10)
                       + QString::fromLocal8Bit("\r\nRX(��) = ") + QString::number(sevenParameter.GetSecondRX(),10,10)
                       + QString::fromLocal8Bit("\r\nRY(��) = ") + QString::number(sevenParameter.GetSecondRY(),10,10)
                       + QString::fromLocal8Bit("\r\nRZ(��) = ") + QString::number(sevenParameter.GetSecondRZ(),10,10)
                       + QString::fromLocal8Bit("\r\nPPM = ") + QString::number(sevenParameter.GetPPM(),10,10));

}

void SevenParameterDialog::on_pushButton_Enter_clicked()
{
    //һ����
    if(ui->comboBox_CaculateMethod->currentIndex() == 1)
    {
        SevenParameterhAssistant::Type = 0;
        SevenParameterhAssistant::projectName = QString::fromLocal8Bit("һ");
    }
    else
    {
        SevenParameterhAssistant::Type = 1;
        SevenParameterhAssistant::projectName = ui->comboBox_Project->currentText();
    }
    SevenParameterhAssistant::sourcePrjoject = sourcePrjoject;
    SevenParameterhAssistant::targetPrjoject = targetPrjoject;
    SevenParameterhAssistant::sevenParameter = sevenParameter;
    accept();
}

void SevenParameterDialog::on_pushButton_Cancel_clicked()
{
    reject();
}

void SevenParameterDialog::on_comboBox_CaculateMethod_activated(const QString &arg1)
{
    if(arg1==QString::fromLocal8Bit("�����߲�"))
    {
        ui->comboBox_Project->setEnabled(true);
        ui->lineEdit_MiddleLon->setEnabled(true);
        ui->lineEditt_FalseEast->setEnabled(true);
        ui->lineEdit_FalseNorth->setEnabled(true);
    }
    else
    {
        ui->comboBox_Project->setEnabled(false);
        ui->lineEdit_MiddleLon->setEnabled(false);
        ui->lineEditt_FalseEast->setEnabled(false);
        ui->lineEdit_FalseNorth->setEnabled(false);
    }

}

