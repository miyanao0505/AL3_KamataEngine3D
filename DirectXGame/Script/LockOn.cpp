#include "LockOn.h"
#include "Enemy.h"

LockOn::~LockOn()
{ 
	delete lockOnMark_;
}

void LockOn::Initialize(Sprite* sprite2DReticle, Vector3 position3DReticle)
{ 
	lockOnMark_ = sprite2DReticle;
	worldPosition3DReticle_ = position3DReticle;

	radius_ = 55.0f;
}

void LockOn::Update(Vector3 playerPos, std::list<Enemy*> enemys, ViewProjection& viewProjection, Sprite* sprite2DReticle, Vector3 position3DReticle) {
	// ターゲットリセット
	target_ = nullptr;
	
	lockOnMark_ = sprite2DReticle;
	lockOnMark_->SetColor({1, 1, 1, 1});

	// 敵リスト分
	for (Enemy* enemy : enemys) {

		if (playerPos.z >= enemy->GetWorldPosition().z)
		{
			continue;
		}

		// ワールド座標からスクリーン座標を計算
		Vector3 position = enemy->GetWorldPosition();

		// ビューポート行列
		Matrix4x4 matViewport = Matrix::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport = Matrix::Multiply(Matrix::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

		// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
		position = Matrix::Transform(position, matViewProjectionViewport);

		// 距離
		float distance = MyTools::Length(MyTools::Subtract(position, {lockOnMark_->GetPosition().x, lockOnMark_->GetPosition().y, 0.0f}));

		// 敵のサイズ
		float enemySize = 1.0f - (enemy->GetWorldPosition().z - playerPos.z) / 30;

		// レティクルと敵の衝突判定
		if (distance <= enemySize + radius_)
		{
			if (target_ == nullptr)
			{
				// 衝突している敵をセット
				target_ = enemy;
			}
			else
			{
				if (enemy->GetWorldPosition().z <= target_->GetWorldPosition().z) {
					// セットしてる敵より手前の敵をセット
					target_ = enemy;
				}
			}

			// 色変更
			lockOnMark_->SetColor({1, 0.5f, 0.5f, 1});

			// レティクルの位置を移動
			lockOnMark_->SetPosition({position.x, position.y});

			continue;
		}
	}

	// ターゲットがセットされていない時
	if (target_ == nullptr)
	{
		lockOnMark_ = sprite2DReticle;

		worldPosition3DReticle_ = position3DReticle;
	}
}

Vector3 LockOn::GetTargetPos() 
{ 
	if (target_ != nullptr && !target_->IsDead())
	{
		return target_->GetWorldPosition();
	}
	
	return worldPosition3DReticle_;
}

void LockOn::Draw()
{
	// 2Dレティクルを描画
	lockOnMark_->Draw();
}
