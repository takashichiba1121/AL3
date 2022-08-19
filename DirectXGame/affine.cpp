#include "WorldTransform.h"
#include<cmath>
#include"affine.h"

//単位行列
void affine::makeMatIdentity(Matrix4& matrix) {
	matrix = {
	   1.0f,0,0,0,
	   0,1.0f,0,0,
	   0,0,1.0f,0,
	   0,0,0,1.0f,
	};
}

//アフィン変換拡縮
void affine::makeMatScale(Matrix4& matrix, Vector3 scale) {
	Matrix4 matScale;
	makeMatIdentity(matScale);
	matScale.m[0][0] = scale.x;
	matScale.m[1][1] = scale.y;
	matScale.m[2][2] = scale.z;
	matrix *= matScale;
}

//アフィン変換回転
void affine::makeMatRot(Matrix4& matrix, Vector3 rot) {
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
void affine::makeMatTrans(Matrix4& matrix, Vector3 trans) {
	Matrix4  matTrams;
	makeMatIdentity(matTrams);
	matTrams.m[3][0] = trans.x;
	matTrams.m[3][1] = trans.y;
	matTrams.m[3][2] = trans.z;

	matrix *= matTrams;
}

Vector3 affine::MatVector(Matrix4 matrix, Vector3 vector) {
	Vector3 matVector = { 0,0,0 };
	matVector.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0];
	matVector.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1];
	matVector.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2];
	return matVector;
}
Vector3 affine::Mat3D(Matrix4 matrix, Vector3 vector) {
	Vector3 matVector = { 0,0,0 };
	matVector.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] * 1 * matrix.m[3][0];
	matVector.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] * 1 * matrix.m[3][1];
	matVector.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] * 1 * matrix.m[3][2];
	return matVector;
}
Vector3 affine::GetWorldTrans(Matrix4 matrix) {
	Vector3 mattrans;
	mattrans.x = matrix.m[3][0];
	mattrans.y = matrix.m[3][1];
	mattrans.z = matrix.m[3][2];
	return mattrans;
}
Vector3 affine::wdivision(Matrix4 matrix, Vector3 vector)
{
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + matrix.m[3][3];

	Vector3 result
	{
		(vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + matrix.m[3][0]) / w,
		(vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + matrix.m[3][1]) / w,
		(vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + matrix.m[3][2]) / w
	};

	return result;
}