#include "WorldTransform.h"
#include<cmath>
#include"affine.h"

//単位行列
void MyFanc::makeMatIdentity(Matrix4& matrix)
{
	matrix = {
	   1.0f,0,0,0,
	   0,1.0f,0,0,
	   0,0,1.0f,0,
	   0,0,0,1.0f,
	};
}

//アフィン変換拡縮
void MyFanc::makeMatScale(Matrix4& matrix, Vector3 scale)
{
	Matrix4 matScale;
	makeMatIdentity(matScale);
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matrix *= matScale;
}

//アフィン変換回転
void MyFanc::makeMatRot(Matrix4& matrix, Vector3 rot)
{
	Matrix4 matrotZ;
	makeMatIdentity(matrotZ);
	matrotZ.m[1][1] = cos(rot.z);
	matrotZ.m[1][2] = sin(rot.z);
	matrotZ.m[2][1] = -sin(rot.z);
	matrotZ.m[2][2] = cos(rot.z);

	Matrix4 matrotY;
	makeMatIdentity(matrotY);
	matrotY.m[0][0] = cos(rot.y);
	matrotY.m[0][2] = -sin(rot.y);
	matrotY.m[2][0] = sin(rot.y);
	matrotY.m[2][2] = cos(rot.y);

	Matrix4 matrotX;
	makeMatIdentity(matrotX);
	matrotX.m[0][0] = cos(rot.x);
	matrotX.m[0][1] = sin(rot.x);
	matrotX.m[1][0] = -sin(rot.x);
	matrotX.m[1][1] = cos(rot.x);

	Matrix4 matRot;
	makeMatIdentity(matRot);

	matRot *= matrotX;
	matRot *= matrotY;
	matRot *= matrotZ;
	matrix *= matRot;
}

//アフィン変換平行移動
void MyFanc::makeMatTrans(Matrix4& matrix, Vector3 trans)
{
	Matrix4  matTrams;
	makeMatIdentity(matTrams);
	matTrams.m[3][0] = trans.x;
	matTrams.m[3][1] = trans.y;
	matTrams.m[3][2] = trans.z;

	matrix *= matTrams;
}