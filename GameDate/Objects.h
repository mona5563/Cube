#pragma once
#ifndef OBJECTS_DEFINED
#define OBJECTS_DEFINED

#include <SimpleMath.h>

//	���̍\����
struct Sphere
{
	DirectX::SimpleMath::Vector3 c;		//	���̒��S
	float r;							//	���̔��a
};

//	�J�v�Z���̍\����
struct Capsule
{
	Capsule():bigin{}, end{}, radius{}{}
	DirectX::SimpleMath::Vector3 bigin;		//	�c�̐����̊J�n�_
	DirectX::SimpleMath::Vector3 end;		//	�c�̐����̏I���_
	float radius;							//	���a
};

#endif		// OBJECTS_DEFINED
