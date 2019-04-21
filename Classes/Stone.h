#ifndef __STONE_H__
#define __STONE_H__

#include "Common.h"
enum STONE_TYPE{ CHE, MA, XIANG, SHI, SHUAI, PAO, BING };

class Stone:public CCSprite
{
public:
	int _id;//ÿ�����Ӷ���һ��id
	bool _red;//��ɫ���Ǻ�ɫ
	STONE_TYPE _type;//����
	int _row;//�������е���
	int _col;//�������е���
	bool _dead;//������û��
	static CCPoint _ptOff;
	static float _myWidth;//���ӵĿ��
	//�������ӣ��ͳ�ʼ��
	static Stone* create(int id);
	bool init(int id);

	void move(int row, int col);
	void kill();
	STONE_TYPE getTypeByID(int id);
	int getRowByID(int);
	int getColByID(int);
	CCPoint getPos();
};

#endif