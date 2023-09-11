#include "pch.h"
#include "Collision.h"

/*--------------------------------------------------
判定用カプセルを設定する(ボックスのカプセル)
--------------------------------------------------*/
void CapsuleCollider::SetCapsule(Capsule capsule)
{
	m_capsule = capsule;
}

/*--------------------------------------------------
コンストラクタ
--------------------------------------------------*/
CapsuleCollider::CapsuleCollider()
	:
	m_capsule{}
{
}

/*--------------------------------------------------
デストラクタ
--------------------------------------------------*/
CapsuleCollider::~CapsuleCollider()
{
}

/*--------------------------------------------------
他のカプセルとの当たり判定をする
--------------------------------------------------*/
bool CapsuleCollider::CheckHit(const Capsule& collider)
{
	float s, t;
	DirectX::SimpleMath::Vector3 c1, c2;
	//	カプセルの中心の線分間の距離の平方を計算
	//	s, t, c1, c2は戻り値
	float dist2 = ClosestPtSegmentSegment(collider, m_capsule.bigin, m_capsule.end, s, t, c1, c2);
	float radius = collider.radius + m_capsule.radius;
	return dist2 <= radius * radius;
}

/*--------------------------------------------------
範囲の制御をする
--------------------------------------------------*/
float CapsuleCollider::Clamp(
	const float& n,
	const float& min,
	const float& max
)
{
	if (n < min) return min;
	if (n > max) return max;
	return n;
}

/*--------------------------------------------------
//2つの線分の最短距離の平方を返す
--------------------------------------------------*/
float CapsuleCollider::ClosestPtSegmentSegment(
	const Capsule& collider,
	const DirectX::SimpleMath::Vector3& bigin,
	const DirectX::SimpleMath::Vector3& end,
	float& s,
	float& t,
	DirectX::SimpleMath::Vector3& c1,
	DirectX::SimpleMath::Vector3& c2
)
{
	//	p1→q1のベクトルを算出
	DirectX::SimpleMath::Vector3 biginVec = collider.end - collider.bigin;
	//	p2→q2のベクトルを算出
	DirectX::SimpleMath::Vector3 endVec = end - bigin;
	//	二つのカプセルが持つ線分の、始点間のベクトルを半径の合計として算出
	DirectX::SimpleMath::Vector3 r = collider.bigin - bigin;

	//	p1→q1のベクトルの長さの2乗を算出
	float a = biginVec.Dot(biginVec);
	//	p2→q2のベクトルの長さの2乗を算出
	float e = endVec.Dot(endVec);
	//	p1→q1のベクトルと半径の合計となるベクトルを内積する
	float f = endVec.Dot(r);

	if (a <= FLT_EPSILON && e <= FLT_EPSILON)
	{
		//	aとeの両方がほぼ0だったら、二つのカプセルは平行である
		s = t = 0.0f;

		//	仮として各始点を獲得する
		c1 = collider.bigin;
		c2 = bigin;
		//	仮取得した点同士の距離を求める
		return (c1 - c2).Dot(c1 - c2);
	}
	if (a <= FLT_EPSILON)
	{
		//	aがほぼ0だった場合は、f/eをtとして計算する
		//	ただし、0～1の間に補正する
		s = 0.0f;
		t = f / e;
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		//	p1→q1のベクトルと半径の合計を内積する
		float c = biginVec.Dot(r);
		if (e <= FLT_EPSILON)
		{
			//	eがほぼ0だった場合は、sとして-c/aを計算する。
			//	ただし、0～1の間に補正する
			t = 0.0f;
			s = Clamp(-c / a, 0.0f, 1.0f);
		}
		else
		{
			//	特に0除算が起きる危険はないので、式通りの計算を行う
			//	各カプセルのベクトルで内積する
			float b = biginVec.Dot(endVec);
			//	それぞれのカプセルの長さの2乗を掛けた値と、
			//	各カプセルのベクトルで内積した結果の2乗を比べる
			float denom = a * e - b * b;
			if (denom != 0.0f)
			{
				//	長さが違っていれば、式通りの計算を行う
				s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
			}
			else
			{
				//	二つのカプセルの向きがほぼ90度の場合、sを0とする
				s = 0.0f;
			}

			//	b * s + f を計算して検証する
			float tnom = (b * s + f);
			if (tnom < 0.0f)
			{
				//	マイナスの値になっていたら、-c / aの値をsとする
				t = 0.0f;
				s = Clamp(-c / a, 0.0f, 1.0f);
			}
			else if (tnom > e)
			{
				//	eより大きければ、(b - c) / aの値をsとする
				t = 1.0f;
				s = Clamp((b - c) / a, 0.0f, 1.0f);
			}
			else
			{
				//	式通りの計算を行う
				t = tnom / e;
			}
		}
	}

	//	それぞれのカプセルにおける、一番近い点を算出する
	c1 = collider.bigin + biginVec * s;
	c2 = bigin + endVec * t;

	//	一番近い二点の距離の二乗を返す
	return (c1 - c2).Dot(c1 - c2);
}
