#include "DxLib.h"
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include "ServerDatabase.h"
#include "Pattern.h"
#include <array>

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "AttackGame";

void Reset();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {
	// ウィンドウモードに設定
	ChangeWindowMode(TRUE);

	// ウィンドウサイズを手動では変更させず、
	// かつウィンドウサイズに合わせて拡大できないようにする
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// タイトルを変更
	SetMainWindowText(TITLE);

	// 画面サイズの最大サイズ、カラービット数を設定(モニターの解像度に合わせる)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// 画面サイズを設定(解像度との比率で設定)
	SetWindowSizeExtendRate(1.0);

	// 画面の背景色を設定する
	SetBackgroundColor(0x1f, 0x1e, 0x33);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);
	// 画像などのリソースデータの変数宣言と読み込み
	int playerG;
	int playerbulletG;

	playerG = LoadGraph("player.png");
	playerbulletG = LoadGraph("playerbullet.png");

	// ゲームループで使う変数の宣言
	// 変数宣言
	//自機
	Pattern* pattern = new Pattern();

	Pattern::Obj player = { 16.0f,300.0f,600.0f,7.0f,7.0f,true,0.0f };
	Pattern::Bullet playerBullet[Pattern::BULLET_MAX_NUM] = {};
	for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
	{
		playerBullet[i] = { 8.0f,0.0f,0.0f,-10.0f,false };
	}

	Pattern::Obj enemy = { 50.0f,50.0f,50.0f,0.1f,0.1f,true,0.0f };
	pattern->SetEnemyLife(10);

	//敵と自機弾の当たり判定

	//int startTime;		// スタート時刻を記憶しておく
	int resultCount;		//結果カウント
	unsigned int color = GetColor(255, 255, 255);	// カラーデータの格納
	int space = 0;

	const int MAX_SCORE = 10000;
	int score = 0; // スコア
	int progressTime = 0; // スタートからの経過時間

	enum Scene
	{
		START,
		Time,
		PostRanking,
		Num
	};
	int scene = START;

	// 最新のキーボード情報用
	char keys[256] = { 0 };

	// 1ループ(フレーム)前のキーボード情報
	char oldkeys[256] = { 0 };

	std::wstring url = L"https://attackgame.vercel.app/scores/";
	std::array<int, 10> ranking{};

	// ゲームループ
	while (true) {
		// 最新のキーボード情報だったものは1フレーム前のキーボード情報として保存
		for (int i = 0; i < 256; ++i)
		{
			oldkeys[i] = keys[i];
		}
		// 最新のキーボード情報を取得
		GetHitKeyStateAll(keys);

		// 画面クリア
		ClearDrawScreen();
		//---------  ここからプログラムを記述  ----------//

		// 更新処理
		//このラインからはUIゾーンなのでいっそここにボックスUIおいてもいいや
		DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);

		switch (scene)
		{
		case START:
			if (keys[KEY_INPUT_SPACE] && !oldkeys[KEY_INPUT_SPACE])space++;
			if (space % Num == Time)
			{
				//リセット
				player = { 16.0f,300.0f,600.0f,7.0f,7.0f,true,0.0f };
				for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
				{
					playerBullet[i] = { 8.0f,0.0f,0.0f,-10.0f,false };
				}
				enemy = { 50.0f,50.0f,50.0f,0.1f,0.1f,true,0.0f };
				pattern->SetEnemyLife(10);

				score = 0; // スコア
				progressTime = 0; // スタートからの経過時間
				
				scene = Time;
				break;
			}
			break;

		case Time:
			//敵挙動
			pattern->AttackPattern(enemy);
			//自機移動
			pattern->PlayerMove(player, keys);
			//弾発射
			if (keys[KEY_INPUT_SPACE] == true && oldkeys[KEY_INPUT_SPACE] == false)
			{
				for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
				{
					if (playerBullet[i].isShot == false)
					{
						playerBullet[i].isShot = true;
						playerBullet[i].posX = player.posX + player.rad;
						playerBullet[i].posY = player.posY;//弾yの初期位
						break;
					}
				}
			}
			//弾移動
			for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
			{
				pattern->PlayerBulletMove(playerBullet[i]);
			}
			//当たり判定
			for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
			{
				pattern->IsHitBulletToEnemy(playerBullet[i], enemy);
			}
			//タイム
			progressTime++;
			resultCount = MAX_SCORE - progressTime;
			//上限
			if (resultCount < 0)
			{
				score = 0; // バーストしたらスコアは0
				scene = PostRanking;
				break;
			}
			if (pattern->GetEnemyLife() <= 0)space++;
			//倒したら集計
			if (space % Num == PostRanking)
			{
				score = resultCount; // 早く倒すほど高得点
				// ランキングに記録する
				auto serverStatusCode = Post(url, score).wait();

				// ランキングを取得する
				auto task = Get<json::value>(url);
				const json::value j = task.get();
				auto array = j.as_array();

				for (int i = 0; i < array.size(); i++)
				{
					ranking[i] = array[i].at(U("score")).as_integer();
				}

				scene = PostRanking;
				break;
			}
			DrawFormatString(650, 50, GetColor(0, 0, 0), "now %d", resultCount);
			break;

		case PostRanking:

			

			if (keys[KEY_INPUT_SPACE] && !oldkeys[KEY_INPUT_SPACE])space++;

			if (space % Num == START)
			{
				scene = START;
				break;
			}
			break;
		}
		

		// 描画処理
		
		switch (scene)
		{
		case START:
			DrawFormatString(250, 200, color, "ATTACKGAME");

			DrawFormatString(250, 250, color, "SPACE START ");
			break;

		case Time:
		//オブジェクト関係
		if (player.alive == true)
		{
			DrawGraph(player.posX, player.posY, playerG, TRUE);
		}

		if (enemy.alive == true)
		{
			DrawCircle(enemy.posX, enemy.posY, enemy.rad, GetColor(0, 0, 255), TRUE);
		}
		//自機弾
		for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
		{
			if (playerBullet[i].isShot == true)
			{
				DrawGraph(playerBullet[i].posX - playerBullet[i].rad,
					playerBullet[i].posY, playerbulletG, TRUE);
			}
		}
		
			break;

		case PostRanking:

			for (int i = 0; i < ranking.size(); i++)
			{
				DrawFormatString(100, 200 + i * 20, color, "%d: %05d", i + 1, ranking[i]);
			}
			DrawFormatString(250, 200, color, "result %d\n", score);
			DrawFormatString(250, 250, color, "SPACE RESET ");

			break;
		}
		
		//以下デバック用
		DrawLine(GAME_LINE, 0, GAME_LINE, WIN_HEIGHT, GetColor(255, 0, 0));
		//操作

		DrawString(650, 100, "アローキーで操作　　", GetColor(0, 0, 0));
		DrawString(650, 150, "スペースキーで弾発射", GetColor(0, 0, 0));

		DrawFormatString(700, 660, GetColor(0, 0, 0), ("Enemy HP %d"), pattern->GetEnemyLife());

		//---------  ここまでにプログラムを記述  ---------//
		// (ダブルバッファ)裏面
		ScreenFlip();

		// 20ミリ秒待機(疑似60FPS)
		WaitTimer(20);

		// Windowsシステムからくる情報を処理する
		if (ProcessMessage() == -1) {
			break;
		}

		// ESCキーが押されたらループから抜ける
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	//お片付け
	delete pattern;
	DeleteGraph(playerG);
	DeleteGraph(playerbulletG);
	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}
