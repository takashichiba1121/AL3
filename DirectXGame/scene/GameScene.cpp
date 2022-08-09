#include "GameScene.h"
#include "TextureManager.h"
#include"AxisIndicator.h"
#include <cassert>
#include<cmath>
#include<random>
#include"PLayer.h"
#include"Enemy.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete debugCamera_;
	//自キャラの解放
	delete player_;
	delete model_;
	delete enemy_;
	delete skydomeModel_;
}

void GameScene::Initialize() {

	debugCamera_ = new DebugCamera(1280, 720);
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//自キャラの生成
	skydome = std::make_unique<Skydome>();
	railCamera_ = std::make_unique <RailCamera>();
	player_ = new Player();
	enemy_ = new Enemy();
	model_ = Model::Create();
	//自キャラの初期化
	player_->Initialize(model_,textureHandle);
	enemy_->Initialize(model_);

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	
	railCamera_->Initialize( Vector3{0.0f,0.0f,-50.0f}, Vector3{0.0f,0.0f,0.0f});

	player_->SetParent(railCamera_->GetWorldTransform());

	//敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	skydomeModel_ = Model::CreateFromOBJ("test", true);

	skydome->Initialize(skydomeModel_);
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_W))
	{
		if (isDebugCameraActive_) { isDebugCameraActive_ = false; }
		else { isDebugCameraActive_ = true; }
	}
#endif
	if (isDebugCameraActive_){
		debugCamera_->Update();
		viewProjection_ = debugCamera_->GetViewProjection();
	}else{
		viewProjection_.Initialize();
	}
	//自キャラの更新
	player_->Update();
	//敵キャラの更新
	enemy_->Update();

	//衝突判定
	CheckAllCollisions();

	railCamera_->Update();

	debugText_->SetPos(10, 30);
	debugText_->Printf("%d", isDebugCameraActive_);
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	//3Dモデル描画
	skydome->Draw(railCamera_->GetViewProjection());
	//自キャラの描画
	player_->Draw(railCamera_->GetViewProjection());
	enemy_->Draw(railCamera_->GetViewProjection());
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
void GameScene::CheckAllCollisions()
{
	//判定対象AとBの座標
	Vector3 posA, posB;

	//自弾リストの取得
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();
	//敵弾リストの取得
	const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵弾の当たり判定
	//自キャラの座標
	posA = player_->GetworldPosition();

	//自キャラと敵弾全ての当たり判定
	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
		//敵弾の座標
		posB = bullet->GetworldPosition();

		const float AR = 1;
		const float BR = 1;

		float A = pow((posB.x-posA.x),2) + pow((posB.y-posA.y),2) + pow((posB.z-posA.z),2);
		float B = pow((AR + BR),2);

		if (A <= B)
		{
			//自キャラの衝突時コールバックを呼び出す
			player_->OnCollision();
			//敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵キャラの当たり判定
	//敵キャラの座標
	posA = enemy_->GetworldPosition();

	//自弾と敵キャラ全ての当たり判定
	for (const std::unique_ptr<PlayerBullet>& bullet : playerBullets) {
		//自弾の座標
		posB = bullet->GetworldPosition();

		const float AR = 1;
		const float BR = 1;

		float A = pow((posB.x - posA.x), 2) + pow((posB.y - posA.y), 2) + pow((posB.z - posA.z), 2);
		float B = pow((AR + BR), 2);

		if (A <= B)
		{
			//自キャラの衝突時コールバックを呼び出す
			enemy_->OnCollision();
			//敵弾の衝突時コールバックを呼び出す
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自弾と敵弾の当たり判定
	//自キャラと敵弾全ての当たり判定
	for (const std::unique_ptr<EnemyBullet>& enemybullet : enemyBullets) {
		//自弾と敵キャラ全ての当たり判定
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
			//自弾の座標
			posA = playerbullet->GetworldPosition();
			//自弾の座標
			posB = enemybullet->GetworldPosition();

			const float AR = 1;
			const float BR = 1;

			float A = pow((posB.x - posA.x), 2) + pow((posB.y - posA.y), 2) + pow((posB.z - posA.z), 2);
			float B = pow((AR + BR), 2);

			if (A <= B)
			{
				//自キャラの衝突時コールバックを呼び出す
				enemybullet->OnCollision();
				//敵弾の衝突時コールバックを呼び出す
				playerbullet->OnCollision();
			}
		}
	}
#pragma endregion


}
