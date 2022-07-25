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
	switch (phase_)
	{
	case Phase::Approach:
		worldTransform_.translation_ += {0.0f, 0.0f, -1.0f};

		affine::makeMatIdentity(worldTransform_.matWorld_);
		affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
		affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
		//‹K’è‚ÌˆÊ’u‚É“ž’B‚µ‚½‚ç—£’E
		if (worldTransform_.translation_.z < 0.0f)
		{
			phase_ = Phase::Leave;
		}
		break;
	case Phase::Leave:
		worldTransform_.translation_ += {-1.0f, -1.0f, -1.0f};

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