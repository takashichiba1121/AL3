#include "GameScene.h"
#include "TextureManager.h"
#include"AxisIndicator.h"
#include <cassert>
#include<cmath>
#include<random>

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

	////カメラ垂直方向視野角を設定
	//viewProjection_.fovAngleY = 10.0f * (PI / 180);

	////アスペクト比を設定
	//viewProjection_.aspectRatio = 1.0f;

	////ニアクリップ距離を設定
	//viewProjection_.nearZ = 52.0f;
	////ファークリップ距離を設定
	//viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロダクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);


	////乱数エンジンを渡し、指定範囲からランダムな数値を得る
	/*for (WorldTransform& worldTransform : worldTransforms_) {

		worldTransform.matWorld_.m[0][0] = 1;
		worldTransform.matWorld_.m[1][1] = 1;
		worldTransform.matWorld_.m[2][2] = 1;
		worldTransform.matWorld_.m[3][3] = 1;
	}*/
	//親（0番）
	worldTransforms_[0].Initialize();
	//子（1番）
	worldTransforms_[1].Initialize();
	worldTransforms_[1].translation_ = { 0,4.5f,0 };
	worldTransforms_[1].parent_ = &worldTransforms_[0];
}

void GameScene::Update() {
	debugCamera_->Update();
	////Fov変更処理
	//{
	//	//上キーで視野角が広がる
	//	if (input_->PushKey(DIK_UP))
	//	{
	//		viewProjection_.fovAngleY += 0.01;
	//		if (viewProjection_.fovAngleY > 3.14)
	//		{
//			viewProjection_.fovAngleY = 3.14;
//		}
//	}
//	else if (input_->PushKey(DIK_DOWN)) {
//		viewProjection_.fovAngleY -= 0.01;
//		if (viewProjection_.fovAngleY < 0.01)
//		{
//			viewProjection_.fovAngleY = 0.01;
//		}
//	}
//	viewProjection_.UpdateMatrix();

//	debugText_->SetPos(50, 110);
//	debugText_->Printf("fovAngleY(Degree):%f", viewProjection_.fovAngleY * 180 / PI);
//}
////クリップ距離変更処理
//{
//	//上下キーでニアクリップ距離を増減
//	if (input_->PushKey(DIK_UP)) {
//		viewProjection_.nearZ += 0.1;
//	}
//	else if (input_->PushKey(DIK_DOWN)) {
//		viewProjection_.nearZ -= 0.1;
//	}
//	//行列再計算
//	viewProjection_.UpdateMatrix();

//	//デバック用表示
//	debugText_->SetPos(50, 130);
//	debugText_->Printf("nearZ:%f",
//		viewProjection_.nearZ);
//}
//{
//	//視点の移動ベクトル
//	Vector3 move = { 0,0,0 };

//	//視点の移動速さ
//	const float kEyeSpeed = 0.2f;

//	//押した方向で移動ベクトルを変更
//	if (input_->PushKey(DIK_W)) {
//		move.z += kEyeSpeed;
//	}
//	else if (input_->PushKey(DIK_S)) {
//		move.z -= kEyeSpeed;
//	}

//	//視点移動（ベクトルの加算）
//	viewProjection_.eye += move;

//	//行列の再計算
//	viewProjection_.UpdateMatrix();
//}
//{
//	//注視点の移動ベクトル
//	Vector3 move = { 0,0,0 };

//	//注視点の移動速さ
//	const float kTargetSpeed = 0.2f;

//	//押した方向で移動ベクトルを変更
//	if (input_->PushKey(DIK_LEFT)){
//		move.x -= kTargetSpeed;
//	}else if(input_->PushKey(DIK_RIGHT)) {
//		move.x += kTargetSpeed;
//	}
//	//注視点移動（ベクトルの加算）
//	viewProjection_.target += move;
//	//行列の再計算
//	viewProjection_.UpdateMatrix();
//}
////上方向回転処理
//{
//	//上方向の回転速さ[ラジアン/frame]
//	const float kUpRotSpeed = 0.05f;

//	//押した方向で移動ベクトルを変更
//	if (input_->PushKey(DIK_SPACE)) {
//		viewAngle += kUpRotSpeed;
//		//2πを超えたら0に戻す
//		viewAngle = fmodf(viewAngle, PI * 2.0f);
//	}
//	//上方向ベクトルを計算（半径1の演習場の座標）
//	viewProjection_.up = { cosf(viewAngle),sinf(viewAngle),0.0f };

//	//行列の再計算
//	viewProjection_.UpdateMatrix();
//}
////デバック用表示
//debugText_->SetPos(50, 50);
//debugText_->Printf(
//	"eye(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
//debugText_->SetPos(50, 70);
//debugText_->Printf(
//	"target(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
//	viewProjection_.target.z);
//debugText_->SetPos(50,90);
//debugText_->Printf(
//	"up(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
//キャラクター移動処理
	{
		//キャラクターの移動ベクトル
		Vector3 move = { 0,0,0 };

		if (input_->PushKey(DIK_LEFT))
		{
			move.x = -1;
		}
		if (input_->PushKey(DIK_RIGHT))
		{
			move.x = 1;
		}
		worldTransforms_[0].translation_ += move;
		Matrix4  matTrams = MathUtility::Matrix4Identity();
		matTrams =
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			move.x,move.y,move.z,1,
		};
		worldTransforms_[0].matWorld_ *= matTrams;
		worldTransforms_[0].TransferMatrix();


		//デバック用表示
		debugText_->SetPos(50, 50);
		debugText_->Printf(
			"親:(%f,%f,%f)", worldTransforms_[0].translation_.x, worldTransforms_[0].translation_.y, worldTransforms_[0].translation_.z);
	}
	//子の更新
	{
		worldTransforms_[1].matWorld_ *= worldTransforms_[1].parent_->matWorld_;
		worldTransforms_[1].TransferMatrix();
	}
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
	model_->Draw(worldTransforms_[0], viewProjection_, textureHandle_);
	model_->Draw(worldTransforms_[1], viewProjection_, textureHandle_);
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
