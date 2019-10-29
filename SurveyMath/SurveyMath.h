#ifndef SURVEYMATH_H
#define SURVEYMATH_H

#include <cmath>
namespace SurveyMath {
    const double PI = 3.14159265358979323846;
	/**������ת��Ϊ�Զ�Ϊ��λ�ĽǶ�*/
	double RadianToDegree(double Radian);
	/**���Զ�Ϊ��λ�ĽǶ�ת��Ϊ����*/
	double DegreeToRadian(double AngleDEG);
	/**������ת��Ϊ�Զȶȷ���*/
	double RadianToDMS(double Radian);
	/**���ȷ���Ƕ�ת��Ϊ����*/
	double DMSToRadian(double AngleDEG);
	//���ȷ���ת��Ϊʮ���ƶ�
	double DMSToDegree(double DMS);
	//��ʮ���ƶ�ת��Ϊ�ȷ���
	double DegreeToDMS(double DEG);
	/**���Զ�Ϊ��λ�ĽǶ�ת��Ϊ�ٷֶ�*/
	double DegreeToGradian(double AngleDEG);
	/**���ٷֶȻ�Ϊ�Զ�Ϊ��λ�ĽǶ�*/
	double GradianToDegree(double Gradian);
}

#endif //SURVEYMATH_H
