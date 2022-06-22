#include"Enemy.h"
#include<assert.h>
#include"affine.h"
void Enemy::Initialize(Model* model)
{
	assert(model);

	this->model_ = model;
	textureHandle_= TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = { 10.0f,10.0f,50.0f };
}

void Enemy::Update()
{
	worldTransform_.translation_ += {0.0f,0.0f,-1.0f};

	affine::makeMatIdentity(worldTransform_.matWorld_);
	affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
	affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
}
void Enemy::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}