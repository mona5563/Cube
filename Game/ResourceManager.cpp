/*
	ResourceManager.cpp
	制作日：2023.04.18
	制作者：加藤穂乃佳
*/
#include "pch.h"
#include "ResourceManager.h"

//============================================================================
// テクスチャの生成
//============================================================================
bool ResourceManager::CreateTextures()
{
	//一時的な画像ファイルリスト
	std::wstring fileList[] =
	{
		L"Resources/Textures/NextScene.png",	//シーン切り替えキー
		L"Resources/Textures/NextPlay.png",		//シーン切り替えクリック
		L"Resources/Textures/Fade.png",			//フェード
		L"Resources/Textures/Explanation.png",	//説明書
		L"Resources/Textures/Back.png",			//背景
		L"Resources/Textures/Title.png",		//タイトルロゴ
		L"Resources/Textures/Number.png",		//数字
		L"Resources/Textures/MoveCount.png",	//MoveCount文字
		L"Resources/Textures/Operation.png",	//操作UI
		L"Resources/Textures/Tab.png",			//TAB操作
		L"Resources/Textures/TabClose.png",		//TAB操作終了
		L"Resources/Textures/star.png",			//星エフェクト
		L"Resources/Textures/Frame.png",		//ボックスの外枠
		L"Resources/Textures/ClearFrame1.png",	//クリア条件ボックスの外枠1
		L"Resources/Textures/CrearFrame2.png",	//クリア条件ボックスの外枠2
		L"Resources/Textures/GoalFrame1.png",	//ゴールボックスの外枠1
		L"Resources/Textures/GoalFrame2.png",	//ゴールボックスの外枠2
		L"Resources/Textures/Clear.png",		//クリア文字
		L"Resources/Textures/Confetti.png",		//紙吹雪
		L"Resources/Textures/ClearTime.png",	//ClearTime文字
	
	};

	//シェーダリソースビューとリソース
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	//テクスチャを読み込む
	ID3D11Device1* d3dDevice = DX::DeviceResources::GetInstance()->GetD3DDevice();
	int num = static_cast<int>(eTextureID::NUM_TEX);
	HRESULT hr = E_FAIL;
	for (int i = 0; i < num; i++)
	{
		//テクスチャを作成する
		hr = DirectX::CreateWICTextureFromFile(
			d3dDevice,
			fileList[i].c_str(),
			&resource,
			texture.ReleaseAndGetAddressOf()
		);
		assert(hr == S_OK);

		//テクスチャを登録する
		eTextureID state = static_cast<eTextureID>(i);
		m_textures[state] = texture;

		//テクスチャサイズを計算する
		this->CalcTextureSize(resource, state);
	}

	//判定する
	return true;
}

//============================================================================
// テクスチャのサイズを計算
//============================================================================
void ResourceManager::CalcTextureSize(
	const Microsoft::WRL::ComPtr<ID3D11Resource>& resource, 
	const eTextureID& id)
{
	ID3D11Texture2D* pTexture;
	D3D11_TEXTURE2D_DESC texDesc;

	pTexture = static_cast<ID3D11Texture2D*>(resource.Get());
	pTexture -> GetDesc(&texDesc);

	DirectX::SimpleMath::Vector2 texSize{
		static_cast<float>(texDesc.Width),
		static_cast<float>(texDesc.Height)
	};
	m_resources[id] = texSize;
}
