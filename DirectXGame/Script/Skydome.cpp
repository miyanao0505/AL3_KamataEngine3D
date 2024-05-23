#include "Skydome.h"
#include "TextureManager.h"

void Skydome::Initialize(Model* model, const Vector3& position) {
	// NULLポインタチェック
	assert(model);

	// 3Dモデルの生成
	model_ = model;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Skydome::Update()
{
	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw(const ViewProjection& viewProjection) 
{
	// 3Dモデル描画
	model_->Draw(worldTransform_, viewProjection);
}
