//
//Box.cpp
//
#include "pch.h"
#include "Box.h"

#include "Game/ResourceManager.h"


//動く時間
const float Box::MOVE_TIME = 1.0f;


//============================================================================
// コンストラクタ
//============================================================================

Box::Box()
	:
	m_pBox(nullptr),
	m_color(DirectX::Colors::White),
	m_hit(false),
	m_moveTime(0.0f),
	m_time(0.0f),
	m_state(State::STAY),
	m_type(Type::NORMAL),
	m_boxRoutine(0),
	m_renderPos(DirectX::SimpleMath::Vector3::Zero)
{	
	//カプセルの半径は0.55とする
	m_hitCapsule.radius = 0.55f;
}

//============================================================================
// デストラクタ
//============================================================================
Box::~Box()
{
	Finalize();
}

//============================================================================
// 初期化する
//============================================================================
void Box::Initialize(const DirectX::SimpleMath::Vector3& cupsulePos, Type type)
{
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//外枠のテクスチャの取得
	m_frame = resourceManager.GetTexture(eTextureID::BOX_FRAME);
	m_clearframe_1 = resourceManager.GetTexture(eTextureID::CLEARBOX_FRAME_1);
	m_clearframe_2 = resourceManager.GetTexture(eTextureID::CLEARBOX_FRAME_2);

	//ジオメトリックプリミティブの作成
	m_pBox = DirectX::GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());

	//マウスレイの当たり判定の初期化
	m_hit = false;

	//	カプセルの上側を始点とする
	m_hitCapsule.bigin.x = cupsulePos.x + 0.15f;
	m_hitCapsule.bigin.y = cupsulePos.y + 0.15f;
	m_hitCapsule.bigin.z = cupsulePos.z + 0.15f;

	//	カプセルの下側を終点とする
	m_hitCapsule.end.x = cupsulePos.x - 0.15f;
	m_hitCapsule.end.y = cupsulePos.y - 0.15f;
	m_hitCapsule.end.z = cupsulePos.z - 0.15f;

	//タイプの設定
	m_type = type;
}

//============================================================================
// 更新する
//============================================================================
void Box::Update(const DX::StepTimer& timer, float scalingRatio)
{
	//色を取得する
	m_color = GetColor();
	//移動終了場所の取得をする
	m_goalPos = GetGoalPos();

	if (m_state == State::MOVING)
	{
		//経過時間を取得する
		m_time = (float)timer.GetElapsedSeconds();
		//移動時間に反映する
		m_moveTime += m_time;
		//移動完了後
		if (m_moveTime > MOVE_TIME)
		{
			//移動開始位置を移動後の座標に指定する
			m_startPos = m_position;
			//座標を移動後の位置に指定する
			m_position = m_goalPos;

			//判定用カプセルも移動させる
			//カプセルの上側を始点とする
			m_hitCapsule.bigin.x = (m_position.x + 0.15f) * scalingRatio;
			m_hitCapsule.bigin.y = (m_position.y + 0.15f) * scalingRatio;
			m_hitCapsule.bigin.z = (m_position.z + 0.15f) * scalingRatio;
			//カプセルの下側を終点とする
			m_hitCapsule.end.x = (m_position.x - 0.15f) * scalingRatio;
			m_hitCapsule.end.y = (m_position.y - 0.15f) * scalingRatio;
			m_hitCapsule.end.z = (m_position.z - 0.15f) * scalingRatio;

			//移動が完了したため、状態を待ちにする
			m_state = State::STAY;
			//移動時間をリセットする
			m_moveTime = 0.0f;
		}
	}
}

