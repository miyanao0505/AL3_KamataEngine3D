#include "Enemy.h"
#include <cassert>
#include "TextureManager.h"

/// Enemyクラスの実装
Enemy::~Enemy() 
{ 
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
}

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 初期座標を保持
	positinInitialize_ = position;

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// 引数で受け取った速度をメンバ変数に代入
	approachVelocity_ = velocity;
	leaveVelocity_ = {-0.1f, 0.1f, -0.1f};
	
	// 初期状態をセット
	ChangeState(std::make_unique<EnemyStateApproach>(this));

	// 接近フェーズ初期化
	ApproachInitialize();
}

void Enemy::Update() 
{
	// デスフラグの立った弾を削除
	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	state_->Update();

	// 発射タイマーカウントダウン
	fireTimer_--;
	// 指定時間に達した
	if (fireTimer_ <= 0.0f) 
	{
		// 弾の発射
		Fire();

		// 発射タイマーを初期化
		fireTimer_ = kFireInterval;
	}
	
	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state)
{ 
	// 引数で受け取った状態を次の状態としてセットする
	state_ = std::move(state);
}

void Enemy::ApproachInitialize()
{
	// 発射タイマーを初期化
	fireTimer_ = kFireInterval;
}

void Enemy::PositionUpdate(const Vector3& velocity)
{ 
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, velocity);
}

void Enemy::Fire()
{
	// 
	// 弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity(0, 0, kBulletSpeed);
	
	// 速度ベクトルを敵機の向きに合わせて回転させる
	velocity = Matrix::TransformNormal(velocity, worldTransform_.matWorld_);
	
	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	
	// 弾を登録する
	bullets_.push_back(newBullet);
	
}

Vector3 Enemy::GetPosition()
{ 
	return worldTransform_.translation_; 
}

Vector3 Enemy::GetApproachVelocity()
{ 
	return approachVelocity_;
}

Vector3 Enemy::GetLeaveVelocity()
{ 
	return leaveVelocity_;
}

void Enemy::Draw(const ViewProjection& viewProjection) 
{
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 弾描画
	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

/// EnemyStateApproachクラスの実装
EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy)
{

}


void EnemyStateApproach::Update() 
{ 
	enemy_->PositionUpdate(enemy_->GetApproachVelocity());

	/*if (enemy_->GetPosition().z < 0.0f)
	{
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}*/
}

/// EnemyStateLeaveクラスの実装
EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy)
{

}

void EnemyStateLeave::Update() 
{ 
	enemy_->PositionUpdate(enemy_->GetLeaveVelocity());
}