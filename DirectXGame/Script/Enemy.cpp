#include "Enemy.h"
#include <cassert>
#include "TextureManager.h"
#include "Player.h"

/// Enemyクラスの実装
Enemy::~Enemy() 
{ 
	// bullet_の解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}

	// timedCall_の解放
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

/// 初期化
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

/// 更新
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

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});
	
	// 弾更新
	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}

	// 範囲forでリストの全要素について回す
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();
}

/// 状態変更
void Enemy::ChangeState(std::unique_ptr<BaseEnemyState> state)
{ 
	// 引数で受け取った状態を次の状態としてセットする
	state_ = std::move(state);
}

/// 接近フェーズ初期化
void Enemy::ApproachInitialize()
{
	// 発射タイマーをセットする
	timedCalls_.push_back(new TimedCall(std::bind_front(&Enemy::FireReset, this), kFireInterval));
}

/// 移動
void Enemy::PositionUpdate(const Vector3& velocity)
{ 
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, velocity);
}

/// 弾発射
void Enemy::Fire()
{
	assert(player_);

	// 弾の速度
	const float kBulletSpeed = 0.5f;

	Vector3 playerPos = player_->GetWorldPosition();
	Vector3 enemyPos = GetWorldPosition();
	Vector3 differenceVector = MyTools::Subtract(playerPos, enemyPos);
	differenceVector = MyTools::Normalize(differenceVector);
	Vector3 velocity = MyTools::Multiply(kBulletSpeed, differenceVector);
	
	// 弾を生成し、初期化
	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(model_, worldTransform_.translation_, velocity);
	
	// 弾を登録する
	bullets_.push_back(newBullet);
}

/// 弾を発射し、タイマーをリセットするコールバック関数
void Enemy::FireReset()
{
	// 弾の発射
	Fire();

	// 発射タイマーをセットする
	timedCalls_.push_back(new TimedCall(std::bind_front(&Enemy::FireReset, this), kFireInterval));
}

/// 時限発動のイベントのクリア
void Enemy::Clear() 
{
	// timedCalls_リストをクリアし、内部のTimedCallオブジェクトを削除
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
	timedCalls_.clear(); // リストを空にする
}

/// 座標の取得
Vector3 Enemy::GetPosition()
{ 
	return worldTransform_.translation_; 
}

/// 接近の速度の取得
Vector3 Enemy::GetApproachVelocity()
{ 
	return approachVelocity_;
}

/// 離脱の速度の取得
Vector3 Enemy::GetLeaveVelocity()
{ 
	return leaveVelocity_;
}

/// ワールド座標を取得
Vector3 Enemy::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

/// 描画
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
	// 接近フェーズ初期化
	enemy->ApproachInitialize();
}

// 更新
void EnemyStateApproach::Update() 
{ 
	enemy_->PositionUpdate(enemy_->GetApproachVelocity());

	if (enemy_->GetPosition().z < 0.0f)
	{
		enemy_->Clear();

		enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	}
}

/// EnemyStateLeaveクラスの実装
EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy)
{

}

// 更新
void EnemyStateLeave::Update() 
{ 
	enemy_->PositionUpdate(enemy_->GetLeaveVelocity());
}