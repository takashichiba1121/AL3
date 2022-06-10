#pragma once
#include "Model.h"
class PlayerBullet
{
public:
	void Intialize(Model* model, const Vector3& position);
	/// <summary>
	/// çXêV
	/// </summary>
	void Update();
	/// <summary>
	/// ï`âÊ
	/// </summary>
	void Draw(const ViewProjection& viewProjection);
private:

};