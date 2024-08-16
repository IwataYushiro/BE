#pragma once

// �E�B���h�E����
const int WIN_WIDTH = 900;
const int GAME_LINE = 600;

// �E�B���h�E�c��
const int WIN_HEIGHT = 600;

class Pattern
{
public:
	//�\����
	struct Obj
	{
		float		rad;
		float       posX;
		float       posY;
		float       speedX;
		float       speedY;
		bool        alive;
		float		count;//�G�p
	};

	//�e
	static const int BULLET_MAX_NUM = 20;//�e��

	struct Bullet
	{
		float		rad;
		float       posX;
		float       posY;
		float       speed;
		bool        isShot;
	};
	
public://�֐�
	//�G�̍U���p�^�[��
	void AttackPattern(Obj& obj);
	//���@�ړ�
	void PlayerMove(Obj& obj,char* key);
	//���@�e�ړ�
	void PlayerBulletMove(Bullet& bullet);
	//�G�ƒe�̓����蔻��
	void IsHitBulletToEnemy(Bullet& bullet, Obj& obj);

private:
	//�����蔻��p
	int x, y, xy;
	//����������
	bool isPlayerBulletHit = false;
	//���C�t
	int EnemyLife = 10;

public://�A�N�Z�b�T
	void SetEnemyLife(const int life) { EnemyLife = life; }
	const int GetEnemyLife() { return EnemyLife; }

private:
	
};

