#include "pch.h"
#include "BoxManager.h"

#include "Game/ResourceManager.h"



BoxManager::BoxManager()
	:m_pBox{}
	, m_color(DirectX::Colors::White)
	, m_movingBox(nullptr)
	, m_nullPos(DirectX::SimpleMath::Vector3::Zero)
	, m_state(State::STAY)
	, m_pGoalBox1(nullptr)
	, m_pGoalBox2(nullptr)
	, m_pShadowBox(nullptr)
	, m_changeScene(false)
	, m_isRender(false)
	, m_isVicinityNull(false)
	, m_isVicinityClear(false)
	, m_moveCount(0)
	, m_soundID(SoundID::NONE)
{
}

BoxManager::~BoxManager()
{
}

void BoxManager::Initialize(const int emptyBoxIndex, const int goalBoxIndex_1, const int goalBoxIndex_2, const int clearBoxIndex_1, const int clearBoxIndex_2)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//必要な変数の準備
	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();

	//TODO::PlaySceneで作成(?)
	//ゴール位置が分かるボックスの作成
	m_pGoalBox1 = GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());
	m_pGoalBox2 = GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());
	//ResourceManagerのインスタンスを取得
	ResourceManager& resourceManager = ResourceManager::GetRefInstance();
	//各画像の取得
	m_goalframe_1 = resourceManager.GetTexture(eTextureID::GOALBOX_FRAME_1);
	m_goalframe_2 = resourceManager.GetTexture(eTextureID::GOALBOX_FRAME_2);
	//影用ボックス
	m_pShadowBox = GeometricPrimitive::CreateCube(pDR->GetD3DDeviceContext());

	//ボックスの生成と初期化
	m_pBox.resize(3 * 3 * 3); //3*3*3のボックスの配列のサイズ
	for (auto& box : m_pBox)
	{
		box = std::make_unique<Box>();
	}
	//1か所空にする
	Delete(emptyBoxIndex);

	//ボックスの位置
	float posX = -1.05f;
	float posY = -1.05f;
	float posZ = -1.05f;

	//各ボックスの初期位置とタイプの設定
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			if (box.get() == m_pBox[clearBoxIndex_1].get())
			{
				box->Initialize(SimpleMath::Vector3(posX, posY, posZ), Box::Type::CLEAR_1);
				m_clearBox1Pos = box->GetPosition();
			}
			else if (box.get() == m_pBox[clearBoxIndex_2].get())
			{
				box->Initialize(Vector3(posX, posY, posZ), Box::Type::CLEAR_2);
				m_clearBox2Pos = box->GetPosition();
			}
			else
			{
				box->Initialize(Vector3(posX, posY, posZ), Box::Type::NORMAL);
			}

			box->SetPosition(Vector3(posX, posY, posZ));
		}
		else
		{
			//空いている位置
			m_nullPos = Vector3(posX, posY, posZ);
		}

		//ボックスが3×3×3になるように調整
		if (posX < 1.0f)
		{
			posX += 1.05f;
		}
		else if (posY < 1.0f)
		{
			posX = -1.05f;
			posY += 1.05f;
		}
		else
		{
			posX = -1.05f;
			posY = -1.05f;
			posZ += 1.05f;
		}
	}
	//ゴール位置の設定
	m_goalBox1Pos = m_pBox[goalBoxIndex_1]->GetPosition();
	m_goalBox2Pos = m_pBox[goalBoxIndex_2]->GetPosition();
	//エフェクトの生成
	m_boxEffect = std::make_unique<EffectManager>();
	m_boxEffect->Create();

	//コライダーの生成
	m_collider = std::make_unique<CapsuleCollider>();
}

