#include "EnemyBullet.h"
#include <assert.h>
#include "TextureManager.h"
#include "Player.h"

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

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

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
	// 敵弾から自キャラへのベクトルを計算
	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 enemyBulletPos = GetWorldPosition();
	Vector3 toPlayer = MyTools::Subtract(playerPos, enemyBulletPos);

	// ベクトルを正規化する
	toPlayer = MyTools::Normalize(toPlayer);
	Vector3 velocity = MyTools::Normalize(velocity_);

	// 球面線形補間により、今の速度と自キャラへのベクトルを内挿し、新たな速度とする
	velocity_ = MyTools::Multiply(speed_, MyTools::Slerp(velocity_, toPlayer, 0.08f));

	// 進行方向に見た目の回転を合わせる
	// Y軸周り角度(θy)
	worldTransform_.rotation_.y = std::atan2(velocity.x, velocity.z);
	float velocityXZ = MyTools::Length(MyTools::Subtract(velocity, Vector3{0.0f, velocity.y, 0.0f}));
	// X軸周り角度(θx)
	worldTransform_.rotation_.x = std::atan2(-velocity.y, velocityXZ);

	// 座標を移動させる(1フレーム分の移動量を足し込む)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, velocity_);

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	// 時間経過でデス
	/*if (--deathTimer_ <= 0) {
		isDead_ = true;
	}*/
}

/// ワールド座標を取得
Vector3 EnemyBullet::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

// 衝突を検出したら呼び出されるコールバック関数
void EnemyBullet::OnCollision() 
{ 
	isDead_ = true;
}

void EnemyBullet::Draw(const ViewProjection& viewProjection) 
{
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}
