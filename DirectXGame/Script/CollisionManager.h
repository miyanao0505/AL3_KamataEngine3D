#pragma once
#include "Collider.h"
#include "MyTools.h"
#include <Vector>

class CollisionManager
{
public:
	CollisionManager();
	~CollisionManager();

	// コライダーをリストに登録する
	void SetColliderList(Collider* collider) { colliders_.push_back(collider); }

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

	 /// <summary>
	 /// コライダー2つの衝突判定と応答
	 /// </summary>
	 /// <param name="colliderA">コライダーA</param>
	 /// <param name="colliderB">コライダーB</param>
	 void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	// コライダーリストをクリア
	void Clear();

private:
	// コライダーリスト
	std::vector<Collider*> colliders_;

};
