#pragma once
#include "Model.h"
class PlayerBullet
{
public:
	void Intialize(Model* model, const Vector3& position);
	/// <summary>
	/// �X�V
	/// </summary>
	void Update();
	/// <summary>
	/// �`��
	/// </summary>
	void Draw(const ViewProjection& viewProjection);
private:

};