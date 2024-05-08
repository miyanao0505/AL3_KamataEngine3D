#pragma once
#include <Input.h>
#include "Model.h"
#include "WorldTransform.h"
#include "MyTools.h"

// 行動フェーズ
enum class Phase {
	Approach,	// 接近する
	Leave,		// 離脱する
};

/// <summary>
/// 敵
/// </summary>
class Enemy : public MyBase {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 接近フェーズの更新
	/// </summary>
	void ApproachUpdate();

	/// <summary>
	/// 離脱フェーズの更新
	/// </summary>
	void LeaveUpdate();

	/// <summary>
	///	
	/// </summary>
	void TestFunc();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// キーボード入力
	Input* input_ = nullptr;

	// 速度
	Vector3 approachVelocity_;		// 接近フェーズの速度
	Vector3 leaveVelocity_;			// 離脱フェーズの速度

	// フェーズ
	Phase phase_ = Phase::Approach;

	// メンバ関数ポインタのテーブル
	static void (Enemy::*phaseTable_[])();
};