void BoxManager::Update(const DX::StepTimer& timer, DirectX::Mouse::ButtonStateTracker mouseStateTracker, Capsule mouseRay, DebugCamera* camera, float scalingRatio)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	//カメラとの仮の最大距離
	float length = 999.9f;
	//当たっているボックスの保存先
	Box* pHitBox = nullptr;

	switch (m_state)
	{
	//ボックスの状態が動いていなかったら
	case State::STAY:
	{
		m_soundID = SoundID::NONE;
		for (auto& box : m_pBox)
		{
			if (box == nullptr)
			{
				continue;
			}

			//ボックスからカプセルを受け取ってコライダークラスから当たり判定をする
			m_collider->SetCapsule(box->GetCapsule());
			bool isHit = m_collider->CheckHit(mouseRay);

			//マウスとの当たり判定をする
			//bool isHit = box->HitCheck_Capsule2Capsule(mouseRay);
			
			//マウスが当たっていたら
			if (isHit)
			{
				//カメラとの距離を比較する
				float dist = DirectX::SimpleMath::Vector3::Distance(camera->GetEyePosition(), box->GetPosition());
				if (dist < length)
				{
					//カメラに近いものを当たったものと判定する
					pHitBox = box.get();
					length = dist;
				}
			}
		}

		for (auto& box : m_pBox)
		{
			if (box == nullptr)
			{
				continue;
			}

			//カメラと一番近い距離のボックスだったら
			if (box.get() == pHitBox)
			{
				//色を変える
				m_color = Colors::LightCoral;
			}
			else
			{
				//マウスに当たっていない時の色を指定する
				if (box->GetType() == Box::Type::CLEAR_1)
				{
					//クリア条件のボックスはオレンジ色に設定
					m_color = Colors::Orange;
				}
				else if (box->GetType() == Box::Type::CLEAR_2)
				{
					//クリア条件2つ目のボックスはライム色に設定
					m_color = Colors::Lime;
				}
				else
				{
					//その他のボックスは白色に設定
					m_color = Colors::White;
				}
			}

			//ボックスの色の設定
			box->SetColor(m_color);
		}

		//カメラと一番近い距離のボックスだったら
		if (pHitBox != nullptr)
		{
			//クリックされたらボックスを動かす
			if (mouseStateTracker.leftButton == DirectX::Mouse::ButtonStateTracker::PRESSED)
			{
				//ボックスの隣が空いているか判定
				m_isVicinityNull = VicinityNullPos(m_nullPos, pHitBox->GetPosition());

				//隣が空いていたらボックスを動かす準備をする
				if (m_isVicinityNull)
				{
					//移動回数を計算する
					m_moveCount++;
					//動かすボックスを指定する
					m_movingBox = pHitBox;
					//移動のゴール位置の設定
					m_movingBox->SetGoalPos(m_nullPos);
					//ステートをMOVINGに変更
					m_movingBox->SetCurrentState(Box::State::MOVING);

					//もし移動したボックスがクリア条件のボックスだったら
					if (pHitBox->GetType() == Box::Type::CLEAR_1)
					{
						//クリア条件のボックスの位置を取得する
						m_isVicinityClear = VicinityClearPos(m_clearBox1Pos, m_goalBox1Pos);
					}
					else if (pHitBox->GetType() == Box::Type::CLEAR_2)
					{
						//クリア条件のボックスの位置を取得する
						m_isVicinityClear = VicinityClearPos(m_clearBox2Pos, m_goalBox2Pos);
					}
					//ボックスを動かす状態にする
					m_state = State::BOX_MOVING;
				}
				else
				{
					//動けないと分かる音のIDを設定
					m_soundID = SoundID::NOT_MOVE;
				}
			}
		}
		break;
	}
	case State::BOX_MOVING:
	{
		//ボックスの状態が待機状態なら
		if (m_movingBox->GetCurrentState() == Box::State::STAY)
		{
			//エフェクトの初期化と設定
			m_boxEffect->Initialize(
				0.75f, 
				m_movingBox->GetPosition()
			);
			m_boxEffect->SetRenderState(
				camera->GetEyePosition(),
				camera->GetViewMatrix(),
				camera->GetProjectionMatrix()
			);

			if (m_movingBox->GetType() == Box::Type::CLEAR_1)
			{
				//クリア条件のボックスの位置を取得する
				m_clearBox1Pos = m_movingBox->GetPosition();
			}
			else if (m_movingBox->GetType() == Box::Type::CLEAR_2)
			{
				//クリア条件のボックスの位置を取得する
				m_clearBox2Pos = m_movingBox->GetPosition();
			}

			//空く場所を現在の位置に設定する
			m_nullPos = m_movingBox->GetStartPos();
			m_movingBox = nullptr;
			m_state = State::FINISH;
		}
		break;
	}
	case State::FINISH:
		//動いた時の音を設定する
		m_soundID = SoundID::MOVE;

		//クリア条件のボックスとクリア位置が一緒だったら
		if (m_clearBox1Pos == m_goalBox1Pos && m_clearBox2Pos == m_goalBox2Pos)
		{
			//クリアしたのでシーンを変える
			m_changeScene = true;
		}
		
		//ボックスの状態を待機状態にする
		m_state = State::STAY;
		break;
	}

	//ボックスの更新
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			box->Update(timer, scalingRatio);
		}
	}

	//エフェクトの更新
	m_boxEffect->Update(timer);
}

