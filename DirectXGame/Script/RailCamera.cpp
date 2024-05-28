#include "RailCamera.h"
#include "ImGuiManager.h"
#define _USE_MATH_DEFINES
#include <cmath>

void RailCamera::Initialize(const Vector3& position, const Vector3& rotate)
{
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ワールドトランスフォームの初期設定
	worldTransform_.translation_ = position;
	worldTransform_.rotation_ = rotate;
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void RailCamera::Update()
{
	// ワールドトランスフォームの座標の数値を加算したりする(移動)
	worldTransform_.translation_ = MyTools::Add(worldTransform_.translation_, {0.0f, 0.0f, 0.0f});
	// ワールドトランスフォームの角度の数値を加算したりする(回転)
	worldTransform_.rotation_ = MyTools::Add(worldTransform_.rotation_, {0.0f, float(M_PI) / 1200.f, 0.0f});
	// ワールドトランスフォームのワールド行列再計算
	worldTransform_.UpdateMatrix();

	// カメラオブジェクトのワールド行列からビュー行列を計算する
	viewProjection_.matView = Matrix::Inverse(worldTransform_.matWorld_);

#ifdef _DEBUG
	// カメラ座標を画面表示する処理
	ImGui::Begin("Camera");
	ImGui::SliderFloat3("translation", &worldTransform_.translation_.x, -100.0f, 100.0f);
	ImGui::SliderFloat3("rotation", &worldTransform_.rotation_.x, -float(M_PI), float(M_PI));
	ImGui::End();
#endif // _DEBUG


}
