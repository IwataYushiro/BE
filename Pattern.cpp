#include "Pattern.h"
#include <math.h>
#include "DxLib.h"


void Pattern::AttackPattern(Obj& obj)
{
	if (obj.alive == true)
	{
		obj.posX = sin(obj.count * obj.speedX) * 200.0f + 300.0f;
		obj.posY = cos(obj.count * obj.speedY) * 100.0f + 200.0f;
		obj.count++;
	}
}

void Pattern::PlayerMove(Obj& obj, char* key)
{
	if (key[KEY_INPUT_UP] == true && key[KEY_INPUT_RIGHT] == true)
	{
		obj.posX += obj.speedX * 0.7f;
		obj.posY -= obj.speedY * 0.7f;
	}
	else if (key[KEY_INPUT_UP] == true && key[KEY_INPUT_LEFT] == true)
	{
		obj.posX -= obj.speedX * 0.7f;
		obj.posY -= obj.speedY * 0.7f;
	}
	else if (key[KEY_INPUT_DOWN] == true && key[KEY_INPUT_RIGHT] == true)
	{
		obj.posX += obj.speedX * 0.7f;
		obj.posY += obj.speedY * 0.7f;
	}
	else if (key[KEY_INPUT_DOWN] == true && key[KEY_INPUT_LEFT] == true)
	{
		obj.posX -= obj.speedX * 0.7f;
		obj.posY += obj.speedY * 0.7f;
	}
	else if (key[KEY_INPUT_RIGHT] == true)
	{
		obj.posX += obj.speedX;
	}
	else if (key[KEY_INPUT_LEFT] == true)
	{
		obj.posX -= obj.speedX;
	}
	else if (key[KEY_INPUT_UP] == true)
	{
		obj.posY -= obj.speedY;
	}
	else if (key[KEY_INPUT_DOWN] == true)
	{
		obj.posY += obj.speedY;
	}

	//’[‚É‚Í•Ç
	if (obj.posX > GAME_LINE - obj.rad * 2.0f)
	{
		obj.posX = GAME_LINE - obj.rad * 2.0f;
	}
	else if (obj.posX < 0.0f)
	{
		obj.posX = 0.0f;
	}
	else if (obj.posY < 0.0f)
	{
		obj.posY = 0.0f;
	}
	else if (obj.posY > WIN_HEIGHT - obj.rad * 2.0f)
	{
		obj.posY = WIN_HEIGHT - obj.rad * 2.0f;
	}
}

void Pattern::PlayerBulletMove(Bullet& bullet)
{
	if (bullet.isShot == true)
	{
		bullet.posY += bullet.speed;
		if (bullet.posY <= 0.0f)
		{
			bullet.isShot = false;
		}
	}
}

void Pattern::IsHitBulletToEnemy(Bullet& bullet, Obj& obj)
{
	if (obj.alive == true && bullet.isShot == true)
	{
		x = obj.posX - bullet.posX;
		y = obj.posY - bullet.posY;
		xy = x * x + y * y;

		if (xy <= (bullet.rad + obj.rad) * (bullet.rad + obj.rad))
		{
			isPlayerBulletHit = true;
		}
	}
	if (isPlayerBulletHit == true)
	{
		EnemyLife--;
		bullet.isShot = false;//false=’Êí,true=ŠÑ’Ê
		isPlayerBulletHit = false;

	}
	if (EnemyLife == 0)
	{
		obj.alive = false;
	}
}
