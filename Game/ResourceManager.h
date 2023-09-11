/*
	ResourceManager.h
	制作日：2023.04.18
	制作者：加藤穂乃佳
*/

#pragma once
#include <unordered_map>

//テクスチャID
enum class eTextureID
{
	NEXT_SCENE_KEY = 0,	//シーン切り替えキー
	NEXT_SCENE_CLICK,	//シーン切り替えクリック
	FADE,				//フェード
	EXPLANATION,		//説明書
	BACK,				//背景
	TITLE_LOGO,			//タイトルロゴ
	NUMBER,				//数字
	MOVE_COUNT,			//MoveCount文字
	OPERATION,			//操作UI
	TAB_OPEN,			//TAB操作
	TAB_CLOSE,			//TAB操作終了
	EFFECT_STAR,		//星エフェクト
	BOX_FRAME,			//ボックスの外枠
	CLEARBOX_FRAME_1,	//クリア条件ボックスの外枠1
	CLEARBOX_FRAME_2,	//クリア条件ボックスの外枠2
	GOALBOX_FRAME_1,	//ゴールボックスの外枠1
	GOALBOX_FRAME_2,	//ゴールボックスの外枠2
	CLEAR,				//クリア文字
	CONFETTI,			//紙吹雪
	CLEAR_TIME,			//ClearTime文字


	NUM_TEX,			//テクスチャ合計数
};

class ResourceManager
{
public:
	//インスタンスの取得
	static ResourceManager& GetRefInstance()
	{
		static ResourceManager instance;
		return instance;
	}

private:
	std::unordered_map<eTextureID,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_textures;

	std::unordered_map<eTextureID, DirectX::SimpleMath::Vector2> m_resources;

private:
	//コンストラクタ
	ResourceManager()
		: m_textures{}
	{
		this->CreateTextures();
	};

	//デストラクタ
	~ResourceManager() = default;

	//コピーコンストラクタ、コピー代入演算子の削除
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator = (const ResourceManager&) = delete;
	
	//ムーブコンストラクタ、ムーブ代入演算子の削除
	ResourceManager(const ResourceManager&&) = delete;
	ResourceManager&& operator = (const ResourceManager&&) = delete;

public:
	//テクスチャの取得
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTexture(const eTextureID& id)
	{
		return m_textures[id];
	}
	//テクスチャサイズの取得
	const DirectX::SimpleMath::Vector2& GetTextureSize(const eTextureID& id)
	{
		return m_resources[id];
	}

private:
	//テクスチャの生成
	bool CreateTextures();

	//テクスチャのサイズを計算
	void CalcTextureSize(
		const Microsoft::WRL::ComPtr<ID3D11Resource>& resource,
		const eTextureID& id
	);
};
