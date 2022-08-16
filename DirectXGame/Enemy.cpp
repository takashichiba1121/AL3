#include"Enemy.h"
#include"PLayer.h"
#include<assert.h>
#include"affine.h"
#include"GameScene.h"
#include<cmath>
void Enemy::Initialize(Model* model, Vector3 translation)
{
	assert(model);

	this->model_ = model;
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = translation;

	//Fire();

	//接近フェーズ初期化
	ApproachInutialize();
}

void Enemy::Fire()
{
	assert(player_);
	assert(gameScene_);
	//弾の速度
	const float kBulletSpeed = 0.5f;
	Vector3 playerPos = player_->GetworldPosition();
	Vector3 enemyPos=worldTransform_.translation_;
	Vector3 vector = { playerPos.x - enemyPos.x,playerPos.y - enemyPos.y,playerPos.z - enemyPos.z };
	float len = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if (len != 0)
	{
		vector /= len;
	}
	vector*= kBulletSpeed;
	Vector3 velocity=vector;

	velocity = affine::MatVector(worldTransform_.matWorld_, velocity);

	//弾の生成し、初期化
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	//弾の登録する
	gameScene_->AddEnemyBullet(std::move(newBullet));
}

void Enemy::Update()
{
	switch (phase_)
	{
	case Phase::Approach:
		worldTransform_.translation_ += {0.0f, 0.0f, -0.1f};

		affine::makeMatIdentity(worldTransform_.matWorld_);
		affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
		affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
		//発射タイマーカウントダウン
		fireTimer--;
		//指定時間にr達した
		if (fireTimer <= 0) {
			//弾を発射
			Fire();
			//発射タイマーを初期化
			fireTimer = kFireInterval;
		}
		//規定の位置に到達したら離脱
		if (worldTransform_.translation_.z < 0.0f)
		{
			phase_ = Phase::Leave;
		}
		break;
	case Phase::Leave:
		worldTransform_.translation_ += {-0.1f, -0.1f, -0.1f};

		affine::makeMatIdentity(worldTransform_.matWorld_);
		affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
		affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
		break;
	}
}
void Enemy::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
void Enemy::ApproachInutialize()
{
	//発射タイマーを初期化
	fireTimer = kFireInterval;
}
void Enemy::OnCollision()
{
	isDead_ = true;
}
Vector3 Enemy::GetworldPosition()
{
	//ワールド座標を入れる変数
	Vector3 worldpos;
	//ワールド行列の平行移動成分を取得（ワールド座標）
	worldpos.x = worldTransform_.translation_.x;
	worldpos.y = worldTransform_.translation_.y;
	worldpos.z = worldTransform_.translation_.z;

	return worldpos;
}