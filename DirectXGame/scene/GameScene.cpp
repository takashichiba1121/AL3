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
}

void GameScene::Initialize() {

	debugCamera_ = new DebugCamera(1280, 720);
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();
	//自キャラの生成
	player_ = new Player();
	enemy_ = new Enemy();
	model_ = Model::Create();
	//自キャラの初期化
	player_->Initialize(model_,textureHandle);
	enemy_->Initialize(model_);

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	
	//敵キャラに自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);
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
	//自キャラの描画
	player_->Draw(viewProjection_);
	enemy_->Draw(viewProjection_);
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
