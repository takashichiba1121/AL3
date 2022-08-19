#include"RailCamera.h"
#include"affine.h"
#include<assert.h>
void RailCamera::Initialize(Vector3 trans, Vector3 rot) {

	worldTransform_.Initialize();
	//ワールドトランスフォームの初期設定
	this->worldTransform_.translation_ = trans;
	this->worldTransform_.rotation_ = rot;
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

	debugText_ = DebugText::GetInstance();

	input_ = Input::GetInstance();
}
void RailCamera::Update() {
	//プレイヤー旋回処理
	Vector3 rot = { 0,0,0 };
	Vector3 move = { 0,0,0 };
	if (input_->PushKey(DIK_Z))
	{
		move.z = 0.1;
	}
	if (input_->PushKey(DIK_X))
	{
		move.z = -0.1;
	}

	worldTransform_.translation_ += move;
	worldTransform_.rotation_.y += rot.y * (180 / PI);

	affine::makeMatIdentity(worldTransform_.matWorld_);
	affine::makeMatRot(worldTransform_.matWorld_, worldTransform_.rotation_);
	affine::makeMatTrans(worldTransform_.matWorld_, worldTransform_.translation_);
	worldTransform_.TransferMatrix();
	viewProjection_.eye = worldTransform_.translation_;
	//ワールド前方ベクトル
	Vector3 forward(0, 0, 1);
	//レールカメラの回転を反映
	forward = affine::MatVector(worldTransform_.matWorld_, forward);
	//視点から前方に適当な距離進んだ位置が注視点
	forward += viewProjection_.eye;
	viewProjection_.target = forward;
	//ワールド上方ベクトル
	Vector3 up(0, 1, 0);
	//レールカメラの回転を反映(レールカメラの上方ベクトル)
	viewProjection_.up = affine::MatVector(worldTransform_.matWorld_, up);
	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	debugText_->SetPos(0.0f, 0.0f);
	debugText_->Printf("eye=%f,%f,%f", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);
}