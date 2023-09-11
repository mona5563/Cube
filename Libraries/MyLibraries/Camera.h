//
// Camera.h
//
#pragma once

#include <SimpleMath.h>

/// <summary>
/// デバッグ用カメラクラス
/// </summary>
class Camera
{
public:
	static const float DEFAULT_CAMERA_DISTANCE;	// カメラの距離

private:

	// ビュー行列
	DirectX::SimpleMath::Matrix m_view;

	// プロジェクション行列
	DirectX::SimpleMath::Matrix m_projection;

	// カメラ座標
	DirectX::SimpleMath::Vector3 m_eye;

	// 注視点
	DirectX::SimpleMath::Vector3 m_target;

	// 上向きベクトル
	DirectX::SimpleMath::Vector3 m_up;

public:
	// コンストラクタ
	Camera();

	// デストラクタ
	~Camera();

	// ビュー行列のアクセサ
	void SetViewMatrix(DirectX::SimpleMath::Matrix view) { m_view = view; }
	DirectX::SimpleMath::Matrix GetViewMatrix() const { return m_view; }

	// プロジェクション行列のアクセサ
	void SetProjectionMatrix(DirectX::SimpleMath::Matrix projection) { m_projection = projection; }
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const { return m_projection; }

	// カメラ座標のアクセサ
	void SetEyePosition(DirectX::SimpleMath::Vector3 eye) { m_eye = eye; }
	DirectX::SimpleMath::Vector3 GetEyePosition() const { return m_eye; }

	// 注視点座標のアクセサ
	void SetTargetPosition(DirectX::SimpleMath::Vector3 target) { m_target = target; }
	DirectX::SimpleMath::Vector3 GetTargetPosition() const { return m_target; }

	// 上向きベクトルのアクセサ
	void SetUpVector(DirectX::SimpleMath::Vector3 up) { m_up = up; }
	DirectX::SimpleMath::Vector3 GetUpVector() const { return m_up; }

protected:

	// ビュー行列の算出
	virtual void CalculateViewMatrix();

	// プロジェクション行列の算出
	virtual void CalculateProjectionMatrix();
};
