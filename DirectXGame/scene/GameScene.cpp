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
	//大元
	worldTransforms_[PartId::kRoot].Initialize();
	//脊椎
	worldTransforms_[PartId::kSpine].Initialize();
	worldTransforms_[PartId::kSpine].parent_ = &worldTransforms_[PartId::kRoot];
	worldTransforms_[PartId::kSpine].translation_ = { 0,4.5f,0 };
	//胸
	worldTransforms_[PartId::kChest].Initialize();
	worldTransforms_[PartId::kChest].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kChest].translation_ = { 0,-2.0f,0 };
	//頭
	worldTransforms_[PartId::kHead].Initialize();
	worldTransforms_[PartId::kHead].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kHead].translation_ = { 0,3.0f,0 };
	//左腕
	worldTransforms_[PartId::kArmL].Initialize();
	worldTransforms_[PartId::kArmL].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmL].translation_ = { 3.0f,0,0 };
	//右腕
	worldTransforms_[PartId::kArmR].Initialize();
	worldTransforms_[PartId::kArmR].parent_ = &worldTransforms_[PartId::kChest];
	worldTransforms_[PartId::kArmR].translation_ = { -3.0f,0,0 };
	//尻
	worldTransforms_[PartId::kHip].Initialize();
	worldTransforms_[PartId::kHip].parent_ = &worldTransforms_[PartId::kSpine];
	worldTransforms_[PartId::kHip].translation_ = { 0,-5.0f,0 };
	//左足
	worldTransforms_[PartId::kLegL].Initialize();
	worldTransforms_[PartId::kLegL].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegL].translation_ = { 2.0f,-2.0f,0 };
	//右足
	worldTransforms_[PartId::kLegR].Initialize();
	worldTransforms_[PartId::kLegR].parent_ = &worldTransforms_[PartId::kHip];
	worldTransforms_[PartId::kLegR].translation_ = { -2.0f,-2.0f,0 };
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
		worldTransforms_[PartId::kRoot].translation_ += move;
		Vector3 Chestrot = { 0,0,0 };
		if (input_->PushKey(DIK_U)) {
			worldTransforms_[PartId::kChest].rotation_.y += 0.001 * (180 / PI);
		}
		else if (input_->PushKey(DIK_I)) {
			worldTransforms_[PartId::kChest].rotation_.y -= 0.001 * (180 / PI);
		}
		if (input_->PushKey(DIK_J)) {
			worldTransforms_[PartId::kHip].rotation_.y += 0.001 * (180 / PI);
		}
		else if (input_->PushKey(DIK_K)) {
			worldTransforms_[PartId::kHip].rotation_.y -= 0.001 * (180 / PI);
		}


		//デバック用表示
		debugText_->SetPos(50, 50);
		debugText_->Printf(
			"親:(%f,%f,%f)", worldTransforms_[PartId::kRoot].translation_.x, worldTransforms_[PartId::kRoot].translation_.y, worldTransforms_[PartId::kRoot].translation_.z);
		for (int i = 0; i < kNumPartId; i++)
		{
			Matrix4  matTrams = MathUtility::Matrix4Identity();
			matTrams.m[3][0] = worldTransforms_[i].translation_.x;
			matTrams.m[3][1] = worldTransforms_[i].translation_.y;
			matTrams.m[3][2] = worldTransforms_[i].translation_.z;
			Matrix4 matrot = MathUtility::Matrix4Identity();
			matrot.m[0][0] = cos(worldTransforms_[i].rotation_.y);
			matrot.m[0][2] = -sin(worldTransforms_[i].rotation_.y);
			matrot.m[2][0] = sin(worldTransforms_[i].rotation_.y);
			matrot.m[2][2] = cos(worldTransforms_[i].rotation_.y);
			Matrix4 matrix= MathUtility::Matrix4Identity();
			matrix *= matTrams;
			matrix *= matrot;
			worldTransforms_[i].matWorld_ = matrix;
			if (i != 0) {
				worldTransforms_[i].matWorld_ *= worldTransforms_[i].parent_->matWorld_;
			}
			worldTransforms_[i].TransferMatrix();
		}
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
	for (int i = 0; i < kNumPartId; i++)
	{
		if (i < 2) { continue; }
		model_->Draw(worldTransforms_[i], viewProjection_, textureHandle_);
	}
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
