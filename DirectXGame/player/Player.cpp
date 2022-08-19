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
	//プレイヤー移動処理
	if (input_->PushKey(DIK_LEFT))
	{
		move.x = -0.1;
	}
	if (input_->PushKey(DIK_RIGHT))
	{
		move.x = 0.1;
	}
	if (input_->PushKey(DIK_UP))
	{
		move.y = 0.1;
	}
	if (input_->PushKey(DIK_DOWN))
	{
		move.y = -0.1;
	}
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

	//自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		//自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		//自機から3Dレティクルへのオフセット(Z+向き)
		Vector3 offset = { 0,0,1.0f };
		//自機のワールド行列の回転を反映
		offset = affine::MatVector(worldTransform_.matWorld_, offset);
		//ベクトルの長さを整える
		float len = sqrt(offset.x * offset.x + offset.y * offset.y + offset.z * offset.z);
		if (len != 0)
		{
			offset /= len;
		}
		offset *= kDistancePlayerTo3DReticle;
		worldTransform3DReticle_.translation_ = offset;
		affine::makeMatIdentity(worldTransform3DReticle_.matWorld_);
		affine::makeMatTrans(worldTransform3DReticle_.matWorld_, worldTransform_.translation_);
		worldTransform3DReticle_.TransferMatrix();
	}
	//3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	{
		Vector3 positionReticle = affine::GetWorldTrans(worldTransform3DReticle_.matWorld_);

		//ビューポート行列
		Matrix4 matViewport = {
			640,    0,0,0,
			  0, -360,0,0,
			  0,    0,1,0,
			640,  360,0,1,
		};

		//ビューポート行列
		Matrix4 matViewProjectionViewport;
		matViewProjectionViewport = viewProjection.matView;
		matViewProjectionViewport *= viewProjection.matProjection;
		matViewProjectionViewport *= matViewport;

		//ワールド→スクリーン座標変換(ここで3Dから2Dになる)
		positionReticle = affine::wdivision(matViewProjectionViewport,positionReticle);

		//スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

		debugText_->SetPos(100, 100);
		debugText_->Printf("X:%f,Y:%f", positionReticle.x,positionReticle.y);
	}
	debugText_->SetPos(10, 10);
	debugText_->Printf("座標:%f,%f,%f\n回転:%f,%f,%f",
		worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z,
		worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z);
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

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
