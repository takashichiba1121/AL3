#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "DebugText.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include"DebugCamera.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

  public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	//アフィン変換拡縮
	Matrix4 matScale(Vector3 Scale);

	//アフィン変換回転
	Matrix4 matRot(Vector3 Rot);

	//アフィン変換平行移動
	Matrix4 matTrams(Vector3 Trams);

	//アフィン変換
	void mat();

  private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;

	//テクスチャハンドル
	uint32_t textureHandle_ = 0;

	//3Dモデル
	Model* model_ = nullptr;

	//ワールドトランスフォーム
	WorldTransform worldTransform_;
	//ビュープロダクション
	ViewProjection viewProjection_;
	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;
	float PI = 3.1415927f;
	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
