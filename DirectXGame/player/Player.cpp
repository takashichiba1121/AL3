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

	//�V���O���g���C���X�^���X���擾����
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	worldTransform_.Initialize();
	worldTransform_.translation_ = Vector3(0.0f, 0.0f, 50.0f);
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3D���e�B�N���̃��[���h�g�����X�t�H�[���̏�����
	worldTransform3DReticle_.Initialize();

	//���e�B�N���p�e�N�X�`���擾
	uint32_t textureReticle = TextureManager::Load("2D.png");

	//�X�v���C�g����
	sprite2DReticle_.reset(Sprite::Create(textureReticle, Vector2{ 640,360 }, Vector4{ 1,1,1,1 }, Vector2(0.5, 0.5)));
}

void Player::Update(ViewProjection viewProjection) {

	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->IsDead();
		});

	Vector3 move = { 0,0,0 };
	Vector3 rot = { 0,0,0 };
	//�v���C���[�ړ�����
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

	//�ړ����E���W
	const float kMoveLimitX = 35.0f;
	const float kMoveLimitY = 19.0f;

	//�͈͂𒴂��Ȃ�����
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);




	affine::makeMatIdentity(worldTransform_.matWorld_);
	affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
	affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	worldTransform_.TransferMatrix();

	//�L�����N�^�[�U������
	Attack();

	//�e�X�V
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	//���@�̃��[���h���W����3D���e�B�N���̃��[���h���W���v�Z
	{
		//���@����3D���e�B�N���ւ̋���
		const float kDistancePlayerTo3DReticle = 50.0f;
		//���@����3D���e�B�N���ւ̃I�t�Z�b�g(Z+����)
		Vector3 offset = { 0,0,1.0f };
		//���@�̃��[���h�s��̉�]�𔽉f
		offset = affine::MatVector(worldTransform_.matWorld_, offset);
		//�x�N�g���̒����𐮂���
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
	//3D���e�B�N���̃��[���h���W����2D���e�B�N���̃X�N���[�����W���v�Z
	{
		Vector3 positionReticle = affine::GetWorldTrans(worldTransform3DReticle_.matWorld_);

		//�r���[�|�[�g�s��
		Matrix4 matViewport = {
			640,    0,0,0,
			  0, -360,0,0,
			  0,    0,1,0,
			640,  360,0,1,
		};

		//�r���[�|�[�g�s��
		Matrix4 matViewProjectionViewport;
		matViewProjectionViewport = viewProjection.matView;
		matViewProjectionViewport *= viewProjection.matProjection;
		matViewProjectionViewport *= matViewport;

		//���[���h���X�N���[�����W�ϊ�(������3D����2D�ɂȂ�)
		positionReticle = affine::wdivision(matViewProjectionViewport,positionReticle);

		//�X�v���C�g�̃��e�B�N���ɍ��W�ݒ�
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));

		debugText_->SetPos(100, 100);
		debugText_->Printf("X:%f,Y:%f", positionReticle.x,positionReticle.y);
	}
	debugText_->SetPos(10, 10);
	debugText_->Printf("���W:%f,%f,%f\n��]:%f,%f,%f",
		worldTransform_.translation_.x, worldTransform_.translation_.y, worldTransform_.translation_.z,
		worldTransform_.rotation_.x, worldTransform_.rotation_.y, worldTransform_.rotation_.z);
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {

		//�e�̑��x
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

		//�e�̐������A������
		std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
		Vector3 playerWolrdPos = worldTransform_.parent_->translation_;
		playerWolrdPos += worldTransform_.translation_;
		Vector3 playerWolrdRot = worldTransform_.parent_->rotation_;
		playerWolrdRot += worldTransform_.rotation_;
		newBullet->Initialize(model_, GetworldPosition(), velocity);

		//�e�̓o�^����
		bullets_.push_back(std::move(newBullet));
	}
}

void Player::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_, textureHandle_);

	model_->Draw(worldTransform3DReticle_, viewProjection_, textureHandle_);
	//�e�`��
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(viewProjection_);
	}
}
void Player::DrawUI() {
	sprite2DReticle_->Draw();
}
Vector3 Player::GetworldPosition()
{
	//���[���h���W������ϐ�
	Vector3 worldpos;
	//���[���h�s��̕��s�ړ��������擾�i���[���h���W�j
	worldpos.x = worldTransform_.matWorld_.m[3][0];
	worldpos.y = worldTransform_.matWorld_.m[3][1];
	worldpos.z = worldTransform_.matWorld_.m[3][2];
	return worldpos;
}
void Player::OnCollision()
{

}
