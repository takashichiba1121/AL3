#include"PLayer.h"
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
#include<cassert>
#include"affine.h"
#include"MathUtility.h"
void Player::Initialize(Model* model, uint32_t textureHandle) {
	assert(model);

	this->model_ = model;
	this->textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_ = Vector3(0.0f, 0.0f, 50.0f);
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dレティクルのワールドトランスフォームの初期化
	worldTransform3DReticle_.Initialize();

	//レティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("2D.png");

	//スプライト生成
	sprite2DReticle_.reset(Sprite::Create(textureReticle, Vector2{ 640,360 }, Vector4{ 1,1,1,1 }, Vector2(0.5, 0.5)));
}

void Player::Update(ViewProjection viewProjection) {

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	Vector3 move = { 0,0,0 };
	Vector3 rot = { 0,0,0 };
	//ゲームパッドの状態を得る変数（XINPUT）
	XINPUT_STATE joyState;
	//ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * 0.1;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * 0.1;
	}
	////プレイヤー移動処理
	//if (input_->PushKey(DIK_LEFT))
	//{
	//	move.x = -0.1;
	//}
	//if (input_->PushKey(DIK_RIGHT))
	//{
	//	move.x = 0.1;
	//}
	//if (input_->PushKey(DIK_UP))
	//{
	//	move.y = 0.1;
	//}
	//if (input_->PushKey(DIK_DOWN))
	//{
	//	move.y = -0.1;
	//}
	if (input_->PushKey(DIK_A))
	{
		rot.y = 0.001;
	}
	if (input_->PushKey(DIK_D))
	{
		rot.y = -0.001;
	}

	worldTransform_.translation_ += move;
	worldTransform_.rotation_.y += rot.y * (180 / PI);

	//移動限界座標
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.0f;

	//範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);




	affine::makeMatIdentity(worldTransform_.matWorld_);
	affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
	affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	worldTransform_.TransferMatrix();

	//キャラクター攻撃処理
	Attack();

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	////自機のワールド座標から3Dレティクルのワールド座標を計算
	//{
	//	//自機から3Dレティクルへの距離
	//	const float kDistancePlayerTo3DReticle = 50.0f;
	//	//自機から3Dレティクルへのオフセット(Z+向き)
	//	Vector3 offset = { 0,0,1.0f };
	//	//自機のワールド行列の回転を反映
	//	offset = affine::MatVector(worldTransform_.matWorld_, offset);
	//	//ベクトルの長さを整える
	//	float len = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
	//	if (len != 0)
	//	{
	//		offset /= len;
	//	}
	//	offset *= kDistancePlayerTo3DReticle;
	//	worldTransform3DReticle_.translation_ = offset;
	//	affine::makeMatIdentity(worldTransform3DReticle_.matWorld_);
	//	affine::makeMatTrans(worldTransform3DReticle_.matWorld_, worldTransform_.translation_);
	//	worldTransform3DReticle_.TransferMatrix();
	//}
	////3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	//{
	//	Vector3 positionReticle = affine::GetWorldTrans(worldTransform3DReticle_.matWorld_);

	//	//ビューポート行列
	//	Matrix4 matViewport = {
	//		640,    0,0,0,
	//		  0, -360,0,0,
	//		  0,    0,1,0,
	//		640,  360,0,1,
	//	};

	//	//ビューポート行列
	//	Matrix4 matViewProjectionViewport;
	//	matViewProjectionViewport = viewProjection.matView;
	//	matViewProjectionViewport *= viewProjection.matProjection;
	//	matViewProjectionViewport *= matViewport;

	//	//ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	//	positionReticle = affine::wdivision(matViewProjectionViewport, positionReticle);

	//	//スプライトのレティクルに座標設定
	//	sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	//}
	//2Dレティクルのスクリーン座標から3Dレティクルのワールド座標を計算
	{
		//POINT mousePosition;
		////マウス座標（スクリーン座標）を取得する
		//GetCursorPos(&mousePosition);

		////クライアントエリア座標に変換する
		//HWND hwnd = WinApp::GetInstance()->GetHwnd();
		//ScreenToClient(hwnd, &mousePosition);

		//sprite2DReticle_->SetPosition(Vector2(mousePosition.x, mousePosition.y));
		
		//スプライトの現在座標を取得
		Vector2 spritePosition = sprite2DReticle_->GetPosition();

		XINPUT_STATE joyState;

		//ジョイスティック状態
		if (Input::GetInstance()->GetJoystickState(0, joyState)) {
			spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
			spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;
			//スプライトの座標変更を反映
			sprite2DReticle_->SetPosition(spritePosition);
		}

		//ビューポート行列
		Matrix4 matViewport = {
			640,    0,0,0,
			  0, -360,0,0,
			  0,    0,1,0,
			640,  360,0,1,
		};

		//ビューポート行列
		Matrix4 matVPV;
		matVPV = viewProjection.matView;
		matVPV *= viewProjection.matProjection;
		matVPV *= matViewport;
		//合成行列の逆行列を計算する
		Matrix4 matInverseVPV = MathUtility::Matrix4Inverse(matVPV);

		//スクリーン座標
		Vector3 posNear = Vector3(spritePosition.x, spritePosition.y, 0);
		Vector3 posFar = Vector3(spritePosition.x, spritePosition.y, 1);

		//スクリーン座標系からワールド座標系へ
		posNear = affine::wdivision(matInverseVPV, posNear);
		posFar = affine::wdivision(matInverseVPV, posFar);

		//マウスレイの方向
		Vector3 mouseDirection = posFar;
		mouseDirection -= posNear;
			float len = sqrt(mouseDirection.x * mouseDirection.x + mouseDirection.y * mouseDirection.y + mouseDirection.z * mouseDirection.z);
		if (len != 0)
		{
			mouseDirection /= len;
		}
		//カメラから照準オブジェクトの距離
		const float kDistanceTestObject = 100.0f;
		Vector3 A = posNear;
		A+= Vector3(mouseDirection.x * kDistanceTestObject, mouseDirection.y * kDistanceTestObject, mouseDirection.z * kDistanceTestObject);
		worldTransform3DReticle_.translation_ = A;
		affine::makeMatIdentity(worldTransform3DReticle_.matWorld_);
		affine::makeMatTrans(worldTransform3DReticle_.matWorld_, worldTransform3DReticle_.translation_);
		worldTransform3DReticle_.TransferMatrix();

		DebugText::GetInstance()->SetPos(20, 200);
		DebugText::GetInstance()->Printf(
			"Mouse ScreenPos:(%d,%d)", spritePosition.x, spritePosition.y);
		DebugText::GetInstance()->SetPos(20, 220);
		DebugText::GetInstance()->Printf("Near:(%f,%f,%f)", posNear.x, posNear.y, posNear.z);
		DebugText::GetInstance()->SetPos(20, 240);
		DebugText::GetInstance()->Printf("Far:(%f,%f,%f)", posFar.x, posFar.y, posFar.z);
		DebugText::GetInstance()->SetPos(20, 260);
		DebugText::GetInstance()->Printf(
			"MouseObject:(%f,%f,%f)", worldTransform3DReticle_.translation_.x, 
			worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	}
	/*debugText_->SetPos(10, 10);
	debugText_->Printf("座標:%f,%f,%f\n回転:%f,%f,%f",
		worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z,
		worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z);*/
}

