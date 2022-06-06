#include "GameScene.h"
#include "TextureManager.h"
#include"AxisIndicator.h"
#include <cassert>
#include<cmath>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_ = Model::Create();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	//ビュープロジェクションも初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280,720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロダクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
	
	//X,Y,Z 方向のスケーリングを設定
	worldTransform_.scale_={ 2,2,2 };
	worldTransform_.rotation_ = { PI / 4,PI / 4,PI/4 };
	worldTransform_.translation_ = { 10,10,10 };
	mat();
}

void GameScene::Update() {
	debugCamera_->Update();
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
	model_->Draw(worldTransform_, debugCamera_->GetViewProjection(), textureHandle_);
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
Matrix4 GameScene::matScale(Vector3 Scale)
{
	Matrix4 matScale =
	{
		Scale.x,0,0,0,
		0,Scale.y,0,0,
		0,0,Scale.z,0,
		0,0,0,1,
	};
	return matScale;
}
Matrix4 GameScene::matRot(Vector3 Rot)
{
	Matrix4 matRotZ =
	{
		1,0,0,0,
		0,cos(Rot.z),sin(Rot.z),0,
		0,-sin(Rot.z),cos(Rot.z),0,
		0,0,0,1,
	};
	Matrix4 matRotY =
	{
		cos(Rot.y),0,-sin(Rot.y),0,
		0,1,0,0,
		sin(Rot.y),0,cos(Rot.y),0,
		0,0,0,1,
	};
	Matrix4 matRotX =
	{
		cos(Rot.x),sin(Rot.x),0,0,
		-sin(Rot.x),cos(Rot.x),0,0,
		0,0,1,0,
		0,0,0,1,
	};
	Matrix4 matRot =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1,
	};
	matRot *= matRotX;
	matRot *= matRotY;
	matRot *= matRotZ;
	return matRot;
}
Matrix4 GameScene::matTrams(Vector3 Trams)
{
	Matrix4  matTrams;
	matTrams =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		Trams.x,Trams.y,Trams.z,1,
	};
	return matTrams;
}
void GameScene::mat()
{
	worldTransform_.matWorld_.m[0][0] = 1;
	worldTransform_.matWorld_.m[1][1] = 1;
	worldTransform_.matWorld_.m[2][2] = 1;
	worldTransform_.matWorld_.m[3][3] = 1;
	worldTransform_.matWorld_ *= matScale(worldTransform_.scale_);
	worldTransform_.matWorld_ *= matRot(worldTransform_.rotation_);
	worldTransform_.matWorld_ *= matTrams(worldTransform_.translation_);
	//行列の転送
	worldTransform_.TransferMatrix();
}