void BoxManager::Render(DebugCamera* camera)
{
	using namespace DirectX;
	using namespace DirectX::SimpleMath;

	DX::DeviceResources* pDR = DX::DeviceResources::GetInstance();
	//ID3D11DeviceContext1* context = pDR->GetD3DDeviceContext();

	//ウィンドウサイズの取得
	float width = static_cast<float>(pDR->GetOutputSize().right);
	float height = static_cast<float>(pDR->GetOutputSize().bottom);

	//変換行列の宣言(ワールド、ビュー、射影(プロジェクション))
	Matrix world, view, projection;

	//ワールド行列の初期化(単位行列)
	world = Matrix::Identity;
	view = camera->GetViewMatrix();						//デバッグカメラを使う

	//プロジェクション
	constexpr float field0fView = XMConvertToRadians(45.0f);	//画角
	float aspectRatio = width / height;                         //画面縦横比
	float nearPlane = 1.0f;										//カメラから一番近い投影面
	float farPlane = 150.0f;									//カメラから一番遠い投影面
	projection = Matrix::CreatePerspectiveFieldOfView
	(field0fView, aspectRatio, nearPlane, farPlane);

	//ボックスの描画
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			box->Render(camera);
		}
	}

	//TODO::PlaySceneに移行
	Matrix goalBox1Pos = Matrix::CreateTranslation(m_goalBox1Pos);
	Matrix goalBox2Pos = Matrix::CreateTranslation(m_goalBox2Pos);
	//クリア位置を示すボックスの描画
	m_pGoalBox1->Draw(goalBox1Pos, view, projection, Color(1.0f, 1.0f, 0.0f, 0.5f), m_goalframe_1.Get());
	m_pGoalBox2->Draw(goalBox2Pos, view, projection, Color(0.0f, 1.0f, 0.0f, 0.25f), m_goalframe_2.Get());
	//影用ボックスの描画
	m_pShadowBox->Draw(Matrix::CreateTranslation(Vector3::Zero), view, projection, Color(0.5f, 0.5f, 0.5f, 0.5f));
	m_boxEffect->Render();
}

void BoxManager::Finalize()
{
	//ボックスの削除
	for (auto& box : m_pBox)
	{
		if (box != nullptr)
		{
			box.reset();
			box->Finalize();
		}
	}

	//ボックスの配列を破棄する
	m_pBox.clear();
}

//============================================================================
//ボックスの一部を削除する
//============================================================================
void BoxManager::Delete(int element)
{
	m_pBox[element].reset();
}

//============================================================================
//ボックスの隣が空いているか判定する
//============================================================================
bool BoxManager::VicinityNullPos(const DirectX::SimpleMath::Vector3& nullPos, const DirectX::SimpleMath::Vector3& boxPos)
{
	//空いている場所のx座標、y座標、ｚ座標が同じところを絞る
	if (nullPos.x == boxPos.x && nullPos.y == boxPos.y ||
		nullPos.y == boxPos.y && nullPos.z == boxPos.z ||
		nullPos.z == boxPos.z && nullPos.x == boxPos.x)
	{
			//選択したボックスの隣がnullPosだったら
		if (boxPos.x - 1.05f == nullPos.x ||
			boxPos.x + 1.05f == nullPos.x ||
			boxPos.y - 1.05f == nullPos.y ||
			boxPos.y + 1.05f == nullPos.y ||
			boxPos.z - 1.05f == nullPos.z ||
			boxPos.z + 1.05f == nullPos.z)
		{
			return true;
		}
	}
	return false;
}

//============================================================================
//クリア用ボックスの隣がゴール位置なのか判定する
//============================================================================
bool BoxManager::VicinityClearPos(const DirectX::SimpleMath::Vector3& colorBoxPos, const DirectX::SimpleMath::Vector3& clearPos)
{
	//クリア条件となるボックスの隣がクリアボックスだったら
	if (clearPos.x == colorBoxPos.x && clearPos.y == colorBoxPos.y ||
		clearPos.y == colorBoxPos.y && clearPos.z == colorBoxPos.z ||
		clearPos.z == colorBoxPos.z && clearPos.x == colorBoxPos.x)
	{
		if (colorBoxPos.x - 1.05f == clearPos.x ||
			colorBoxPos.x + 1.05f == clearPos.x ||
			colorBoxPos.y - 1.05f == clearPos.y ||
			colorBoxPos.y + 1.05f == clearPos.y ||
			colorBoxPos.z - 1.05f == clearPos.z ||
			colorBoxPos.z + 1.05f == clearPos.z)
		{
			return true;
		}
	}
	return false;
}
