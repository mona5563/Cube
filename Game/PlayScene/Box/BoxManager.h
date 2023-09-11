#pragma once

#include "Box.h"
#include "Game/Shader/EffectManager.h"
#include "GameDate/Collision.h"

struct Capsule;
class DebugCamera;
class CapsuleCollider;

class BoxManager
{
public:
	//ボックスの状態
	enum class State
	{
		STAY,
		BOX_MOVING,
		FINISH
	};

	//SE
	enum class SoundID
	{
		MOVE,
		NOT_MOVE,
		NONE,
	};

public:
	//隣が空いているか取得する
	bool GetVicinityNull() const { return m_isVicinityNull; }
	//隣がゴールなのか取得する
	bool GetVicinityClear() const { return m_isVicinityClear; }
	//移動数を取得する
	int GetMovingCount() const { return m_moveCount; }
	//効果音を取得する
	SoundID GetSoundID() const { return m_soundID; }
	//シーン遷移フラグを取得する
	bool GetChangeScene() const { return m_changeScene; }

public:
	//コンストラクタ
	BoxManager();
	//デストラクタ
	~BoxManager();
	//初期化する
	void Initialize(
		const int emptyBoxIndex,
		const int goalBoxIndex_1,
		const int goalBoxIndex_2,
		const int clearBoxIndex_1,
		const int clearBoxIndex_2
	);
	//更新する
	void Update(
		const DX::StepTimer& timer,
		DirectX::Mouse::ButtonStateTracker mouseStateTracker,
		Capsule mouseRay,
		DebugCamera* camera,
		float scalingRatio
	);
	//描画する
	void Render(DebugCamera* camera);
	//終了処理する
	void Finalize();
	//ボックスの一部を削除する
	void Delete(int element);
	//隣が空いているか判定する
	bool VicinityNullPos(
		const DirectX::SimpleMath::Vector3&  nullPos,
		const DirectX::SimpleMath::Vector3& boxPos
	);
	//隣がクリア条件のボックスなのか判定する
	bool VicinityClearPos(
		const DirectX::SimpleMath::Vector3& colorBoxPos,
		const DirectX::SimpleMath::Vector3& clearPos
	);

private:
	//ボックス
	std::vector<std::unique_ptr<Box>> m_pBox;
	//色
	DirectX::SimpleMath::Color m_color;
	//動いているフラグ
	Box* m_movingBox;
	//空いているボックスの場所
	DirectX::SimpleMath::Vector3  m_nullPos;
	//ボックスの状態
	State m_state;
	//クリアとなるボックスの位置
	DirectX::SimpleMath::Vector3 m_clearBox1Pos;
	DirectX::SimpleMath::Vector3 m_clearBox2Pos;
	//外枠のテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_goalframe_1;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_goalframe_2;
	//ゴールとなるボックスの位置
	DirectX::SimpleMath::Vector3 m_goalBox1Pos;
	DirectX::SimpleMath::Vector3 m_goalBox2Pos;
	//クリアとなるボックスのジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_pGoalBox1;
	std::unique_ptr<DirectX::GeometricPrimitive> m_pGoalBox2;
	//中心影用ボックス
	std::unique_ptr<DirectX::GeometricPrimitive> m_pShadowBox;
	//シーン変更用フラグ
	bool m_changeScene;
	//矢印を描画するかのフラグ
	bool m_isRender;
	//隣が空いた場所かのフラグ
	bool m_isVicinityNull;
	//クリア条件となるボックスとクリアボックスのフラグ
	bool m_isVicinityClear;
	//移動回数
	int m_moveCount;
	//音
	SoundID m_soundID;
	//星のエフェクト
	std::unique_ptr<EffectManager> m_boxEffect;
	//コライダー
	std::unique_ptr<CapsuleCollider> m_collider;
};