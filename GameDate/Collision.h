#pragma once

#include "GameDate/Objects.h"

// カプセルの判定用構造体
class CapsuleCollider
{
public:
	//判定用カプセルを設定する
	void SetCapsule(Capsule capsule);

public:
	//コンストラクタ
	CapsuleCollider();
	//デストラクタ
	~CapsuleCollider();

	//他のカプセルとの当たり判定をする
	bool CheckHit(const Capsule& collider);
	//範囲の制御をする
	float Clamp(
		const float& n,
		const float& min,
		const float& max
	);
	//2つの線分の最短距離の平方を返す
	float ClosestPtSegmentSegment(
		const Capsule& collider,
		const DirectX::SimpleMath::Vector3& bigin,
		const DirectX::SimpleMath::Vector3& end,
		float& s,
		float& t,
		DirectX::SimpleMath::Vector3& c1,
		DirectX::SimpleMath::Vector3& c2
	);
private:
	//判定用カプセル
	Capsule m_capsule;
};