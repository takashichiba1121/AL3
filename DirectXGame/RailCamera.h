#pragma once
#include"WorldTransform.h"
#include"ViewProjection.h"
#include"DebugText.h"
#include "Input.h"
/// <summary>
/// 自キャラ
/// </summary>
class RailCamera {

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 translation, Vector3 rot);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	ViewProjection& GetViewProjection() { return viewProjection_; };

	WorldTransform* GetWorldTransform() { return &worldTransform_; };
private:
	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

	DebugText* debugText_ = nullptr;

	Input* input_ = nullptr;

	float PI = 3.1415927f;
};