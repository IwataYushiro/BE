#include "DxLib.h"
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include "ServerDatabase.h"
#include <array>

// ウィンドウのタイトルに表示する文字列
const char TITLE[] = "stopWatch";

// ウィンドウ横幅
const int WIN_WIDTH = 600;

// ウィンドウ縦幅
const int WIN_HEIGHT = 400;

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
	SetBackgroundColor(0x00, 0x00, 0x00);

	// DXlibの初期化
	if (DxLib_Init() == -1) { return -1; }

	// (ダブルバッファ)描画先グラフィック領域は裏面を指定
	SetDrawScreen(DX_SCREEN_BACK);

	// ゲームループで使う変数の宣言
	// 変数宣言
	float startTime;		// スタート時刻を記憶しておく
	float resultCount;		//結果カウント
	unsigned int color = GetColor(255, 255, 255);	// カラーデータの格納
	int space = 0;
	int score = 0; // スコア
	float progressTime = 0; // スタートからの経過時間

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

	std::wstring url = L"http://localhost:3000/scores/";


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
		if (keys[KEY_INPUT_SPACE] && !oldkeys[KEY_INPUT_SPACE])
		{
			space++;
		}
		switch (scene)
		{
		case START:
			if (space % Num == Time)
			{
				startTime = GetNowCount();
				scene = Time;
				break;
			}

			DrawFormatString(250, 200, color, "0.000\n");
			DrawFormatString(250, 250, color, "SPACE START ");
			break;

		case Time:
			progressTime = GetNowCount() - startTime;
			resultCount = progressTime / 1000.0f;
			//上限
			if (progressTime > 1000.0f * 12.0f)
			{
				score = 0; // バーストしたらスコアは0
				scene = PostRanking;
				break;
			}
			//スペースでストップ
			if (space % Num == PostRanking)
			{
				score = -abs(progressTime - 10000) + 10000; // 10秒に近いほど高得点
				// ランキングに記録する
				auto serverStatusCode = Post(url, score).wait();
				scene = PostRanking;
				break;
			}

			DrawFormatString(250, 200, color, "now %.3f\n", progressTime / 1000.0f);
			DrawFormatString(250, 250, color, "SPACE STOP ");
			break;

		case PostRanking:


			// ランキングを取得する

			if (space % Num == START)
			{
				scene = START;
				break;
			}

			DrawFormatString(250, 200, color, "result %.3f   score %d\n", resultCount, score);
			DrawFormatString(250, 250, color, "SPACE RESET ");
			break;
		}

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
	// Dxライブラリ終了処理
	DxLib_End();

	// 正常終了
	return 0;
}
