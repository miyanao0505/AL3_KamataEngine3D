#include "Enemy.h"
#include <cassert>
#include "TextureManager.h"
#include "Player.h"
#include "GameScene.h"

/// Enemyクラスの実装
Enemy::~Enemy() 
{ 
	// timedCall_の解放
	for (TimedCall* timedCall : timedCalls_) {
		delete timedCall;
	}
}

/// 初期化
void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// NULLポインタチェック
	assert(model);

	model_ = Model::CreateFromOBJ("enemy", true);
	modelBullet_ = model;
	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load("enemy/enemy.png");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// 初期座標を保持
	positinInitialize_ = position;

	// 引数で受け取った初期座標をセット
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = float(M_PI);

	// 引数で受け取った速度をメンバ変数に代入
	approachVelocity_ = velocity;
	leaveVelocity_ = {-0.1f, 0.1f, -0.1f};
	
	// 接近フェーズ初期化
	ApproachInitialize();

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeEnemy);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributeEnemy);
}

/// 更新
void Enemy::Update(ViewProjection& viewProjection) 
{
	PositionUpdate(approachVelocity_);

	if (GetPosition().z < player_->GetWorldPosition().z) {
		Clear();
	}

	// 終了したタイマーを削除
	timedCalls_.remove_if([](TimedCall* timedCall) {
		if (timedCall->IsFinished()) {
			delete timedCall;
			return true;
		}
		return false;
	});

	// 範囲forでリストの全要素について回す
	for (TimedCall* timedCall : timedCalls_) {
		timedCall->Update();
	}

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

	// lockOnフラグが下がってる時
	if (!isLockOn_)
	{
		if (player_->GetWorldPosition().z >= GetWorldPosition().z)
		{
			return;
		}

		// ビューポート行列
		Matrix4x4 matViewport = Matrix::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport = Matrix::Multiply(Matrix::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

		// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
		Vector3 position = Matrix::Transform(GetWorldPosition(), matViewProjectionViewport);

		// 距離
		float distance = MyTools::Length(MyTools::Subtract(position, { player_->GetSprite2DReticle()->GetPosition().x, player_->GetSprite2DReticle()->GetPosition().y, 0.0f}));

		// 敵のサイズ
		float enemySize = 1.0f - (GetWorldPosition().z - player_->GetWorldPosition().z) / 30;

		// レティクルと敵の衝突判定
		if (distance <= enemySize + player_->GetReticleSize()) {
			LockOn* newLockOn = new LockOn();
			newLockOn->Initialize(player_->GetSprite2DReticle()->GetTextureHandle(), {position.x, position.y}, this);
			
			gameScene_->AddLockOnMark(newLockOn);

			isLockOn_ = true;
		}
	}
}

/// 接近フェーズ初期化
void Enemy::ApproachInitialize()
{
	// 発射タイマーをセットする
	timedCalls_.push_back(new TimedCall(std::bind_front(&Enemy::FireReset, this), uint32_t(kFireInterval)));
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
	newBullet->Initialize(modelBullet_, worldTransform_.translation_, velocity);
	newBullet->SetPlayer(player_);

	gameScene_->AddEnemyBullet(newBullet);
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

// 衝突を検出したら呼び出されるコールバック関数
void Enemy::OnCollision()
{ 
	// デスフラグを立てる
	isDead_ = true;
}

/// 描画
void Enemy::Draw(const ViewProjection& viewProjection) 
{
	// モデルの描画
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

