#pragma once
#include "Sprite.h"
#include "WinApp.h"
#include "ViewProjection.h"
#include <list>

class Enemy;

class LockOn {
public:
	~LockOn();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(uint32_t texture, Vector2 position, Enemy* enemy);
	// 引数変更->2Dレティクルのデータ & ターゲットのデータのみ

	/// <summary>
	/// 更新
	/// </summary>
	void Update(ViewProjection& viewProjection);
	// 引数変更->なにもなし 引数の見直し必須

	Vector3 GetTargetPos();
	// 変更->消去

	// 追加->引数の
	bool IsDead();

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
