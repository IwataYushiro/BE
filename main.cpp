#include "DxLib.h"
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include "ServerDatabase.h"
#include "Pattern.h"
#include <array>

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "AttackGame";

void Reset();

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine,
	_In_ int nCmdShow) {
	// �E�B���h�E���[�h�ɐݒ�
	ChangeWindowMode(TRUE);

	// �E�B���h�E�T�C�Y���蓮�ł͕ύX�������A
	// ���E�B���h�E�T�C�Y�ɍ��킹�Ċg��ł��Ȃ��悤�ɂ���
	SetWindowSizeChangeEnableFlag(FALSE, FALSE);

	// �^�C�g����ύX
	SetMainWindowText(TITLE);

	// ��ʃT�C�Y�̍ő�T�C�Y�A�J���[�r�b�g����ݒ�(���j�^�[�̉𑜓x�ɍ��킹��)
	SetGraphMode(WIN_WIDTH, WIN_HEIGHT, 32);

	// ��ʃT�C�Y��ݒ�(�𑜓x�Ƃ̔䗦�Őݒ�)
	SetWindowSizeExtendRate(1.0);

	// ��ʂ̔w�i�F��ݒ肷��
	SetBackgroundColor(0x1f, 0x1e, 0x33);

	// DXlib�̏�����
	if (DxLib_Init() == -1) { return -1; }

	// (�_�u���o�b�t�@)�`���O���t�B�b�N�̈�͗��ʂ��w��
	SetDrawScreen(DX_SCREEN_BACK);
	// �摜�Ȃǂ̃��\�[�X�f�[�^�̕ϐ��錾�Ɠǂݍ���
	int playerG;
	int playerbulletG;

	playerG = LoadGraph("player.png");
	playerbulletG = LoadGraph("playerbullet.png");

	// �Q�[�����[�v�Ŏg���ϐ��̐錾
	// �ϐ��錾
	//���@
	Pattern* pattern = new Pattern();

	Pattern::Obj player = { 16.0f,300.0f,600.0f,7.0f,7.0f,true,0.0f };
	Pattern::Bullet playerBullet[Pattern::BULLET_MAX_NUM] = {};
	for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
	{
		playerBullet[i] = { 8.0f,0.0f,0.0f,-10.0f,false };
	}

	Pattern::Obj enemy = { 50.0f,50.0f,50.0f,0.1f,0.1f,true,0.0f };
	pattern->SetEnemyLife(10);

	//�G�Ǝ��@�e�̓����蔻��

	//int startTime;		// �X�^�[�g�������L�����Ă���
	int resultCount;		//���ʃJ�E���g
	unsigned int color = GetColor(255, 255, 255);	// �J���[�f�[�^�̊i�[
	int space = 0;

	const int MAX_SCORE = 10000;
	int score = 0; // �X�R�A
	int progressTime = 0; // �X�^�[�g����̌o�ߎ���

	enum Scene
	{
		START,
		Time,
		PostRanking,
		Num
	};
	int scene = START;

	// �ŐV�̃L�[�{�[�h���p
	char keys[256] = { 0 };

	// 1���[�v(�t���[��)�O�̃L�[�{�[�h���
	char oldkeys[256] = { 0 };

	std::wstring url = L"https://attackgame.vercel.app/scores/";
	std::array<int, 10> ranking{};

	// �Q�[�����[�v
	while (true) {
		// �ŐV�̃L�[�{�[�h��񂾂������̂�1�t���[���O�̃L�[�{�[�h���Ƃ��ĕۑ�
		for (int i = 0; i < 256; ++i)
		{
			oldkeys[i] = keys[i];
		}
		// �ŐV�̃L�[�{�[�h�����擾
		GetHitKeyStateAll(keys);

		// ��ʃN���A
		ClearDrawScreen();
		//---------  ��������v���O�������L�q  ----------//

		// �X�V����
		//���̃��C�������UI�]�[���Ȃ̂ł����������Ƀ{�b�N�XUI�����Ă�������
		DrawBox(GAME_LINE, 0, WIN_WIDTH, WIN_HEIGHT, GetColor(0xff, 0xff, 0xff), TRUE);

		switch (scene)
		{
		case START:
			if (keys[KEY_INPUT_SPACE] && !oldkeys[KEY_INPUT_SPACE])space++;
			if (space % Num == Time)
			{
				//���Z�b�g
				player = { 16.0f,300.0f,600.0f,7.0f,7.0f,true,0.0f };
				for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
				{
					playerBullet[i] = { 8.0f,0.0f,0.0f,-10.0f,false };
				}
				enemy = { 50.0f,50.0f,50.0f,0.1f,0.1f,true,0.0f };
				pattern->SetEnemyLife(10);

				score = 0; // �X�R�A
				progressTime = 0; // �X�^�[�g����̌o�ߎ���
				
				scene = Time;
				break;
			}
			break;

		case Time:
			//�G����
			pattern->AttackPattern(enemy);
			//���@�ړ�
			pattern->PlayerMove(player, keys);
			//�e����
			if (keys[KEY_INPUT_SPACE] == true && oldkeys[KEY_INPUT_SPACE] == false)
			{
				for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
				{
					if (playerBullet[i].isShot == false)
					{
						playerBullet[i].isShot = true;
						playerBullet[i].posX = player.posX + player.rad;
						playerBullet[i].posY = player.posY;//�ey�̏�����
						break;
					}
				}
			}
			//�e�ړ�
			for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
			{
				pattern->PlayerBulletMove(playerBullet[i]);
			}
			//�����蔻��
			for (int i = 0; i < Pattern::BULLET_MAX_NUM; i++)
			{
				pattern->IsHitBulletToEnemy(playerBullet[i], enemy);
			}
			//�^�C��
			progressTime++;
			resultCount = MAX_SCORE - progressTime;
			//���
			if (resultCount < 0)
			{
				score = 0; // �o�[�X�g������X�R�A��0
				scene = PostRanking;
				break;
			}
			if (pattern->GetEnemyLife() <= 0)space++;
			//�|������W�v
			if (space % Num == PostRanking)
			{
				score = resultCount; // �����|���قǍ����_
				// �����L���O�ɋL�^����
				auto serverStatusCode = Post(url, score).wait();

				// �����L���O���擾����
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
		

		// �`�揈��
		
		switch (scene)
		{
		case START:
			DrawFormatString(250, 200, color, "ATTACKGAME");

			DrawFormatString(250, 250, color, "SPACE START ");
			break;

		case Time:
		//�I�u�W�F�N�g�֌W
		if (player.alive == true)
		{
			DrawGraph(player.posX, player.posY, playerG, TRUE);
		}

		if (enemy.alive == true)
		{
			DrawCircle(enemy.posX, enemy.posY, enemy.rad, GetColor(0, 0, 255), TRUE);
		}
		//���@�e
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
		
		//�ȉ��f�o�b�N�p
		DrawLine(GAME_LINE, 0, GAME_LINE, WIN_HEIGHT, GetColor(255, 0, 0));
		//����

		DrawString(650, 100, "�A���[�L�[�ő���@�@", GetColor(0, 0, 0));
		DrawString(650, 150, "�X�y�[�X�L�[�Œe����", GetColor(0, 0, 0));

		DrawFormatString(700, 660, GetColor(0, 0, 0), ("Enemy HP %d"), pattern->GetEnemyLife());

		//---------  �����܂łɃv���O�������L�q  ---------//
		// (�_�u���o�b�t�@)����
		ScreenFlip();

		// 20�~���b�ҋ@(�^��60FPS)
		WaitTimer(20);

		// Windows�V�X�e�����炭�������������
		if (ProcessMessage() == -1) {
			break;
		}

		// ESC�L�[�������ꂽ�烋�[�v���甲����
		if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) {
			break;
		}
	}
	//���Еt��
	delete pattern;
	DeleteGraph(playerG);
	DeleteGraph(playerbulletG);
	// Dx���C�u�����I������
	DxLib_End();

	// ����I��
	return 0;
}
