#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

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
	//X,Y,Z 方向のスケーリングを設定
	worldTransform_.scale_={ 2,2,2 };
	//スケーリング行列を宣言
	Matrix4 matScale;
	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[0][1] = 0;
	matScale.m[0][2] = 0;
	matScale.m[0][3] = 0;

	matScale.m[1][0] = 0;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[1][2] = 0;
	matScale.m[1][3] = 0;

	matScale.m[2][0] = 0;
	matScale.m[2][1] = 0;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[2][3] = 0;

	matScale.m[3][0] = 0;
	matScale.m[3][1] = 0;
	matScale.m[3][2] = 0;
	matScale.m[3][3] = 1;

	worldTransform_.matWorld_.m[0][0] = 1;
	worldTransform_.matWorld_.m[1][1] = 1;
	worldTransform_.matWorld_.m[2][2] = 1;
	worldTransform_.matWorld_.m[3][3] = 1;
	worldTransform_.matWorld_ *= matScale;

	//行列の転送
	worldTransform_.TransferMatrix();
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
