#include "Stone.h"
#include "Game.h"
CCPoint Stone::_ptOff = ccp(45, 25);//ƫ��ֵ��ʼ��
float Stone::_myWidth;// = winSize.height / 10;//���ӿ�ȳ�ʼ��(ǧ��ע�⡣�������winSize������⡣���ò���winSize��ֻ���ں������������)

Stone* Stone::create(int id)
{
	Stone* stone = new Stone;
	stone->init(id);
	stone->autorelease();
	return stone;
}
bool Stone::init(int id)
{
	if (Game::_redStart)
	{
		_row = getRowByID(id);
		_col = getColByID(id);
	}
	else
	{
		_row = getRowByID(id);
		_col = getColByID(id);
		_row = 9 - _row;
		_col = 8 - _col;
	}
	_id = id;
	_type = getTypeByID(id);
	_red = id < 16;
	_dead = false;

	const char* bfile[] = { "bche.png", "bma.png", "bxiang.png", "bshi.png", "bjiang.png", "bpao.png", "bzu.png" };
	const char* rfile[] = { "rche.png", "rma.png", "rxiang.png", "rshi.png", "rshuai.png", "rpao.png", "rbing.png" };

	if (_red)
		CCSprite::initWithFile(rfile[_type]);
	else
		CCSprite::initWithFile(bfile[_type]);
	//���������Լ���λ��
	setPosition(getPos());
	//���������е���ˣ���Сһ��
	setScale(.9f);

	return true;
}
//��ȡ���͡�����ֻ��6��{ CHE, MA, XIANG, SHI, SHUAI, PAO, BING };Ҳ����˵����ֻ�� 0 -- 5
STONE_TYPE Stone::getTypeByID(int id)
{
	//ÿһ��ID ��Ӧһ��λ�ã���Ӧһ��type
	STONE_TYPE type[32] = { CHE, MA, XIANG, SHI, SHUAI, SHI, XIANG, MA, CHE,
		PAO, PAO,
		BING, BING, BING, BING, BING,
		BING, BING, BING, BING, BING,
		PAO, PAO,
		CHE, MA, XIANG, SHI, SHUAI, SHI, XIANG, MA, CHE };
	return type[id];
}
//�ж�ÿ�����Ӵ�����һ��
int Stone::getRowByID(int id)
{
	int row[32] = { 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 2,
		3, 3, 3, 3, 3,
		6, 6, 6, 6, 6,
		7, 7,
		9, 9, 9, 9, 9, 9, 9, 9, 9 };
	return row[id];
}
//�ж�ÿ�����Ӵ�����һ��
int Stone::getColByID(int id)
{
	int col[32] = { 0, 1, 2, 3, 4, 5, 6, 7, 8,
		1, 7,
		0, 2, 4, 6, 8,
		0, 2, 4, 6, 8,
		1, 7,
		0, 1, 2, 3, 4, 5, 6, 7, 8 };
	return col[id];
}
//��ȡ�������ӵ�λ�ã��������꣩
CCPoint Stone::getPos()
{
	float x = _col*_myWidth;
	float y = _row*_myWidth;
	return ccp(x, y) + _ptOff;//��ʱ��֪��Ϊʲô��ֱ�Ӹ�һ���ض���ֵ���ǵö��徲̬��Ա������Ȼ����Game����ֵ������Ҫ�Ǻ���û�õ���ɾ����̬
}
void Stone::move(int row, int col)
{
	_row = row;
	_col = col;
	//this->setPosition(getPos());
	CCPoint pt = getPos();
	this->setPosition(pt);
}