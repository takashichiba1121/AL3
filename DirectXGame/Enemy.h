#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"EnemyBullet.h"

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
	///<summary>
	///弾発射
	///</summary>
	void Fire();
	void ApproachInutialize();
	//発射間隔
	static const int kFireInterval = 60;
private:
	//ワールドトランスフォーム
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	//弾
	std::list<std::unique_ptr<EnemyBullet>>bullets_;

	enum class Phase {
		Approach, //接近する
		Leave,    //離脱する
	};

	//フェーズ
	Phase phase_ = Phase::Approach;
	//発射タイマー
	int32_t fireTimer = 0;
};