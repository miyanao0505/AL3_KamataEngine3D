#pragma once
#include <Input.h>
#include <string>
#include "Model.h"
#include "WorldTransform.h"
#include "MyTools.h"
#include "EnemyBullet.h"

class Enemy;	// Enemyクラスの前方宣言

// 敵の状態基底
class BaseEnemyState
{
public:
	BaseEnemyState(const std::string& name, Enemy* enemy) : name_(name), enemy_(enemy){};

	// 毎フレーム処理(純粋仮想関数)
	virtual void Update() = 0;

	protected:
		// 状態名
	    std::string name_;
		// 操作対象の敵
	    Enemy* enemy_ = nullptr;
};

/// <summary>
/// 接近フェーズ
/// </summary>
class EnemyStateApproach : public BaseEnemyState
{
public:
	// コンストラクタ
	EnemyStateApproach(Enemy* enemy);
	// 更新
	void Update();
};

/// <summary>
/// 離脱フェーズ
/// </summary>
class EnemyStateLeave : public BaseEnemyState
{
public:
	// コンストラクタ
	EnemyStateLeave(Enemy* enemy);
	// 更新
	void Update();
};

/// <summary>
/// 敵
/// </summary>
class Enemy : public MyBase {
public:

	~Enemy();		// デストラクタ

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
	/// 状態変更
	/// </summary>
	/// <param name="newState"></param>
	void ChangeState(std::unique_ptr<BaseEnemyState> state);

	/// <summary>
	/// 接近フェーズ初期化
	/// </summary>
	void ApproachInitialize();

	/// <summary>
	/// 移動
	/// </summary>
	void PositionUpdate(const Vector3& velocity);

	/// <summary>
	/// 弾発射
	/// </summary>
	void Fire();

	/// <summary>
	/// 座標の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetPosition();

	/// <summary>
	/// 接近の速度の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetApproachVelocity();

	/// <summary>
	/// 離脱の速度の取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetLeaveVelocity();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(const ViewProjection& viewProjection);

	// 発射間隔
	static const int kFireInterval = 60;

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// キーボード入力
	Input* input_ = nullptr;

	// 初期座標
	Vector3 positinInitialize_;

	// 速度
	Vector3 approachVelocity_;		// 接近フェーズの速度
	Vector3 leaveVelocity_;			// 離脱フェーズの速度

	// 状態
	std::unique_ptr<BaseEnemyState> state_;

	// 弾
	std::list<EnemyBullet*> bullets_;
	EnemyBullet* bullet_ = nullptr;

	// 発射タイマー
	int32_t fireTimer_ = 0;
};