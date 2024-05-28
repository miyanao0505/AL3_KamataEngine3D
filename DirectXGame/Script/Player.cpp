#define _USE_MATH_DEFINES
#include <cmath>
#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model, uint32_t textureHandle, Vector3 position, uint32_t reticleTextureHandle) {
	// NULLポインタチェック
	assert(model);

	// 3Dモデルの生成
	modelPlayer_ = model;
	modelBullet_ = model;
	
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = textureHandle;

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.UpdateMatrix();

	// シングルトンインスタンスを取得する
	input_ = Input::GetInstance();

	// 衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	// 衝突対象を自分の属性以外に設定
	SetCollisionMask(~kCollisionAttributePlayer);

	// 3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

	// スプライト
	sprite2DReticle_ = Sprite::Create(reticleTextureHandle, {640.0f, 360.0f}, {1, 1, 1, 1}, {0.5f, 0.5f});
}

Player::~Player() {
	// bullets_の解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	// スプライトの解放
	delete sprite2DReticle_;
}

void Player::Update(ViewProjection& viewProjection) {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// デスフラグの立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) { 
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	// 旋回処理
	Rotate();

	// キャラクターの移動ベクトル
	Vector3 move = {0, 0, 0};

	// キャラクターの移動速さ
	const float kCharacterSpeed = 0.2f;

	// ゲームパッド操作
	// ゲームパッド状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		move.x += (float)joyState.Gamepad.sThumbLX / SHRT_MAX * kCharacterSpeed;
		move.y += (float)joyState.Gamepad.sThumbLY / SHRT_MAX * kCharacterSpeed;
	}

	// キーボード操作
	// 押した方向で移動ベクトルを変更(左右)
	/*if (input_->PushKey(DIK_A)) {
		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_D)) {
		move.x += kCharacterSpeed;
	}*/

	// 押した方向で移動ベクトルを変更(上下)
	/*if (input_->PushKey(DIK_W)) {
		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_S)) {
		move.y -= kCharacterSpeed;
	}*/

	// 座標移動(ベクトルの加算)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, move);

	// 自機のワールド座標から3Dレティクルのワールド座標を計算
	/* {
		// 自機から3Dレティクルの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		// 自機から3Dレティクルへのオフセット(Z+向き)
		Vector3 offset = {0, 0, 1.0f};
		// 自機のワールド行列の回転を反映
		offset = Matrix::TransformNormal(offset, worldTransform_.matWorld_);
		// ベクトルの長さを整える
		offset = MyTools::Multiply(kDistancePlayerTo3DReticle, MyTools::Normalize(offset));
		// 3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_ = MyTools::Add(GetWorldPosition(), offset);
		worldTransform3DReticle_.UpdateMatrix();
	}*/

	// 3Dレティクルのワールド座標から2Dレティクルのスクリーン座標を計算
	/* { 
		Vector3 positionReticle = Get3DReticleWorldPosition(); 
	
		// ビューポート行列
		Matrix4x4 matViewport = Matrix::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport = Matrix::Multiply(Matrix::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

		// ワールド→スクリーン座標変換(ここで3Dから2Dになる)
		positionReticle = Matrix::Transform(positionReticle, matViewProjectionViewport);

		// スプライトのレティクルに座標設定
		sprite2DReticle_->SetPosition(Vector2(positionReticle.x, positionReticle.y));
	}*/

	// マウスカーソルのスクリーン座標からワールド座標を取得して3Dレティクル配置
	{ Set3DReticleFromMouseCursor(viewProjection); }

	// 攻撃処理
	Attack();

	// 弾更新
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}

	// スケーリング行列の作成
	worldTransform_.UpdateMatrix();
	
#ifdef _DEBUG
	// キャラクターの座標を画面表示する処理
	ImGui::Begin("Player");
	// float3スライダー
	ImGui::DragFloat3("translate", &worldTransform_.translation_.x, 0.05f);
	//
	ImGui::End();
#endif // _DEBUG

	// 移動限界座標
	const float kMoveLimitX = 34.f;
	const float kMoveLimitY = 18.f;

	// 範囲を超えない処理
	worldTransform_.translation_.x = MyTools::Clamp(worldTransform_.translation_.x, -kMoveLimitX, kMoveLimitX);
	worldTransform_.translation_.y = MyTools::Clamp(worldTransform_.translation_.y, -kMoveLimitY, kMoveLimitY);

	// ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix();

}

void Player::Rotate() {
	// 回転速さ[ラジアン/frame]
	const float kRotSpeed = 0.02f;

	// 押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		worldTransform_.rotation_.y += kRotSpeed;
	} else if(input_->PushKey(DIK_RIGHT)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}
}

void Player::Attack() {
	XINPUT_STATE joyState;

	// ゲームパッド未接続なら何もせず抜ける
	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	// Rトリガーを押していたら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, 0);

		// 自機から照準オブジェクトへのベクトル
		velocity = MyTools::Subtract(Get3DReticleWorldPosition(), GetWorldPosition());
		velocity = MyTools::Multiply(kBulletSpeed, MyTools::Normalize(velocity));

		// 速度ベクトルを自機の向きに合わせて回転させる
		//velocity = Matrix::TransformNormal(velocity, Matrix::Inverse(worldTransform_.parent_->matWorld_));

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}

	// 発射キーをトリガーしたら(キーボード)
	/* if (input_->TriggerKey(DIK_SPACE)) {

		// 弾の速度
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, 0);

		// 自機から照準オブジェクトへのベクトル
		velocity = MyTools::Subtract(Get3DReticleWorldPosition(), GetWorldPosition());
		velocity = MyTools::Multiply(kBulletSpeed, MyTools::Normalize(velocity));

		// 速度ベクトルを自機の向きに合わせて回転させる
		velocity = Matrix::TransformNormal(velocity, worldTransform_.matWorld_);

		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(modelBullet_, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}*/
}

