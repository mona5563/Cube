#pragma once
#ifndef OBJECTS_DEFINED
#define OBJECTS_DEFINED

#include <SimpleMath.h>

//	球の構造体
struct Sphere
{
	DirectX::SimpleMath::Vector3 c;		//	球の中心
	float r;							//	球の半径
};

//	カプセルの構造体
struct Capsule
{
	Capsule():bigin{}, end{}, radius{}{}
	DirectX::SimpleMath::Vector3 bigin;		//	芯の線分の開始点
	DirectX::SimpleMath::Vector3 end;		//	芯の線分の終了点
	float radius;							//	半径
};

#endif		// OBJECTS_DEFINED
