/*
シーンインターフェース
*/
#pragma once

#include "StepTimer.h"
#include "GameMain.h"


class IScene
{
public:
	//デストラクタ
	virtual ~IScene() = default;
	//初期化する
	virtual void Initialize() = 0;
	//更新する
	virtual GAME_SCENE Update(const DX::StepTimer& timer) = 0;
	//描画する
	virtual void Draw() = 0;
	//終了処理する
	virtual void Finalize() = 0;
};