#pragma once
#include "Sprite.h"
#include "WinApp.h"
#include "ViewProjection.h"
#include <list>

class Enemy;

class LockOn 
{
public:

	~LockOn();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Sprite* sprite2DReticle, Vector3 position3DReticle);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Vector3 playerPos, std::list<Enemy*> enemys, ViewProjection& viewProjection, Sprite* sprite2DReticle, Vector3 position3DReticle);

	Vector3 GetTargetPos();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// 2Dレティクル用スプライト
	Sprite* lockOnMark_ = nullptr;

	// 3Dレティクル用ワールドトランスフォーム
	Vector3 worldPosition3DReticle_;

	float radius_;

	// ロックオン対象
	Enemy* target_ = nullptr;
};
