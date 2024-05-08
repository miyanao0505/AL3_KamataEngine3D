#include "Enemy.h"
#include <cassert>
#include "TextureManager.h"

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// 引数で受け取った速度をメンバ変数に代入
	approachVelocity_ = velocity;
	leaveVelocity_ = {-0.1f, 0.1f, -0.1f};

	phase_ = Phase::Approach;
}

void (Enemy::*Enemy::phaseTable_[])(){
    &Enemy::ApproachUpdate, // 要素番号0
    &Enemy::LeaveUpdate,    // 要素番号1
};

void Enemy::Update() {

	/*switch (phase_) { 
	case Phase::Approach:
	default:
		ApproachUpdate();
		break;
	case Phase::Leave:
		LeaveUpdate();
		break;
	}*/

	// メンバ関数ポインタに入っている関数を呼び出す
	(this->*phaseTable_[static_cast<size_t>(phase_)])();

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	
}

void Enemy::ApproachUpdate()
{
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, approachVelocity_);
	// 規定の位置に到達したら離脱
	if (worldTransform_.translation_.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeaveUpdate()
{
	// 移動(ベクトルを加算)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, leaveVelocity_);
}

void Enemy::Draw(const ViewProjection& viewProjection) {
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
