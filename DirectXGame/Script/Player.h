#pragma once
#include <Input.h>
#include <list>
#include "Model.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "WinApp.h"
#include "imgui.h"
#include "MyTools.h"
#include "Matrix.h"
#include "PlayerBullet.h"
#include "Collider.h"

/// <summary>
/// 自キャラ
/// </summary>
class Player : public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="position">初期座標</param>
	/// <param name="reticleTextureHandle">レティクルのテクスチャハンドル</param>
	void Initialize(Model* model, uint32_t textureHandle, Vector3 position, uint32_t reticleTextureHandle);
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Update(ViewProjection& viewProjection);
	
	/// <summary>
	/// 旋回
	/// </summary>
	void Rotate();
	
	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition() override;

	/// <summary>
	/// 3Dレティクルのワールド座標を取得
	/// </summary>
	Vector3 Get3DReticleWorldPosition();

	/// <summary>
	/// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Set3DReticleFromMouseCursor(ViewProjection& viewProjection);

	/// <summary>
	/// 衝突を検出したら呼び出されるコールバック関数
	/// </summary>
	void OnCollision() override;

	/// <summary>
	/// 弾リストを取得
	/// </summary>
	/// <returns></returns>
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

	/// <summary>
	/// 親となるワールドトランスフォームをセット
	/// </summary>
	/// <param name="parent">親となるワールドトランスフォーム</param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(ViewProjection& viewProjection);

	/// <summary>
	/// UI描画
	/// </summary>
	void DrawUI();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* modelPlayer_ = nullptr;	// 自キャラ
	Model* modelBullet_ = nullptr;	// 弾
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// キーボード入力
	Input* input_ = nullptr;

	// 自キャラの速さ
	const float kCharacterSpeed = 0.2f;

	// 弾
	std::list<PlayerBullet*> bullets_;

	// 3Dレティクル用ワールドトランスフォーム
	WorldTransform worldTransform3DReticle_;

	// 2Dレティクル用スプライト
	Sprite* sprite2DReticle_ = nullptr;
};