//============================================================================
// 描画する
//============================================================================
void Box::Render(const Camera* pCamera)
{
	//名前空間
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//変換行列
	Matrix world = Matrix::Identity;		//ワールド行列
	Matrix view = pCamera->GetViewMatrix();				//ビュー行列
	Matrix projection = pCamera->GetProjectionMatrix();	//射影行列
	
	//イージング関数を使用し、ボックスが移動するよう描画する
	m_renderPos = Vector3::Lerp(
		m_position,
		m_goalPos,
		easeInBounce(m_moveTime / MOVE_TIME)
	);
	world = Matrix::CreateTranslation(m_renderPos);

	//ボックスの描画(タイプによってフレーム画像を変える)
	switch (m_type)
	{
	case Box::Type::NORMAL:
		m_pBox->Draw(
			world,
			view,
			projection,
			m_color,
			m_frame.Get()
		);
		break;
	case Box::Type::CLEAR_1:
		m_pBox->Draw(
			world, 
			view,
			projection,
			m_color,
			m_clearframe_1.Get()
		);
		break;
	case Box::Type::CLEAR_2:
		m_pBox->Draw(
			world,
			view,
			projection,
			m_color,
			m_clearframe_2.Get()
		);
		break;
	}
	
}



//============================================================================
//後始末する
//============================================================================
void Box::Finalize()
{
}

//============================================================================
//位置を取得する
//============================================================================
const DirectX::SimpleMath::Vector3& Box::GetPosition() const
{
	return m_position;
}

//============================================================================
//位置を設定する
//============================================================================
void Box::SetPosition(const DirectX::SimpleMath::Vector3& position)
{
	m_position = position;
}

//============================================================================
//色を取得する
//============================================================================
const DirectX::SimpleMath::Color& Box::GetColor() const
{
	return m_color;
}

//============================================================================
//色を設定する
//============================================================================
void Box::SetColor(const DirectX::SimpleMath::Color& color)
{
	m_color = color;
}

//============================================================================
//移動開始位置を取得する
//============================================================================
DirectX::SimpleMath::Vector3 Box::GetStartPos() const
{
	return m_startPos;
}
//============================================================================
//移動開始位置を設定する
//============================================================================
void Box::SetStartPos(const DirectX::SimpleMath::Vector3& startPos)
{
	m_startPos = startPos;
}
//============================================================================
//移動後位置を取得する
//============================================================================
DirectX::SimpleMath::Vector3 Box::GetGoalPos() const
{
	return m_goalPos;
}
//============================================================================
//移動後位置を設定する
//============================================================================
void Box::SetGoalPos(const DirectX::SimpleMath::Vector3& goalPos)
{
	m_goalPos = goalPos;
}

//============================================================================
//線形補間
//============================================================================
float Box::Lerp(const float& start, const float& end, const float& t)
{
	return start + (end - start) * t;
}

//============================================================================
//イージング関数
//============================================================================
float Box::easeOutBounce(const float& x)
{
	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	float x1 = x;

	if (x1 < 1.0f / d1) {
		return n1 * x1 * x1;
	}
	else if (x1 < 2.0f / d1) {
		return n1 * (x1 -= 1.5f / d1) * x1+ 0.75f;
	}
	else if (x1 < 2.5f / d1) {
		return n1 * (x1 -= 2.25f / d1) * x1 + 0.9375f;
	}
	else {
		return n1 * (x1 -= 2.625f / d1) * x1 + 0.984375f;
	}
}

float Box::easeInBounce(const float& x)
{
	return 1.0f - easeOutBounce(1.0f - x);
}

