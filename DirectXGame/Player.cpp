#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	// 3Dモデルの生成
	model_ = model;

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw(ViewProjection& viewProjevtion) {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, viewProjevtion, textureHandle_);
}
