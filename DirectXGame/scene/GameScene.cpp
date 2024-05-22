#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "AxisIndicator.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete player_;
	delete debugCamera_;
	delete collisionManager_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");

	// 3Dモデルの生成
	model_ = Model::Create();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_);

	// 敵キャラの生成
	enemy_ = new Enemy();
	// 敵キャラの初期化
	enemy_->Initialize(model_, { 20.0f, 2.0f, 40.f }, { 0.0f, 0.0f, -0.2f });
	// 敵に自キャラのアドレスを渡す
	enemy_->SetPlayer(player_);

	// 衝突マネージャの生成
	collisionManager_ = new CollisionManager();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	// 衝突マネージャのリストをクリア
	collisionManager_->Clear();

	// 自キャラの更新
	player_->Update();

	// 敵キャラの更新
	enemy_->Update();

	// コライダー全てを衝突マネージャのリストに登録する
	//collisionManager_->SetColliderList();
	SetCollisionManager();
	
	// 衝突判定と応答
	collisionManager_->CheckAllCollisions();

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_P)) {
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif // _DEBUG

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		// ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
}

//void GameScene::CheckAllCollisions()
//{
//	// 自弾リストの取得
//	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
//	// 敵弾リストの取得
//	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();
//
//	// コライダー
//	std::list<Collider*> colliders_;
//	// コライダーをリストに登録
//	colliders_.push_back(player_);
//	colliders_.push_back(enemy_);
//	// 自弾全てについて
//	for (PlayerBullet* playerBullet : playerBullets)
//	{
//		colliders_.push_back(playerBullet);
//	}
//	// 敵弾全てについて
//	for (EnemyBullet* enemyBullet : enemyBullets)
//	{
//		colliders_.push_back(enemyBullet);
//	}
//
//	// リスト内のペアを総当たり
//	std::list<Collider*>::iterator itrA = colliders_.begin();
//	for (; itrA != colliders_.end(); ++itrA)
//	{
//		// イテレータAからコライダーAを取得する
//		Collider* colliderA = *itrA;
//
//		// イテレータBはイテレータAの次の要素から回す(重複判定を回避)
//		std::list<Collider*>::iterator itrB = itrA;
//		itrB++;
//
//		for (; itrB != colliders_.end(); ++itrB)
//		{
//			// イテレータBからコライダーBを取得する
//			Collider* colliderB = *itrB;
//
//			// ペアの当たり判定
//			CheckCollisionPair(colliderA, colliderB);
//		}
//	}
//}

void GameScene::SetCollisionManager() { 

	// 自キャラセット
	collisionManager_->SetColliderList(player_);

	// 敵キャラセット
	collisionManager_->SetColliderList(enemy_);

	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemy_->GetBullets();

	// 自弾全てセット
	for (PlayerBullet* playerBullet : playerBullets) {
		collisionManager_->SetColliderList(playerBullet);
	}
	// 敵弾全てセット
	for (EnemyBullet* enemyBullet : enemyBullets) {
		collisionManager_->SetColliderList(enemyBullet);
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// 自キャラの描画
	player_->Draw(viewProjection_);

	// 敵キャラの描画
	enemy_->Draw(viewProjection_);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

