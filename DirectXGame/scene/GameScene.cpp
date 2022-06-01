#include "GameScene.h"
#include "TextureManager.h"
#include"AxisIndicator.h"
#include <cassert>
#include<cmath>
#include<random>
#include"PLayer.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete debugCamera_;
	delete player_;
	delete model_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	player_ = new Player();
	model_ = Model::Create();
	player_->Initialize(model_,textureHandle);

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
	
	////X,Y,Z 方向のスケーリングを設定
	//worldTransform_.scale_={ 2,2,2 };
	//worldTransform_.rotation_ = { PI / 4,PI / 4,PI/4 };
	//worldTransform_.translation_ = { 10,10,10 };
	////スケーリング行列を宣言
	//Matrix4 matScale=
	//{
	//	worldTransform_.scale_.x,0,0,0,
	//	0,worldTransform_.scale_.y,0,0,
	//	0,0,worldTransform_.scale_.z,0,
	//	0,0,0,1,
	//};
	//Matrix4 matRotZ =
	//{
	//	1,0,0,0,
	//	0,cos(worldTransform_.rotation_.z),sin(worldTransform_.rotation_.z),0,
	//	0,-sin(worldTransform_.rotation_.z),cos(worldTransform_.rotation_.z),0,
	//	0,0,0,1,
	//};
	//Matrix4 matRotY =
	//{
	//	cos(worldTransform_.rotation_.y),0,-sin(worldTransform_.rotation_.z),0,
	//	0,1,0,0,
	//	sin(worldTransform_.rotation_.y),0,cos(worldTransform_.rotation_.z),0,
	//	0,0,0,1,
	//};
	//Matrix4 matRotX =
	//{
	//	cos(worldTransform_.rotation_.x),sin(worldTransform_.rotation_.x),0,0,
	//	-sin(worldTransform_.rotation_.x),cos(worldTransform_.rotation_.x),0,0,
	//	0,0,1,0,
	//	0,0,0,1,
	//};
	//Matrix4 matRot =
	//{
	//	1,0,0,0,
	//	0,1,0,0,
	//	0,0,1,0,
	//	0,0,0,1,
	//};
	//Matrix4  matTrams = MathUtility::Matrix4Identity();
	//matTrams =
	//{
	//	1,0,0,0,
	//	0,1,0,0,
	//	0,0,1,0,
	//	worldTransform_.translation_.x,worldTransform_.translation_.y,worldTransform_.translation_.z,1,
	//};

	//worldTransform_.matWorld_.m[0][0] = 1;
	//worldTransform_.matWorld_.m[1][1] = 1;
	//worldTransform_.matWorld_.m[2][2] = 1;
	//worldTransform_.matWorld_.m[3][3] = 1;
	//worldTransform_.matWorld_ *= matScale;
	//matRot*= matRotX;
	//matRot *= matRotY;
	//matRot *= matRotZ;
	//worldTransform_.matWorld_ *= matRot;
	//worldTransform_.matWorld_ *= matTrams;
	////行列の転送
	//worldTransform_.TransferMatrix();
	
}

void GameScene::Update() {
#ifdef _DEBUG
	if (input_->TriggerKey(DIK_RETURN))
	{
		isDebugCameraActive_ = -isDebugCameraActive_;
	}
#endif
	player_->Update();
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
	player_->Draw(viewProjection_);
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
