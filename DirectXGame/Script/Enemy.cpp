#include "Enemy.h"
#include <cassert>
#include "TextureManager.h"

/// Enemyクラスの実装
Enemy::Enemy()
{

}

Enemy::~Enemy() 
{ 
	
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
}

void Enemy::Update() 
{
	state_->Update();

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state)
{ 
	// 引数で受け取った状態を次の状態としてセットする
	state_ = std::move(state);
}

void Enemy::PositionUpdate(const Vector3& velocity)
{ 
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, velocity);
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
}

/// EnemyStateApproachクラスの実装
EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy)
{

}


void EnemyStateApproach::Update() 
{ 
	enemy_->PositionUpdate(enemy_->GetApproachVelocity());

	if (enemy_->GetPosition().z < 0.0f)
	{
		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}

/// EnemyStateLeaveクラスの実装
EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy)
{

}

void EnemyStateLeave::Update() 
{ 
	enemy_->PositionUpdate(enemy_->GetLeaveVelocity());
}