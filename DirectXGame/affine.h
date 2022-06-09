#pragma once

#include "WorldTransform.h"

namespace MyFanc
{
	//単位行列
	void makeMatIdentity(Matrix4& matrix);

	//アフィン変換拡縮
	void makematScale(Matrix4& matrix, Vector3 scale);

	//アフィン変換回転
	void makematRot(Matrix4& matrix, Vector3 rot);

	//アフィン変換平行移動
	void makematTrans(Matrix4& matrix, Vector3 Trans);
}

