#pragma once
#include "Model.h"
#include "WorldTransform.h"

///<summary>
///敵
///</summary>
class Enemy {
public:
	void Initialize(Model* model);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& viewProjection);
private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
};