//
//Box.h
//
#pragma once

#include <GeometricPrimitive.h>
#include <SimpleMath.h>

#include "GameDate/Objects.h"

//前方宣言
class Camera;

//ボックスクラス
class Box
{
public:
	//ボックスの状態
	enum class State
	{
		STAY,		//動いていない
		MOVING,		//動いている
	};
	//ボックスのタイプ
	enum class Type
	{
		NORMAL,		//白いボックス
		CLEAR_1,	//1と描かれたクリア用ボックス
		CLEAR_2		//2と描かれたクリア用ボックス
	};

public:
	//位置を取得する
	const DirectX::SimpleMath::Vector3& GetPosition() const;
	//位置を設定する
	void SetPosition(const DirectX::SimpleMath::Vector3& position);
	//色を取得する
	const DirectX::SimpleMath::Color& GetColor() const;
	//色を設定する
	void SetColor(const DirectX::SimpleMath::Color& color);
	//スタート位置を取得する
	DirectX::SimpleMath::Vector3 GetStartPos() const;
	//スタート位置を設定する
	void SetStartPos(const DirectX::SimpleMath::Vector3& startPos);
	//ゴール位置を取得する
	DirectX::SimpleMath::Vector3 GetGoalPos() const;
	//ゴール位置を設定する
	void SetGoalPos(const DirectX::SimpleMath::Vector3& goalPos);
	//Typeを取得する
	Type GetType() { return m_type; }
	// 状態を取得する
	const State& GetCurrentState() const
	{
		return m_state;
	}
	// 状態を設定する
	void SetCurrentState(const State& state)
	{
		m_state = state;
	}
	//カプセルを取得する
	Capsule GetCapsule() const { return m_hitCapsule; }

public:
	//コンストラクタ
	Box();
	//デストラクタ
	~Box();
	//初期化する
	void Initialize(const DirectX::SimpleMath::Vector3& cupsulePos, Type type);
	//更新する
	void Update(const DX::StepTimer& timer, float scalingRatio);
	//描画する
	void Render(const Camera* pCamera);
	//後処理する
	void Finalize();

	//線形補間
	float Lerp(
		const float& start,
		const float& end,
		const float& t
	);
	//イージング関数
	float easeOutBounce(const float& x);
	float easeInBounce(const float& x);

	////カプセルとカプセルの衝突判定をする
	//float ClosestPtSegmentSegment(
	//	const DirectX::SimpleMath::Vector3& mouseBegin,
	//	const DirectX::SimpleMath::Vector3& mouseEnd,
	//	float& s,
	//	float& t,
	//	DirectX::SimpleMath::Vector3& c1,
	//	DirectX::SimpleMath::Vector3& c2
	//);
	////Clamp関数
	//float Clamp(const float& n, const float& min, const float& max);
	////2つの線分の最短距離の平方を返す
	//bool  HitCheck_Capsule2Capsule(const Capsule& mouse);

private:
	//移動にかかる時間(定数)
	static const float MOVE_TIME;

private:
	//ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_pBox;
	//位置
	DirectX::SimpleMath::Vector3 m_position;
	//色
	DirectX::SimpleMath::Color m_color;
	//当たり判定用のレイ(カプセル)
	Capsule m_hitCapsule;
	//ヒット状態を記録するフラグ
	float m_hit;
	//移動時間
	float m_moveTime;
	//経過時間
	float m_time;
	// 状態
	State m_state;
	//タイプ
	Type  m_type;
	//スタート位置
	DirectX::SimpleMath::Vector3  m_startPos;
	//ゴール位置
	DirectX::SimpleMath::Vector3  m_goalPos;
	//ルーチン
	int m_boxRoutine;
	//ジオメトリックプリミティブに張り付けるテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_frame; //外枠
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearframe_1; //1のついたもの
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_clearframe_2; //2のついたもの
	//線形補間時の位置
	DirectX::SimpleMath::Vector3 m_renderPos;
};
 