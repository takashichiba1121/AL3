#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"EnemyBullet.h"
class Player;
///<summary>
///�G
///</summary>
class Enemy {
public:
	void Initialize(Model* model);
	/// <summary>
	/// �X�V
	/// </summary>
	void Update();
	/// <summary>
	/// �`��
	/// </summary>
	void Draw(const ViewProjection& viewProjection);
	///<summary>
	///�e����
	///</summary>
	void Fire();
	void ApproachInutialize();

	void SetPlayer(Player* player) { player_ = player; }

	//�Փ˂����o������Ăяo���ꂽ��R�[���o�b�N�֐�
	void OnCollision();

	//���[���h���W���擾
	Vector3 GetworldPosition();

	const std::list<std::unique_ptr<EnemyBullet>>& GetBullets() { return bullets_; }

	//���ˊԊu
	static const int kFireInterval = 60;
private:
	//���[���h�g�����X�t�H�[��
	WorldTransform worldTransform_;
	//���f��
	Model* model_ = nullptr;
	//�e�N�X�`���n���h��
	uint32_t textureHandle_ = 0u;

	//�e
	std::list<std::unique_ptr<EnemyBullet>>bullets_;

	enum class Phase {
		Approach, //�ڋ߂���
		Leave,    //���E����
	};

	//�t�F�[�Y
	Phase phase_ = Phase::Approach;
	//���˃^�C�}�[
	int32_t fireTimer = 0;
	//���L����
	Player* player_ = nullptr;
};