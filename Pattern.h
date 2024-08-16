#pragma once

// ウィンドウ横幅
const int WIN_WIDTH = 900;
const int GAME_LINE = 600;

// ウィンドウ縦幅
const int WIN_HEIGHT = 600;

class Pattern
{
public:
	//構造体
	struct Obj
	{
		float		rad;
		float       posX;
		float       posY;
		float       speedX;
		float       speedY;
		bool        alive;
		float		count;//敵用
	};

	//弾
	static const int BULLET_MAX_NUM = 20;//弾数

	struct Bullet
	{
		float		rad;
		float       posX;
		float       posY;
		float       speed;
		bool        isShot;
	};
	
public://関数
	//敵の攻撃パターン
	void AttackPattern(Obj& obj);
	//自機移動
	void PlayerMove(Obj& obj,char* key);
	//自機弾移動
	void PlayerBulletMove(Bullet& bullet);
	//敵と弾の当たり判定
	void IsHitBulletToEnemy(Bullet& bullet, Obj& obj);

private:
	//当たり判定用
	int x, y, xy;
	//当たったか
	bool isPlayerBulletHit = false;
	//ライフ
	int EnemyLife = 10;

public://アクセッサ
	void SetEnemyLife(const int life) { EnemyLife = life; }
	const int GetEnemyLife() { return EnemyLife; }

private:
	
};

