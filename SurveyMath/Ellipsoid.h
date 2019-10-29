#ifndef ELLIPSOID_H
#define ELLIPSOID_H
#include "SurveyMath\SurveyMath.h"
namespace SurveyMath {
	class Ellipsoid
	{
	public:
		Ellipsoid()
		{
			LongAxis = 6378137;
			Flattening = 1.0/298.257223563;
		}
		Ellipsoid(double a, double f)
		{
			LongAxis = a;
			Flattening = f;
		}
	private:
		double LongAxis;//���򳤰���
		double Flattening;//�������
	public:
		double inline GetLongAxis()//���򳤰���
		{
			return LongAxis;
		}
		double inline GetFlattening()//�������
		{
			return Flattening;
		}
		double inline GetFirstEccentricity()//��һƫ����e
		{
			return sqrt(2.0 * Flattening - Flattening * Flattening);
		}
		double inline GetSecondEccentricity()//�ڶ�ƫ����e'
		{
			double b = LongAxis - LongAxis * Flattening;
			return sqrt(LongAxis * LongAxis - b * b) / b;
		}

	};
}
#endif //ELLIPSOID_H
