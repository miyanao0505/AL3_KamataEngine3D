#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "PrimitiveDrawer.h"
#include "MyTools.h"
#include "Matrix.h"

/// <summary>
/// レールカメラ
/// </summary>
class RailCamera : public MyBase
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="position">ワールド座標</param>
	/// <param name="rotate">回転角(ラジアン)</param>
	/// <param name="velocity">速度</param>
	void Initialize(const Vector3& position, const Vector3& rotate);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// ビュープロジェクションを取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	const ViewProjection& GetViewProjection() { return viewProjection_; }

	/// <summary>
	/// ワールドトランスフォームを取得
	/// </summary>
	/// <returns></returns>
	const WorldTransform& GetWorldTransform() { return worldTransform_; }

	/// <summary>
	/// Catmull-RomSplineの描画
	/// </summary>
	void DrawCatmullRom();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection viewProjection_;
};
