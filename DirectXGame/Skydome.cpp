#include"skydome.h"
#include<assert.h>
#include"affine.h"
void Skydome::Initialize(Model* model) {

	assert(model);

	//3Dƒ‚ƒfƒ‹‚Ì¶¬
	model_ = model;

	worldTransform_.Initialize();

	worldTransform_.scale_ = {100.0f,100.0f,100.0f};
	affine::makeMatIdentity(worldTransform_.matWorld_);
	affine::makeMatScale(worldTransform_.matWorld_, worldTransform_.scale_);
	worldTransform_.TransferMatrix();
}

void Skydome::Update() {

}

void Skydome::Draw(ViewProjection& viewProjection_) {
	model_->Draw(worldTransform_, viewProjection_);
}
