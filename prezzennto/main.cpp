#include "DxLib.h"
#define _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
#include "ServerDatabase.h"
#include <array>

// �E�B���h�E�̃^�C�g���ɕ\�����镶����
const char TITLE[] = "stopWatch";

// �E�B���h�E����
const int WIN_WIDTH = 600;

// �E�B���h�E�c��
const int WIN_HEIGHT = 400;

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
	SetBackgroundColor(0x00, 0x00, 0x00);

	// DXlib�̏�����
	if (DxLib_Init() == -1) { return -1; }

	// (�_�u���o�b�t�@)�`���O���t�B�b�N�̈�͗��ʂ��w��
	SetDrawScreen(DX_SCREEN_BACK);

	// �Q�[�����[�v�Ŏg���ϐ��̐錾
	// �ϐ��錾
	float startTime;		// �X�^�[�g�������L�����Ă���
	float resultCount;		//���ʃJ�E���g
	unsigned int color = GetColor(255, 255, 255);	// �J���[�f�[�^�̊i�[
	int space = 0;
	int score = 0; // �X�R�A
	float progressTime = 0; // �X�^�[�g����̌o�ߎ���

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

	std::wstring url = L"http://localhost:3000/scores/";


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
			//���
			if (progressTime > 1000.0f * 12.0f)
			{
				score = 0; // �o�[�X�g������X�R�A��0
				scene = PostRanking;
				break;
			}
			//�X�y�[�X�ŃX�g�b�v
			if (space % Num == PostRanking)
			{
				score = -abs(progressTime - 10000) + 10000; // 10�b�ɋ߂��قǍ����_
				// �����L���O�ɋL�^����
				auto serverStatusCode = Post(url, score).wait();
				scene = PostRanking;
				break;
			}

			DrawFormatString(250, 200, color, "now %.3f\n", progressTime / 1000.0f);
			DrawFormatString(250, 250, color, "SPACE STOP ");
			break;

		case PostRanking:


			// �����L���O���擾����

			if (space % Num == START)
			{
				scene = START;
				break;
			}

			DrawFormatString(250, 200, color, "result %.3f   score %d\n", resultCount, score);
			DrawFormatString(250, 250, color, "SPACE RESET ");
			break;
		}

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
	// Dx���C�u�����I������
	DxLib_End();

	// ����I��
	return 0;
}
