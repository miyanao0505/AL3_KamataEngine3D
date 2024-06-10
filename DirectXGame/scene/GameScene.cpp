#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "AxisIndicator.h"
#include <cassert>
#include <fstream>

GameScene::GameScene() {}

GameScene::~GameScene() { 
	delete model_;
	delete modelSkydome_;
	delete player_;
	delete skydome_;
	delete railCamera_;
	delete debugCamera_;
	delete collisionManager_;

	// enemy_の解放
	for (Enemy* enemy : enemys_) {
		delete enemy;
	}

	// enemyBullet_の解放
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		delete enemyBullet;
	}
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("player.png");

	// 3Dモデルの生成
	model_ = Model::Create();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 2Dレティクル用テクスチャ取得
	uint32_t textureReticle = TextureManager::Load("2DReticle.png");

	// 自キャラの生成
	player_ = new Player();
	// 自機の位置調整
	Vector3 playerPosition(0, 0, 50);
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, playerPosition, textureReticle);

	/// 敵発生データの読み込み
	LoadEnemyPopData();

	// 天球の生成
	skydome_ = new Skydome();
	// 天球の初期化
	skydome_->Initialize(modelSkydome_, {0.0f, 0.0f, 0.0f});

	// 衝突マネージャの生成
	collisionManager_ = new CollisionManager();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// レールカメラの生成
	railCamera_ = new RailCamera();
	// レールカメラの初期化
	railCamera_->Initialize({0.0f, 0.0f, -50.f}, {0.0f, 0.0f, 0.0f});

	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// ビュープロジェクションの初期化
	debugCamera_->SetFarZ(1000);
	viewProjection_.Initialize();

	primitiveDrawer_ = PrimitiveDrawer::GetInstance();
	primitiveDrawer_->Initialize();
	primitiveDrawer_->SetViewProjection(&viewProjection_);

	// スプライン曲線制御点(通過点)
	controlPoints_ = {
	    {0.f,  0.f,  0.f},
        {10.f, 10.f, 0.f},
        {10.f, 15.f, 0.f},
        {20.f, 15.f, 0.f},
        {20.f, 0.f,  0.f},
        {30.f, 0.f,  0.f},
	};
	// 線分の数
	const size_t segmentCount = 100;
	// 線分の数+1個分の頂点座標を計算
	for (size_t i = 0; i < segmentCount + 1; i++) {
		float t = 1.0f / segmentCount * i;
		Vector3 pos = MyTools::CatmullRomPosition(controlPoints_, t);
		// 描画用頂点リストに追加
		pointsDrawing.push_back(pos);
	}

	// 軸方向表示の表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する(アドレス渡し)
	AxisIndicator::GetInstance()->SetTargetViewProjection(&viewProjection_);
}

void GameScene::Update() {
	// デスフラグの立った弾を削除
	enemyBullets_.remove_if([](EnemyBullet* enemyBullet) {
		if (enemyBullet->IsDead()) {
			delete enemyBullet;
			return true;
		}
		return false;
	});

	// デスフラグの立った敵を削除
	enemys_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
	});

	// 衝突マネージャのリストをクリア
	collisionManager_->Clear();

	/// 敵発生コマンドの更新
	UpdateEnemyPopCommands();

	// 自キャラの更新
	player_->Update(viewProjection_);

	// 敵キャラの更新
	for (Enemy* enemy : enemys_) {
		enemy->Update();
	}

	// 弾更新
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Update();
	}

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

	// 天球
	skydome_->Update();

	// レールカメラの更新
	railCamera_->Update();
	viewProjection_.matView = railCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;
	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();
	} else {
		// ビュープロジェクション行列の更新と転送
		//viewProjection_.UpdateMatrix();
	}
}

void GameScene::SetCollisionManager() { 

	// 自キャラセット
	collisionManager_->SetColliderList(player_);

	// 敵キャラセット
	const std::list<Enemy*>& enemys = enemys_;
	// 自弾リストの取得
	const std::list<PlayerBullet*>& playerBullets = player_->GetBullets();
	// 敵弾リストの取得
	const std::list<EnemyBullet*>& enemyBullets = enemyBullets_;

	// 敵全てセット
	for (Enemy* enemy : enemys) {
		collisionManager_->SetColliderList(enemy);
	}
	// 自弾全てセット
	for (PlayerBullet* playerBullet : playerBullets) {
		collisionManager_->SetColliderList(playerBullet);
	}
	// 敵弾全てセット
	for (EnemyBullet* enemyBullet : enemyBullets) {
		collisionManager_->SetColliderList(enemyBullet);
	}
}

void GameScene::LoadEnemyPopData()
{
	// ファイルを開く
	std::ifstream file;
	file.open("Resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::EnemyBorn(Vector3 pos)
{
	// 敵キャラの生成
	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(model_, pos, {0.0f, 0.0f, -0.2f});
	newEnemy->SetGameScene(this);
	newEnemy->SetPlayer(player_);

	// 敵キャラリストに登録
	enemys_.push_back(newEnemy);
}

void GameScene::UpdateEnemyPopCommands()
{
	// 待機処理
	if (isWait) {
		waitTimer--;
		if (waitTimer <= 0)
		{
			// 待機完了
			isWait = false;
		}
		return;
	}

	// 1行分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands, line)) {
		// 1行列の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// X座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			EnemyBorn(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0)
		{
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待機開始
			isWait = true;
			waitTimer = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet)
{
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
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
	 
	// 天球の描画
	skydome_->Draw(viewProjection_);

	// 自キャラの描画
	player_->Draw(viewProjection_);

	// 敵キャラの描画
	for (Enemy* enemy : enemys_) {
		enemy->Draw(viewProjection_);
	}

	// 弾描画
	for (EnemyBullet* enemyBullet : enemyBullets_) {
		enemyBullet->Draw(viewProjection_);
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// 自キャラの2D描画
	player_->DrawUI();

	// 先頭から2点ずつ取り出してライン描画
	for (size_t i = 0; i < pointsDrawing.size() - 1; i++) {
		primitiveDrawer_->DrawLine3d(pointsDrawing.at(i), pointsDrawing.at(i + 1), Vector4(255, 0, 0, 255));
	}
	
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

