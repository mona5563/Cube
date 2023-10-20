//
// DebugCamera.cpp
//
#include "pch.h"
#include "DebugCamera.h"

// 定数の初期化
const float DebugCamera::DEFAULT_CAMERA_DISTANCE = 5.0f;

// コンストラクタ
DebugCamera::DebugCamera()
	: Camera()
	, m_angleX(0.0f)
	, m_angleY(0.0f)
	, m_prevX(0)
	, m_prevY(0)
	, m_scrollWheelValue(0)
	, m_oldUp(DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f))
	, m_nowRotateQuaternion()
{
}

// デストラクタ
DebugCamera::~DebugCamera()
{
}

//-------------------------------------------------------------------
// 更新処理
//-------------------------------------------------------------------
void DebugCamera::Update()
{
	// どこかでMouseインスタンスが作られていれば、マウスの状態が取得できる(参考：Impl)
	DirectX::Mouse::State state = DirectX::Mouse::Get().GetState();


	// マウスの右クリック＆ドラッグでカメラ座標を更新する
	if (state.rightButton)
	{
		DraggedDistance(state.x, state.y);
	}

	// マウスの座標を前回の値として保存
	m_prevX = state.x;
	m_prevY = state.y;

	//// マウスホイールのスクロール値を取得
	//m_scrollWheelValue = state.scrollWheelValue;
	//if (m_scrollWheelValue > 0)
	//{
	//	m_scrollWheelValue = 0;
	//	DirectX::Mouse::Get().ResetScrollWheelValue();
	//}

	// ビュー行列の算出
	CalculateViewMatrix();
}

//-------------------------------------------------------------------
// マウスポインタのドラッグ開始位置からの変位(相対値)
//-------------------------------------------------------------------
void DebugCamera::DraggedDistance(int x, int y)
{
	// マウスポインタの前回からの変位
	// なお、0.2fは適当な補正値 -> ドラッグの移動量を調整する
	float dx = static_cast<float>(x - m_prevX) * 0.2f;
	float dy = static_cast<float>(y - m_prevY) * 0.2f;

	if (dx != 0.0f || dy != 0.0f)
	{
		// マウスポインタの変位を元に、Ｘ軸Ｙ軸の回転角を求める
		// XとYの関係に注意！！
		float angleX = dy * DirectX::XM_PI / 180.0f;
		float angleY = dx * DirectX::XM_PI / 180.0f;

		DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(angleY);
		DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(angleX);

		DirectX::SimpleMath::Matrix rotY2 = DirectX::SimpleMath::Matrix::CreateRotationY(m_angleY);
		DirectX::SimpleMath::Matrix rotX2 = DirectX::SimpleMath::Matrix::CreateRotationX(m_angleX);

		//	各軸の回転を合成する
		DirectX::SimpleMath::Matrix rt = rotY * rotX;

		//	現在のカメラの向きから、Y方向の回転用軸ベクトルを作成
		DirectX::SimpleMath::Vector3    eye(0.0f, 0.0f, 10.7f);
		DirectX::SimpleMath::Matrix nowPositionMatrinx = DirectX::SimpleMath::Matrix::CreateTranslation(eye);
		DirectX::SimpleMath::Matrix nowRotateMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_nowRotateQuaternion);
		DirectX::SimpleMath::Matrix nowEyeMatrinx = nowPositionMatrinx * nowRotateMatrix;
		eye = DirectX::SimpleMath::Vector3(nowEyeMatrinx._41, nowEyeMatrinx._42, nowEyeMatrinx._43);

		DirectX::SimpleMath::Vector3 yRotateAxis = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f) - eye;

		yRotateAxis = yRotateAxis.Cross(m_oldUp);

		//	各軸の回転クォータニオンを作成
		DirectX::SimpleMath::Quaternion yRotateQuaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(m_oldUp, -angleY);
		DirectX::SimpleMath::Quaternion xRotateQuaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(yRotateAxis, -angleX);

		//	現在の回転角度に新しい観点角度を反映する
		m_nowRotateQuaternion *= yRotateQuaternion;
		m_nowRotateQuaternion *= xRotateQuaternion;
	}
}

//-------------------------------------------------------------------
// ビュー行列の算出と、カメラ座標・注視点の取得
//-------------------------------------------------------------------
void DebugCamera::CalculateViewMatrix()
{
	//	現在の回転角をクォータニオンから算出
	DirectX::SimpleMath::Matrix nowRotateMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_nowRotateQuaternion);

	// ビュー行列を算出する
	DirectX::SimpleMath::Matrix rotY = DirectX::SimpleMath::Matrix::CreateRotationY(m_angleY);
	DirectX::SimpleMath::Matrix rotX = DirectX::SimpleMath::Matrix::CreateRotationX(m_angleX);

	DirectX::SimpleMath::Matrix rt = rotY * rotX;

	DirectX::SimpleMath::Vector3    eye(0.0f, 0.0f, 10.7f);
	DirectX::SimpleMath::Vector3 target(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3     up(0.0f, 1.0f, 0.0f);

	DirectX::SimpleMath::Matrix nowPositionMatrinx = DirectX::SimpleMath::Matrix::CreateTranslation(eye);
	DirectX::SimpleMath::Matrix nowEyeMatrinx = nowPositionMatrinx * nowRotateMatrix;
	DirectX::SimpleMath::Matrix nowUpMatrinx = DirectX::SimpleMath::Matrix::CreateTranslation(up) * nowRotateMatrix;

	SetEyePosition(eye = DirectX::SimpleMath::Vector3(nowEyeMatrinx._41, nowEyeMatrinx._42, nowEyeMatrinx._43));
	SetTargetPosition(target);
	SetUpVector(up = DirectX::SimpleMath::Vector3(nowUpMatrinx._41, nowUpMatrinx._42, nowUpMatrinx._43));

	SetViewMatrix(DirectX::SimpleMath::Matrix::CreateLookAt(eye, target, up));

	m_oldUp = up;
}


void DebugCamera::ComputeAnglesFromMatrix(
	DirectX::SimpleMath::Matrix R,
	double& angle_x,
	double& angle_y,
	double& angle_z
)
{

	double threshold = 0.001;

	if (abs(R._32 - 1.0) < threshold)
	{
		// R(2,1) = sin(x) = 1の時
		angle_x = 3.1415926545f / 2;
		angle_y = 0;
		angle_z = atan2(R._21, R._11);
	}
	else if (abs(R._32 + 1.0) < threshold)
	{
		// R(2,1) = sin(x) = -1の時
		angle_x = -3.1415926545f / 2;
		angle_y = 0;
		angle_z = atan2(R._21, R._11);
	}
	else

	{
		angle_x = asin(R._32);
		angle_y = atan2(-R._31, R._33);
		angle_z = atan2(-R._12, R._22);
	}
}

