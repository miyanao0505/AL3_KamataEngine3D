#include "PlayerBullet.h"
#include <cassert>
#include "TextureManager.h"
#include "Enemy.h"

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, uint32_t type) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("cube/cube.jpg");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	worldTransform_.scale_ = {0.5f, 0.5f, 0.5f};

	// 引数で受け取った速度をメンバ変数に代入
	velocity_ = velocity;

	// タイプをセット
	SetType(type);

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributePlayer);
}

void PlayerBullet::Update() {

	//type_->Update(velocity_);

	// 座標を移動させる(1フレーム分の移動量を足し込む)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, velocity_);

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

/// ワールド座標を取得
Vector3 PlayerBullet::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

// 衝突を検出したら呼び出されるコールバック関数
void PlayerBullet::OnCollision() 
{ 
	isDead_ = true;
}

// タイプをセット
void PlayerBullet::SetType(uint32_t type)
{
	/*if (type == 0) {
		type_ = std::move(std::make_unique<PlayerBulletTypeNormal>(this));
	} else if (type == 1) {
		type_ = std::move(std::make_unique<PlayerBulletTypeHoming>(this));
	}*/
}

/// 親となるワールドトランスフォームをセット
void PlayerBullet::SetParent(const WorldTransform* parent)
{ 
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) 
{ 
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

/// PlayerBulletTypeNormalクラスの実装
//PlayerBulletTypeNormal::PlayerBulletTypeNormal(PlayerBullet* playerBullet) : BasePlayerBulletType("Type Normal", playerBullet) {
//	
//}
//
//// 更新
//void PlayerBulletTypeNormal::Update(const Vector3& velocity) {
//	
//}
//
///// PlayerBulletTypeHomingクラスの実装
//PlayerBulletTypeHoming::PlayerBulletTypeHoming(PlayerBullet* playerBullet) : BasePlayerBulletType("Type Homing", playerBullet) {
//
//}
//
//// 更新
//void PlayerBulletTypeHoming::Update(const Vector3& velocity) { 
//
//	if (enemy_ == nullptr)
//	{
//		playerBullet_->SetType(0);
//		return;
//	}
//	// 弾の速度
//	const float kBulletSpeed = 1.0f;
//
//	Vector3 playerBulletPos = playerBullet_->GetWorldPosition();
//	Vector3 enemyPos = enemy_->GetWorldPosition();
//	Vector3 toEnemy = MyTools::Subtract(enemyPos, playerBulletPos);
//	
//	// ベクトルを正規化する
//	toEnemy = MyTools::Normalize(toEnemy);
//	Vector3 NVelocity = MyTools::Normalize(velocity);
//
//	// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
//	playerBullet_->SetVelocity(MyTools::Multiply(kBulletSpeed, MyTools::Slerp(velocity, toEnemy, 0.08f)));
//	// 進行方向に見た目の回転を合わせる
//	float velocityXZ = MyTools::Length(MyTools::Subtract(NVelocity, Vector3{0.0f, NVelocity.y, 0.0f}));
//
//	playerBullet_->SetRotate({std::atan2(-NVelocity.y, velocityXZ), std::atan2(NVelocity.x, NVelocity.z), 0.0f});
//}