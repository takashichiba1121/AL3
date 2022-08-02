#include"Enemy.h"
#include"PLayer.h"
#include<assert.h>
#include"affine.h"
#include<cmath>
void Enemy::Initialize(Model* model)
{
	assert(model);

	this->model_ = model;
	textureHandle_ = TextureManager::Load("black.png");

	worldTransform_.Initialize();

	worldTransform_.translation_ = { 10.0f,10.0f,50.0f };

	//Fire();

	//�ڋ߃t�F�[�Y������
	ApproachInutialize();
}

void Enemy::Fire()
{
	assert(player_);
	//�e�̑��x
	const float kBulletSpeed = -1.0f;
	Vector3 playerPos = player_->GetworldPosition();
	Vector3 enemyPos=worldTransform_.translation_;
	Vector3 vector = { playerPos.x - enemyPos.x,playerPos.y - enemyPos.y,playerPos.z - enemyPos.z };
	float len = sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
	if (len != 0)
	{
		vector /= len;
	}
	Vector3 velocity=vector;

	velocity = affine::MatVector(worldTransform_.matWorld_, velocity);

	//�e�̐������A������
	std::unique_ptr<EnemyBullet> newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);

	//�e�̓o�^����
	bullets_.push_back(std::move(newBullet));
}

void Enemy::Update()
{

	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->IsDead();
		});
	switch (phase_)
	{
	case Phase::Approach:
		worldTransform_.translation_ += {0.0f, 0.0f, -0.1f};

		affine::makeMatIdentity(worldTransform_.matWorld_);
		affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
		affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
		worldTransform_.TransferMatrix();
		//���˃^�C�}�[�J�E���g�_�E��
		fireTimer--;
		//�w�莞�Ԃ�r�B����
		if (fireTimer <= 0) {
			//�e�𔭎�
			Fire();
			//���˃^�C�}�[��������
			fireTimer = kFireInterval;
		}
		//�K��̈ʒu�ɓ��B�����痣�E
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
	//�e�X�V
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Update();
	}
}
void Enemy::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	//�e�`��
	for (std::unique_ptr<EnemyBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}
void Enemy::ApproachInutialize()
{
	//���˃^�C�}�[��������
	fireTimer = kFireInterval;
}