#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

#include "ui_dialogchangeangle.h"
#include "dialogchangeangle.h"
DialogChangeAngle::DialogChangeAngle(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChangeAngle)
{
    ui->setupUi(this);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//��񲻿ɱ༭
}

DialogChangeAngle::~DialogChangeAngle()
{
    delete ui;
}
double DialogChangeAngle::ChangeAngle(double inputAngle)
{
    double dSource=inputAngle;
    double dTarget = 0;
    QString qStrSource=ui->comboBox_AngleTran_Source->currentText();
    QString qStrTarget=ui->comboBox_AngleTran_Target->currentText();
    QString strAngle[4];
    strAngle[0]=QString::fromLocal8Bit("ʮ���ƶ�");
    strAngle[1]=QString::fromLocal8Bit("�ȷ���");
    strAngle[2]=QString::fromLocal8Bit("����");
    strAngle[3]=QString::fromLocal8Bit("�ٷֶ�");

    if(qStrSource == strAngle[0] )
    {
        if(qStrTarget == strAngle[1])//ʮ���ƶ�ת�ȷ���
        {
           dTarget = DegreeToDMS(dSource);
        }
        if(qStrTarget == strAngle[2])//ʮ���ƶ�ת����
        {
           dTarget = DegreeToRadian(dSource);
        }
        if(qStrTarget == strAngle[3])//ʮ���ƶ�ת�ٷֶ�
        {
           dTarget = DegreeToGradian(dSource);
        }
    }
    if(qStrSource == strAngle[1] )
    {
        if(qStrTarget == strAngle[0])//�ȷ���תʮ���ƶ�
        {
           dTarget = DMSToDegree(dSource);
        }
        if(qStrTarget == strAngle[2])//�ȷ����ת����
        {
           dTarget = DMSToRadian(dSource);
        }
        if(qStrTarget == strAngle[3])//�ȷ���ת�ٷֶ�
        {
           double dDeg=DMSToDegree(dSource);//�ȰѶȷ���תΪ��
           dTarget = DegreeToGradian(dDeg);
        }
    }
    if(qStrSource == strAngle[2] )
    {
        if(qStrTarget == strAngle[0])//����תʮ���ƶ�
        {
            dTarget = RadianToDegree(dSource);

        }
        if(qStrTarget == strAngle[1])//����ת�ȷ���
        {
            dTarget = RadianToDMS(dSource);

        }
        if(qStrTarget == strAngle[3])//����ת�ٷֶ�
        {
            double dDeg=RadianToDegree(dSource);//�Ȱѻ���תΪ��
            dTarget = DegreeToGradian(dDeg);

        }
    }
    if(qStrSource == strAngle[3] )
    {
        if(qStrTarget == strAngle[0])//�ٷֶ�תʮ���ƶ�
        {
            dTarget = GradianToDegree(dSource);

        }
        if(qStrTarget == strAngle[1])//�ٷֶ�ת�ȷ���
        {
            double dDeg=GradianToDegree(dSource);//�ȰѰٷֶ�תΪ��
            dTarget =DegreeToDMS(dDeg) ;
        }
        if(qStrTarget == strAngle[2])//�ٷֶ�ת����
        {
            double dDeg=GradianToDegree(dSource);//�ȰѰٷֶ�תΪ��
            dTarget =DegreeToRadian(dDeg) ;
        }
    }
    return dTarget;
}

void DialogChangeAngle::on_pushButton_AngleTran_Point_clicked()
{
    double dSource=ui->lineEdit_AngleTran_Soruce->text().toDouble();
    double dTarget = ChangeAngle(dSource);
    ui->lineEdit_AngleTran_Target->setText(QString::number(dTarget,14,9));
}

void DialogChangeAngle::on_comboBox_AngleTran_Source_currentIndexChanged(const QString &arg1)
{
    ui->comboBox_AngleTran_Target->clear();
    QString strAngle[4];
    strAngle[0]=QString::fromLocal8Bit("ʮ���ƶ�");
    strAngle[1]=QString::fromLocal8Bit("�ȷ���");
    strAngle[2]=QString::fromLocal8Bit("����");
    strAngle[3]=QString::fromLocal8Bit("�ٷֶ�");
    for(int i=0;i<4;i++)
    {
        if(strAngle[i]!=arg1)
        {
            ui->comboBox_AngleTran_Target->addItem(strAngle[i]);

        }
    }
}

