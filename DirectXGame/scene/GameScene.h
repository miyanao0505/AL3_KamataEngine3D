#pragma once

#include <sstream>
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Script/RailCamera.h"
#include "Script/Player.h"
#include "Script/Enemy.h"
#include "Script/Skydome.h"
#include "Script/MyTools.h"
#include "Script/CollisionManager.h"
#include "DebugCamera.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突マネージャに登録
	/// </summary>
	void SetCollisionManager();

	/// <summary>
	/// 敵発生データの読み込み
	/// </summary>
	void LoadEnemyPopData();

	/// <summary>
	/// 敵発生
	/// </summary>
	/// <param name="pos">初期位置</param>
	void EnemyBorn(Vector3 pos);

	/// <summary>
	/// 敵発生コマンドの更新
	/// </summary>
	void UpdateEnemyPopCommands();

	/// <summary>
	/// 敵弾を追加する
	/// </summary>
	/// <param name="enemyBullet">敵弾</param>
	void AddEnemyBullet(EnemyBullet* enemyBullet);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0;

	// 3Dモデルデータ
	Model* model_ = nullptr;

	// 3Dモデル
	Model* modelSkydome_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;

	// レールカメラ
	RailCamera* railCamera_ = nullptr;

	// 自キャラ
	Player* player_ = nullptr;

	// 敵キャラ
	std::list<Enemy*> enemys_;
	// 敵発生コマンド
	std::stringstream enemyPopCommands;
	bool isWait = false;					// 待機中フラグ
	int32_t waitTimer = 0;					// 待機タイマー

	// 敵弾
	std::list<EnemyBullet*> enemyBullets_;

	// 天球
	Skydome* skydome_ = nullptr;

	// 衝突マネージャ
	CollisionManager* collisionManager_ = nullptr;

	// デバッグカメラ
	bool isDebugCameraActive_ = false;
	DebugCamera* debugCamera_ = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