Vector3 Player::GetWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

/// 3Dレティクルのワールド座標を取得
Vector3 Player::Get3DReticleWorldPosition()
{
	// ワールド座標を入れる変数
	Vector3 worldPos;

	// ワールド行列の平行移動成分を取得(ワールド座標)
	worldPos.x = worldTransform3DReticle_.matWorld_.m[3][0];
	worldPos.y = worldTransform3DReticle_.matWorld_.m[3][1];
	worldPos.z = worldTransform3DReticle_.matWorld_.m[3][2];

	return worldPos;
}

/// マウスカーソルのワールド座標を取得
void Player::Set3DReticleFromMouseCursor(ViewProjection& viewProjection) {
	// スプライトの現在座標を取得
	Vector2 spritePosition = sprite2DReticle_->GetPosition();

	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;

	// ジョイスティック状態取得
	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		spritePosition.x += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * 5.0f;
		spritePosition.y -= (float)joyState.Gamepad.sThumbRY / SHRT_MAX * 5.0f;

		// スプライトの座標変更を反映
		sprite2DReticle_->SetPosition(spritePosition);
	}

	// マウス操作
	/* POINT mousePosition;
	// マウス座標(スクリーン座標)を取得
	GetCursorPos(&mousePosition);

	// クライアントエリア座標に変換する
	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePosition);

	// マウス座標を2Dレティクルのスプライトに代入する
	sprite2DReticle_->SetPosition(Vector2(float(mousePosition.x), float(mousePosition.y)));*/

	// ビューポート行列
	Matrix4x4 matViewport = Matrix::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);
	// ビュー行列、射影行列、ビューポート行列の合成行列を計算する
	Matrix4x4 matVPV = Matrix::Multiply(Matrix::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);
	// 合成行列の逆行列を計算する
	Matrix4x4 matInverseVPV = Matrix::Inverse(matVPV);

	// スクリーン座標
	// ゲームパッド
	Vector3 posNear = Vector3(spritePosition.x, spritePosition.y, 0);
	Vector3 posFar = Vector3(spritePosition.x, spritePosition.y, 1);
	// マウス
	/*Vector3 posNear = Vector3(float(mousePosition.x), float(mousePosition.y), 0);
	Vector3 posFar = Vector3(float(mousePosition.x), float(mousePosition.y), 1);*/

	// ニアクリップ面上のワールド座標を得る(スクリーン→ワールド変換)
	posNear = Matrix::Transform(posNear, matInverseVPV);
	// ファークリップ面上のワールド座標を得る(スクリーン→ワールド変換)
	posFar = Matrix::Transform(posFar, matInverseVPV);

	// マウスレイの方向
	// マウスの前方ベクトルを計算する
	Vector3 mouseDirection = MyTools::Subtract(posFar, posNear);
	mouseDirection = MyTools::Normalize(mouseDirection);
	// カメラから照準オブジェクトの距離
	const float kDistanceTestObject = 100.0f;
	// ニアクリップ面上のワールド座標から一定距離前進したところに3Dレティクルを配置
	worldTransform3DReticle_.translation_ = MyTools::Add(posNear, MyTools::Multiply(kDistanceTestObject, mouseDirection));
	worldTransform3DReticle_.UpdateMatrix();

#ifdef _DEBUG
	ImGui::Begin("Player");
	ImGui::Text("2DReticle:(%f,%f)", sprite2DReticle_->GetPosition().x, sprite2DReticle_->GetPosition().y);
	ImGui::Text("Near:(%+.2f,%+.2f,%+.2f)", posNear.x, posNear.y, posNear.z);
	ImGui::Text("Far:(%+.2f,%+.2f,%+.2f)", posFar.x, posFar.y, posFar.z);
	ImGui::Text("3DReticle(%+.2f,%+.2f,%+.2f)", worldTransform3DReticle_.translation_.x, worldTransform3DReticle_.translation_.y, worldTransform3DReticle_.translation_.z);
	ImGui::End();
#endif // _DEBUG

}

// 衝突を検出したら呼び出されるコールバック関数
void Player::OnCollision() 
{

}

/// 親となるワールドトランスフォームをセット
void Player::SetParent(const WorldTransform* parent)
{
	// 親子関係を結ぶ
	worldTransform_.parent_ = parent;
}

void Player::Draw(ViewProjection& viewProjection) {
	// 3Dモデルを描画
	modelPlayer_->Draw(worldTransform_, viewProjection, textureHandle_);

	// 3Dレティクルを描画
	modelBullet_->Draw(worldTransform3DReticle_, viewProjection);

	// 弾描画
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::DrawUI()
{
	// 2Dレティクルを描画
	sprite2DReticle_->Draw();
}