void DialogChangeAngle::on_pushButton_Another_AngleTran_SelectFile_clicked()
{
    QString csvFileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ��Դ�ļ�"), UserFilePath, QString::fromLocal8Bit("�ı��ļ�(*.csv)"), 0);
    if (!csvFileName.isEmpty())
    {
        ui->lineEdit_AngleTran_FileName->setText(csvFileName);
    }
    else
    {
        return;
    }
    QFileInfo fileInfo (csvFileName);
    UserFilePath = fileInfo.filePath();

    QFile sourceFile(ui->lineEdit_AngleTran_FileName->text());
    QTextStream inSourceFile(&sourceFile);

    if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       return;
    }
    //��ȡԴ����
    QStringList lineList;
    QString strLine = inSourceFile.readLine();
    sourceFile.close();
    lineList = strLine.split(",", QString::SkipEmptyParts);

    QStringList headerLabels;
    for (int i=1;i<=lineList.count();i++)
    {

        headerLabels.append(QString::fromLocal8Bit("��")+QString::number(i));
    }

    ui->tableWidget->setColumnCount(lineList.size());
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->verticalHeader()->setVisible(false); // ����ˮƽheader
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);

    for(int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
       QTableWidgetItem   *item = new QTableWidgetItem(lineList[i]);
       item->setTextAlignment(Qt::AlignCenter);
       ui->tableWidget->setItem(0, i,item);
    }
}

void DialogChangeAngle::on_pushButton_AngleTran_File_clicked()
{

    std::vector<int> vecItemIndex;//����ѡ���е�����
    QItemSelectionModel *selections =  ui->tableWidget->selectionModel(); //���ص�ǰ��ѡ��ģʽ
    QModelIndexList selectedsList = selections->selectedIndexes(); //��������ѡ����ģ����Ŀ�����б�

    for (int i = 0; i < selectedsList.count(); i++)
    {
        vecItemIndex.push_back(selectedsList.at(i).column());
    }
    std::sort(vecItemIndex.begin(), vecItemIndex.end());
    vecItemIndex.erase(std::unique(vecItemIndex.begin(), vecItemIndex.end()), vecItemIndex.end());

    if(vecItemIndex.size()<1)
    {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ѡ����Ҫת������!"), QMessageBox::Ok, QMessageBox::Ok);
        return;
    }



    QFile sourceFile(ui->lineEdit_AngleTran_FileName->text());
    QTextStream inSourceFile(&sourceFile);
    QVector<SurveyPoint> inputPointVector;
    if(!sourceFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       return;
    }
    QString csvfileName = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("�������ļ���"), UserFilePath, QString::fromLocal8Bit("csv�ļ�(*.csv)"),0);
    QFileInfo fileInfo (csvfileName);
    UserFilePath = fileInfo.filePath().left( fileInfo.filePath().size() -4) + "T.CSV" ;
    QFile csvFile(csvfileName);
    if(!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return ;
    }

    QTextStream txtOutput(&csvFile);
    //��ȡԴ����
    while(!inSourceFile.atEnd())
    {


        QStringList lineList;
        QString strLine = inSourceFile.readLine();
        lineList = strLine.split(",", QString::SkipEmptyParts);
        int beginColumn = vecItemIndex[0];
        int endColumn = vecItemIndex[vecItemIndex.size()-1];
        for(int i=0;i<lineList.size();i++)
        {
            if(i<beginColumn || i>endColumn)
            {
                txtOutput<<lineList[i];
            }
            else
            {
                qDebug()<<i;
                txtOutput<<qSetRealNumberPrecision(11)<<fixed
                 << ChangeAngle(lineList[i].toDouble());
            }
            if(i<lineList.size() - 1)
            {
               txtOutput<<",";
            }
        }
            txtOutput<<endl;
    }
    sourceFile.close();
    csvFile.close();
}
