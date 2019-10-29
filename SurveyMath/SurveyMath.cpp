
#include "SurveyMath\SurveyMath.h"
#include <QDebug>
namespace SurveyMath {
    /**������ת��Ϊ�Զ�Ϊ��λ�ĽǶ�*/
    double RadianToDegree(double Radian)
    {
        double AngleDEG;
        double AngleRadian = Radian;
        AngleDEG = AngleRadian * 180.0 / PI;
        return AngleDEG;
    }
    /**���Զ�Ϊ��λ�ĽǶ�ת��Ϊ����*/
    double DegreeToRadian(double AngleDEG)
    {
        return AngleDEG * PI / 180.0;
    }
    /**������ת��Ϊ�Զȶȷ���*/
    double RadianToDMS(double Radian)
    {
        //1,������ת��Ϊ��
        double dDeg = RadianToDegree(Radian);
        //2,����ת��Ϊ�ȷ���
        return DegreeToDMS(dDeg);
    }
    /**���ȷ���Ƕ�ת��Ϊ����*/
    double DMSToRadian(double AngleDEG)
    {
        //1,���ȷ���ת��Ϊ��
        double dR = DMSToDegree(AngleDEG);
        //2,����ת��Ϊ����
        return DegreeToRadian(dR);
    }
    //���ȷ���ת��Ϊʮ���ƶ�
    double DMSToDegree(double DMS)
    {
        double AngleDEG, AngleDMS;
        double Degree, Minute, Second;
        double tSymbol = 1;
        if (DMS < 0)
        {
            tSymbol = -1;
        }
        AngleDMS = abs(DMS);
        Degree = int(AngleDMS);
        Minute = int(((AngleDMS - Degree) * 10000 + 0.5) / 100);
        Second = ((AngleDMS - Degree) * 100.0 - Minute) * 100.0;
        AngleDEG = Degree + Minute / 60.0 + Second / 3600.0;
        return tSymbol * AngleDEG;
    }
    //��ʮ���ƶ�ת��Ϊ�ȷ���
    double DegreeToDMS(double DEG)
    {
        double AngleDMS, AngleDEG;
        double Degree, Minute, Second;
        double tSymbol = 1;
        if (DEG < 0)
        {
            tSymbol = -1;
        }
        AngleDEG = abs(DEG);
        Degree = int((AngleDEG*1000000000 + 5)/1000000000);
        Minute = int((AngleDEG - Degree) * 60);
        Second = ((AngleDEG - Degree) * 60.0 - Minute) * 60;
        AngleDMS = Degree + Minute / 100.0 + Second / 10000.0;
        return tSymbol * AngleDMS;
    }

    /**���Զ�Ϊ��λ�ĽǶ�ת��Ϊ�ٷֶ�*/
    double DegreeToGradian(double AngleDEG)
    {
        double dT = 400.0 / 360.0;
        return AngleDEG * dT;
    }
    /**���ٷֶȻ�Ϊ�Զ�Ϊ��λ�ĽǶ�*/
    double GradianToDegree(double Gradian)
    {
        double dT = 400.0 / 360.0;
        return Gradian / dT;
    }

}