void Player::Attack() {
	XINPUT_STATE joyState;
	if (!Input::GetInstance()->GetJoystickState(0, joyState))
	{
		return;
	}
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);
		velocity = affine::MatVector(worldTransform_.parent_->matWorld_, velocity);
		velocity = affine::MatVector(worldTransform_.matWorld_, velocity);
		velocity = MathUtility::operator-(affine::GetWorldTrans(worldTransform3DReticle_.matWorld_), affine::GetWorldTrans(worldTransform_.matWorld_));
		float len = sqrt(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
		if (len != 0)
		{
			velocity /= len;
		}
		velocity *= kBulletSpeed;

		//弾の生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		Vector3 playerWolrdPos = worldTransform_.parent_->translation_;
		playerWolrdPos += worldTransform_.translation_;
		Vector3 playerWolrdRot = worldTransform_.parent_->rotation_;
		playerWolrdRot += worldTransform_.rotation_;
		newBullet->Initialize(model_, GetworldPosition(), velocity);

		//弾の登録する
		bullets_.push_back(std::move(newBullet));
	}
}

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_);
	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}
void Player::DrawUI() {
	sprite2DReticle_->Draw();
}
Vector3 Player::GetworldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldpos;
	//ワールド行列の平行移動成分を取得（ワールド座標）
	worldpos.x = worldTransform_.matWorld_.m[3][0];
	worldpos.y = worldTransform_.matWorld_.m[3][1];
	worldpos.z = worldTransform_.matWorld_.m[3][2];
	return worldpos;
}
void Player::OnCollision()
{

}
