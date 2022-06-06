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
#include "PLayer.h"
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
	Vector3 move = { 0,0,0 };

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
		move.y= 1;
	}
	if (input_->PushKey(DIK_DOWN))
	{
		move.y = -1;
	}

	worldTransform_.translation_ += move;

	//移動限界座標
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.0f;

	//範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

	Matrix4  matTrams = MathUtility::Matrix4Identity();
	matTrams.m[3][0] = worldTransform_.translation_.x;
	matTrams.m[3][1] = worldTransform_.translation_.y;
	matTrams.m[3][2] = worldTransform_.translation_.z;
	worldTransform_.matWorld_ = matTrams;
	worldTransform_.TransferMatrix();

	debugText_->SetPos(10, 10);
	debugText_->Printf("座標:%f,%f,%f", worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z);
}

void Player::Draw(ViewProjection& viewProjection_){
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
}
