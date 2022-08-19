#pragma once
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
#include"PlayerBullet.h"
#include<memory>
#include<list>

/// <summary>
/// ���L����
/// </summary>
class Player {

public:
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Model* model, uint32_t textureHandle);
	/// <summary>
	/// �X�V
	/// </summary>
	void Update(ViewProjection viewProjection);

	///<summary>
	///�U��
	///</summary>
	void Attack();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(ViewProjection& viewProjection_);

	//���[���h���W���擾
	Vector3 GetworldPosition();

	//�Փ˂����o������Ăяo���ꂽ��R�[���o�b�N�֐�
	void OnCollision();

	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets() { return bullets_; }

	void SetParent(WorldTransform* worldTransform) { worldTransform_.parent_ = worldTransform; };

	/// <summary>
	/// UI�`��
	/// </summary>
	void DrawUI();
private:
	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;

	Input* input_ = nullptr;

	DebugText* debugText_ = nullptr;

	//�e
	std::list<std::unique_ptr<PlayerBullet>>bullets_;

	float PI = 3.1415927f;

	//3D���e�B�N���p���[���h�g�����X�t�H�[��
	WorldTransform worldTransform3DReticle_;

	//2D���e�B�N���p�X�v���C�g
	std::unique_ptr<Sprite> sprite2DReticle_;
};