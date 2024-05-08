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

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;

	// 引数で受け取った速度をメンバ変数に代入
	approachVelocity_ = velocity;
	leaveVelocity_ = {-0.1f, 0.1f, -0.1f};

	state_ = new EnmeyStateApproach();
}

void Enemy::Update() {

	// メンバ関数ポインタに入っている関数を呼び出す
	//(this->*phaseTable_[static_cast<size_t>(phase_)])();

	state_->Update(this);

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

void Enemy::changeState(BaseEnemyState* newState)
{ 
	state_ = newState;
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
void EnmeyStateApproach::Update(Enemy* pEnemy) 
{ 
	pEnemy->PositionUpdate(pEnemy->GetApproachVelocity());

	if (pEnemy->GetPosition().z < 0.0f)
	{
		pEnemy->changeState(new EnemyStateLeave());
	}
}

/// EnemyStateLeaveクラスの実装
void EnemyStateLeave::Update(Enemy* pEnemy) 
{ 
	pEnemy->PositionUpdate(pEnemy->GetLeaveVelocity());
}