#include "EnemyBullet.h"
#include <assert.h>
#include "TextureManager.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity)
{
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemybullet.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// Z方向に伸びた形状
	worldTransform_.scale_.x = 0.5f;
	worldTransform_.scale_.y = 0.5f;
	worldTransform_.scale_.z = 3.0f;

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(velocity.x, velocity.z);
	float velocityXZ = MyTools::Length(MyTools::Subtract(velocity, Vector3{0.0f, velocity.y, 0.0f}));
	// X軸周り角度(θx)
	worldTransform_.rotation_.x = std::atan2(-velocity.y, velocityXZ);
}

void EnemyBullet::Update()
{
	// 座標を移動させる(1フレーム分の移動量を足し込む)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, velocity_);

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) 
{
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