////============================================================================
////当たり判定
////============================================================================
//bool Box::HitCheck_Capsule2Capsule(const Capsule& mouse)
//{
//	float s, t;
//	DirectX::SimpleMath::Vector3 c1, c2;
//	//	カプセルの中心の線分間の距離の平方を計算
//	//	s, t, c1, c2は戻り値
//	float dist2 = ClosestPtSegmentSegment(mouse.bigin, mouse.end, s, t, c1, c2);
//	float radius = m_hitCapsule.radius + mouse.radius;
//	return dist2 <= radius * radius;
//}
//
////============================================================================
////範囲制御
////============================================================================
//float Box::Clamp(const float& n, const float& min, const float& max)
//{
//	if (n < min) return min;
//	if (n > max) return max;
//	return n;
//}
//
////============================================================================
////２つの線分の最短距離の平方を返す関数
////============================================================================
//float Box::ClosestPtSegmentSegment(
//	const DirectX::SimpleMath::Vector3& mouseBigin, const DirectX::SimpleMath::Vector3& mouseEnd,
//	float& s, float& t, DirectX::SimpleMath::Vector3& c1, DirectX::SimpleMath::Vector3& c2)
//{
//	//	p1→q1のベクトルを算出
//	DirectX::SimpleMath::Vector3 biginVec = m_hitCapsule.end - m_hitCapsule.bigin;
//	//	p2→q2のベクトルを算出
//	DirectX::SimpleMath::Vector3 endVec = mouseEnd - mouseBigin;
//	//	二つのカプセルが持つ線分の、始点間のベクトルを半径の合計として算出
//	DirectX::SimpleMath::Vector3 r = m_hitCapsule.bigin - mouseBigin;
//
//	//	p1→q1のベクトルの長さの2乗を算出
//	float a = biginVec.Dot(biginVec);
//	//	p2→q2のベクトルの長さの2乗を算出
//	float e = endVec.Dot(endVec);
//	//	p1→q1のベクトルと半径の合計となるベクトルを内積する
//	float f = endVec.Dot(r);
//
//	if (a <= FLT_EPSILON && e <= FLT_EPSILON)
//	{
//		//	aとeの両方がほぼ0だったら、二つのカプセルは平行である
//		s = t = 0.0f;
//
//		//	仮として各始点を獲得する
//		c1 = m_hitCapsule.bigin;
//		c2 = mouseBigin;
//		//	仮取得した点同士の距離を求める
//		return (c1 - c2).Dot(c1 - c2);
//	}
//	if (a <= FLT_EPSILON)
//	{
//		//	aがほぼ0だった場合は、f/eをtとして計算する
//		//	ただし、0～1の間に補正する
//		s = 0.0f;
//		t = f / e;
//		t = Clamp(t, 0.0f, 1.0f);
//	}
//	else
//	{
//		//	p1→q1のベクトルと半径の合計を内積する
//		float c = biginVec.Dot(r);
//		if (e <= FLT_EPSILON)
//		{
//			//	eがほぼ0だった場合は、sとして-c/aを計算する。
//			//	ただし、0～1の間に補正する
//			t = 0.0f;
//			s = Clamp(-c / a, 0.0f, 1.0f);
//		}
//		else
//		{
//			//	特に0除算が起きる危険はないので、式通りの計算を行う
//			//	各カプセルのベクトルで内積する
//			float b = biginVec.Dot(endVec);
//			//	それぞれのカプセルの長さの2乗を掛けた値と、
//			//	各カプセルのベクトルで内積した結果の2乗を比べる
//			float denom = a * e - b * b;
//			if (denom != 0.0f)
//			{
//				//	長さが違っていれば、式通りの計算を行う
//				s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
//			}
//			else
//			{
//				//	二つのカプセルの向きがほぼ90度の場合、sを0とする
//				s = 0.0f;
//			}
//
//			//	b * s + f を計算して検証する
//			float tnom = (b * s + f);
//			if (tnom < 0.0f)
//			{
//				//	マイナスの値になっていたら、-c / aの値をsとする
//				t = 0.0f;
//				s = Clamp(-c / a, 0.0f, 1.0f);
//			}
//			else if (tnom > e)
//			{
//				//	eより大きければ、(b - c) / aの値をsとする
//				t = 1.0f;
//				s = Clamp((b - c) / a, 0.0f, 1.0f);
//			}
//			else
//			{
//				//	式通りの計算を行う
//				t = tnom / e;
//			}
//		}
//	}
//
//	//	それぞれのカプセルにおける、一番近い点を算出する
//	c1 = m_hitCapsule.bigin + biginVec * s;
//	c2 = mouseBigin + endVec * t;
//
//	//	一番近い二点の距離の二乗を返す
//	return (c1 - c2).Dot(c1 - c2);
//}
//
