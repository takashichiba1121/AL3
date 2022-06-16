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
void Player::Initialize(Model* model, uint32_t textureHandle) {
	assert(model);

	this->model_ = model;
	this->textureHandle_ = textureHandle;

	//シングルトンインスタンスを取得する
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.matWorld_.m[0][0] = 1;
	worldTransform_.matWorld_.m[1][1] = 1;
	worldTransform_.matWorld_.m[2][2] = 1;
	worldTransform_.matWorld_.m[3][3] = 1;
	textureHandle_ = TextureManager::Load("mario.jpg");
}

void Player::Update() {

	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	Vector3 move = { 0,0,0 };
	Vector3 rot = { 0,0,0 };
	//プレイヤー移動処理
	if (input_->PushKey(DIK_LEFT))
	{
		move.x = -1;
	}
	if (input_->PushKey(DIK_RIGHT))
	{
		move.x = 1;
	}
	if (input_->PushKey(DIK_UP))
	{
		move.y = 1;
	}
	if (input_->PushKey(DIK_DOWN))
	{
		move.y = -1;
	}
	//プレイヤー旋回処理
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
	worldTransform_.TransferMatrix();

	//キャラクター攻撃処理
	Attack();

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	debugText_->SetPos(10, 10);
	debugText_->Printf("座標:%f,%f,%f\n回転:%f,%f,%f",
		worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z,
		worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z);
}

void Player::Attack(){
	if (input_->TriggerKey(DIK_SPACE)){
		
		//弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, kBulletSpeed);

		velocity = affine::MatVector(worldTransform_.matWorld_, velocity);

		//弾の生成し、初期化
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(model_, worldTransform_.translation_,velocity);

		//弾の登録する
		bullets_.push_back(std::move(newBullet));
	}
}

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}
