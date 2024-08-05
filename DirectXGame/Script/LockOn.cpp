#include "LockOn.h"
#include "Enemy.h"

LockOn::~LockOn()
{ 
	delete lockOnMark_;
}

void LockOn::Initialize(uint32_t texture, Vector2 position, Enemy* enemy) 
{

	lockOnMark_ = Sprite::Create(texture, position, {1, 0.5f, 0.5f, 1}, {0.5f, 0.5f});

	target_ = enemy;

	radius_ = 55.0f;
}

void LockOn::Update(ViewProjection& viewProjection) {
	// ワールド座標からスクリーン座標を計算
	Vector3 position = target_->GetWorldPosition();

	// ビューポート行列
	Matrix4x4 matViewport = Matrix::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = Matrix::Multiply(Matrix::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
	position = Matrix::Transform(position, matViewProjectionViewport);

	lockOnMark_->SetPosition({position.x, position.y});
}

Vector3 LockOn::GetTargetPos() 
{ 
	if (target_ != nullptr && !target_->IsDead())
	{
		return target_->GetWorldPosition();
	}
	
	return worldPosition3DReticle_;
}

bool LockOn::IsDead() { return target_->IsDead(); }

void LockOn::Draw()
{
	// 2Dレティクルを描画
	lockOnMark_->Draw();
}
