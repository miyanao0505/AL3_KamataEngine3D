#pragma once
#include <Input.h>
#include "Model.h"
#include "WorldTransform.h"
#include "MyTools.h"
#include "Collider.h"

// 自キャラの弾の前方宣言
//class PlayerBullet;

// 敵の前方宣言
//class Enemy;

// 自キャラの弾のタイプ基底
//class BasePlayerBulletType {
//public:
//	BasePlayerBulletType(const std::string& name, PlayerBullet* playerBullet) : name_(name), playerBullet_(playerBullet){};
//
//	// 毎フレーム処理(純粋仮想関数)
//	virtual void Update(const Vector3& velocity) = 0;
//
//protected:
//	// 状態名
//	std::string name_;
//	// 操作対象の自キャラの弾
//	PlayerBullet* playerBullet_ = nullptr;
//};

/// <summary>
/// 通常弾
/// </summary>
//class PlayerBulletTypeNormal : public BasePlayerBulletType {
//public:
//	// コンストラクタ
//	PlayerBulletTypeNormal(PlayerBullet* playerBullet);
//	// 更新
//	void Update(const Vector3& velocity);
//};

/// <summary>
/// ホーミング弾
/// </summary>
//class PlayerBulletTypeHoming : public BasePlayerBulletType {
//public:
//	// コンストラクタ
//	PlayerBulletTypeHoming(PlayerBullet* playerBullet);
//	// 更新
//	void Update(const Vector3& velocity);
//	// 敵をセット
//	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
//
//protected:
//	// ホーミングする敵の情報
//	Enemy* enemy_ = nullptr;
//};

/// <summary>
/// 自キャラの弾
/// </summary>
class PlayerBullet : public Collider
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="type">タイプ 0:通常 1:ホーミング</param>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity, uint32_t type);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition();

	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision() override;

	/// <summary>
	/// 速度をセット
	/// </summary>
	/// <param name="velocity">速度</param>
	void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }

	/// <summary>
	/// 角度をセット
	/// </summary>
	/// <param name="rotate">角度</param>
	void SetRotate(const Vector3& rotate) { worldTransform_.rotation_ = rotate; }

	/// <summary>
	/// タイプをセット
	/// </summary>
	/// <param name="type">タイプ 0:通常 1:ホーミング</param>
	void SetType(uint32_t type);

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

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
	Vector3 velocity_;

	// タイプ
	//std::unique_ptr<BasePlayerBulletType> type_;

	// 寿命<frm>
	static const int32_t kLifeTime = 60 * 5;

	// デスタイマー
	int32_t deathTimer_ = kLifeTime;
	// デスフラグ
	bool isDead_ = false;
};
