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

	//カメラ視点座標を設定
	viewProjection_.eye = { 0,0,-10 };

	//カメラ注視点座標を設定
	viewProjection_.target = { 10,0,0 };

	//カメラ上方向ベクトルを設定（右上45度指定）
	viewProjection_.up = { cosf(PI / 4.0f),sinf(PI / 4.0f),0.0f };

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	//デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	//軸方向表示が参照するビュープロダクションを指定する（アドレス渡し）
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);

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
	//乱数シード生成器
	std::random_device seed_gen;
	//メルセンヌ・ツイスターの乱数エンジン
	std::mt19937_64 engine(seed_gen());
	//乱数範囲の指定
	std::uniform_real_distribution<float> TransX(-10.0, +10.0);
	std::uniform_real_distribution<float> TransY(-10.0, +10.0);
	std::uniform_real_distribution<float> TransZ(-10.0, +10.0);
	std::uniform_real_distribution<float> RotX(0, PI);
	std::uniform_real_distribution<float> RotY(0, PI);
	std::uniform_real_distribution<float> RotZ(0, PI);

	//乱数エンジンを渡し、指定範囲からランダムな数値を得る
	for (WorldTransform& worldTransform : worldTransforms_) {
		//ワールドトランスフォームの初期化
		worldTransform.Initialize();

		//X,Y,Z 方向のスケーリングを設定
		worldTransform.scale_ = { 1,1,1 };
		worldTransform.rotation_ = { RotX(engine),RotY(engine),RotZ(engine) };
		worldTransform.translation_ = { TransX(engine),TransY(engine),TransZ(engine) };
		mat(worldTransform);
	}
}

void GameScene::Update() {
	debugCamera_->Update();
	{
		//視点の移動ベクトル
		Vector3 move = { 0,0,0 };

		//視点の移動速さ
		const float kEyeSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_W)) {
			move.z += kEyeSpeed;
		}
		else if (input_->PushKey(DIK_S)) {
			move.z -= kEyeSpeed;
		}

		//視点移動（ベクトルの加算）
		viewProjection_.eye += move;

		//行列の再計算
		viewProjection_.UpdateMatrix();
	}
	{
		//注視点の移動ベクトル
		Vector3 move = { 0,0,0 };

		//注視点の移動速さ
		const float kTargetSpeed = 0.2f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_LEFT)) {
			move.x -= kTargetSpeed;
		}
		else if (input_->PushKey(DIK_RIGHT)) {
			move.x += kTargetSpeed;
		}
		//注視点移動（ベクトルの加算）
		viewProjection_.target += move;
		//行列の再計算
		viewProjection_.UpdateMatrix();
	}
	//上方向回転処理
	{
		//上方向の回転速さ[ラジアン/frame]
		const float kUpRotSpeed = 0.05f;

		//押した方向で移動ベクトルを変更
		if (input_->PushKey(DIK_SPACE)) {
			viewAngle += kUpRotSpeed;
			//2πを超えたら0に戻す
			viewAngle = fmodf(viewAngle, PI * 2.0f);
		}
		//上方向ベクトルを計算（半径1の演習場の座標）
		viewProjection_.up = { cosf(viewAngle),sinf(viewAngle),0.0f };

		//行列の再計算
		viewProjection_.UpdateMatrix();
	}
	//デバック用表示
	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"eye(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"target(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y,
		viewProjection_.target.z);
	debugText_->SetPos(50, 90);
	debugText_->Printf(
		"up(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);
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
	for (WorldTransform& worldTransform : worldTransforms_)
	{
		model_->Draw(worldTransform, viewProjection_, textureHandle_);
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
	Matrix4  matTrams = MathUtility::Matrix4Identity();
	matTrams =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		Trams.x,Trams.y,Trams.z,1,
	};
	return matTrams;
}
void GameScene::mat(WorldTransform worldTransform)
{
	worldTransform.matWorld_.m[0][0] = 1;
	worldTransform.matWorld_.m[1][1] = 1;
	worldTransform.matWorld_.m[2][2] = 1;
	worldTransform.matWorld_.m[3][3] = 1;
	worldTransform.matWorld_ *= matScale(worldTransform.scale_);
	worldTransform.matWorld_ *= matRot(worldTransform.rotation_);
	worldTransform.matWorld_ *= matTrams(worldTransform.translation_);
	//行列の転送
	worldTransform.TransferMatrix();
}
