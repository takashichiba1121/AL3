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
#include "PLayer.h"
#include"Enemy.h"
#include"Skydome.h"
#include"RailCamera.h"
#include<sstream>


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
	void Initialize(GameScene* gameScene);

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void Fire(Vector3 trans);

	///<summary>
	///衝突判定と応答
	///</summary>
	void CheckAllCollisions();

	void AddEnemyBullet(std::unique_ptr<EnemyBullet> enemyBullet);

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugText* debugText_ = nullptr;
	//自キャラ
	Player* player_ = nullptr;
	//敵キャラ
	std::list<std::unique_ptr<Enemy>> enemy_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	//デバッグカメラ
	DebugCamera* debugCamera_ = nullptr;

	//デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	Model* model_ = nullptr;

	uint32_t textureHandle = 0;

	//カメラ上方向の角度
	float viewAngle = 0.0f;

	Model* skydomeModel_ = nullptr;

	std::unique_ptr<Skydome> skydome = nullptr;

	std::unique_ptr <RailCamera> railCamera_ = nullptr;

	//弾
	std::list<std::unique_ptr<EnemyBullet>> enemyBullets_;

	GameScene* gameScene_ = nullptr;

	std::stringstream enemyPopCommands;

	bool enemyPop = true;

	float enemyPopTime = true;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
