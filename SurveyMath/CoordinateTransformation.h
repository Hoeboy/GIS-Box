#ifndef COORDINATETRANSFORMATION_H
#define COORDINATETRANSFORMATION_H
#include "SurveyMath\BasePoint.h"
#include "SurveyMath\Projection.h"
namespace SurveyMath {
	class CoordinateTransformation
	{
	public:
        CoordinateTransformation(){}

	public:
		//ת�������ת��Ϊ�ռ�ֱ������
		static GeocentricCoordinate GeodeticToGeocentric(Projection prj, GeodeticCoordinate GeodeticPoint);
		//ת�ռ�ֱ�����껻Ϊ�������
		static GeodeticCoordinate GeocentricToGeodetic(Projection prj, GeocentricCoordinate GeocentricPoint);

		//��˹��������
        static CartesianCoordinate PositiveGauss(Projection prj, GeodeticCoordinate GeodeticPoint);
		static CartesianCoordinate PositiveUTM(GeodeticCoordinate GeodeticPoint);
		//��˹���귴��
		static GeocentricCoordinate InverseGauss(Projection prj, CartesianCoordinate CartesianPoint);
		static	GeocentricCoordinate InverseUTM(int Zone, bool IsSouth, CartesianCoordinate CartesianPoint);

	private:
		//ƽ������ת�������
		static CartesianCoordinate GeodeticToCartesian(Projection prj, GeodeticCoordinate  GeodeticPoint);
		//�������תƽ������
		static GeocentricCoordinate CartesianToGeodetic(Projection prj, CartesianCoordinate CartesianPoint);

		static double  X0(double B, double a, double e1);
		//�����߻���
		static double  FB0(double B, double a, double e1);
		//�׵�γ���õ��ĺ���
		static double  FB1(double B, double a, double e1);
	};
}
#endif //COORDINATETRANSFORMATION_H
