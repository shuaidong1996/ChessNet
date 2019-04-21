#include "Game.h"
#include "SceneGame.h"
#include "Net.h"
#include "backMusic.h"
#include "SimpleAudioEngine.h"

bool Game::_redStart;//����   .h�ļ���������
bool Game::_redTurn;
void Game::onEnter()
{
	CCLayer::onEnter();
	_steps = CCArray::create();//û�м��뵽�κ��ӽڵ��У���һ֡�Զ����
	_steps->retain();//���Ա���
	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("backMusic.mp3", true);
}
void Game::onExit()
{
	CCLayer::onExit();
	_steps->release();//retain ֮��һ��Ҫ release 
	CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

bool Game::init()
{
	CCLayer::init();

	//����Լ���Ƭ��������
	CCSprite* sprite = CCSprite::create("3.jpg");
	sprite->setPosition(ccp(630, 200));
	sprite->setScale(0.4);
	addChild(sprite);

	//�������
	backMusic* music = backMusic::create();
	addChild(music);

	//�����̷ŵ�Game��
	_plate = Plate::create();
	addChild(_plate);
	//�ں�����
	Stone::_myWidth = winSize.height / 10;
	for (size_t i = 0; i < 32; i++)
	{
		_stone[i] = Stone::create(i);
		addChild(_stone[i]);
	}

	//��������
	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	//��ʼ����ť
	initItem();

	//���ùرհ�ť
	CCMenuItem* closeItem = CCMenuItemImage::create("CloseNormal.png", "CloseSelected.png", this, menu_selector(Game::closeGame));
	closeItem->setPosition(ccp(370, -200));
	menu->addChild(closeItem);

	//��ʼ��ѡ��ʱ�ľ���
	_selectFlag = CCSprite::create("selected.png");
	addChild(_selectFlag);
	_selectFlag->setVisible(false);

	//һ��ʼû���κ����ӱ�ѡ��
	_selectid = -1;

	//�����־Ϊfalse
	bool regretFlag = false;

	_redTurn = true;

	//������ʱ��
	scheduleUpdate();

	return true;
}

bool Game::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
	return true;
}
void Game::ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent)
{
	int clickid = getClickStone(pTouch);//���ݴ������ȡ���ӣ���������û�����ӷ��� -1
	if (_selectid == -1)//�����û�����ӱ�ѡ��
		selectStone(clickid);
	else//����
		moveStone(clickid,pTouch->getLocation());
}
//ѡ������
void Game::selectStone(int clickid)
{
	if (clickid != -1 && _redTurn == _redStart && _redTurn == _stone[clickid]->_red)//���1.���������  2.�ֵ����ҷ���  3.���ҷ�������
	{
		doSelectStone(clickid);

		sendSelectStoneToNet();
	}
}
//�ƶ�����
void Game::moveStone(int clickid,CCPoint pos)
{
	if (clickid != -1)//�����жϵ��е�λ���ǲ�������
	{
		if (_stone[clickid]->_red == _stone[_selectid]->_red)//����Ǽ����������ٴ�ѡ��
		{
			doSelectStone(clickid);
			
			sendSelectStoneToNet();
			return;
		}
	}
	//�����߶�
	int row, col;
	if (!getRowColByPos(row, col,pos))//���ܷ�������
		return;
	if (!canMove(_selectid, row, col, clickid))//ÿ���������Լ��̶����߷������뾭����˲�����
		return;

	//��Է�������Ϣ
	sendMoveStoneToNet(row, col);

	//����								ע����������selectid  һ���Ƿ��͹�����_selectid һ�����Լ�������� _selectid
	doMoveStone(_selectid, row, col);//�������ܣ�1.�Է�����Ϣ�����壨��ҪID��Ŀ��λ�ã�2.�Լ������壨��Ҫѡ�е����Ŀ��λ�ã�
}
//����ѡ����Ϣ
void Game::sendSelectStoneToNet()
{
	char buf[2];
	buf[0] = 1;//������1  ��ʾѡ������
	buf[1] = _selectid;
	Net::getInstance()->Send(buf, 2);
}
//����������Ϣ
void Game::sendMoveStoneToNet(int row, int col)
{
	char buf[4];
	buf[0] = 2;
	buf[1] = _selectid;
	buf[2] = 9 - row;
	buf[3] = 8 - col;
	Net::getInstance()->Send(buf, 4);
}

//���ڽ�������
void Game::update(float)
{
	if (_redStart == _redTurn && !regretFlag)//��ʱ��������Ϣ
		return;

	char buf;
	char id;
	char row;
	char col;
	if (Net::getInstance()->Recv(&buf, 1) != 1)
		return;
	
	switch (buf)
	{
	case 1://���նԷ�ѡ�����ӵ�����
		Net::getInstance()->Recv(&id, 1);
		doSelectStone(id);//�ѶԷ�ѡ��������ʾ����
		break;
	case 2:
		Net::getInstance()->Recv(&id, 1);
		Net::getInstance()->Recv(&row, 1);
		Net::getInstance()->Recv(&col, 1);
		doMoveStone(id, row, col);
		break;
	case 3:
		judgeRegret();
		break;
	case 'y': 
		waitRegretYes();
		break;
	case 'n':
		waitRegretNo();
		break;
	case 9:
		CCDirector::sharedDirector()->replaceScene(CreateScene(Game::create()));
	}
}
void Game::initItem()
{
	menu = CCMenu::create();
	addChild(menu);

	//�������尴ť
	initRegretButton();

	//������ť
	regretTextItem = CCMenuItemImage::create("huiqikuang.png", "huiqikuang2.png");
	yesItem = CCMenuItemImage::create("tongyi.png", "tongyi2.png");
	noItem = CCMenuItemImage::create("butongyi.png", "butongyi2.png");
	giveMyItem = CCMenuItemImage::create("shenqing2.png", "huiqikuang2.png");
	giveMyItemYes = CCMenuItemImage::create("duifangtongyi2.png", "duifangtongyi.png");
	giveMyItemNo = CCMenuItemImage::create("duifangbutongyi2.png", "duifangbutongyi.png");
	menu->addChild(regretTextItem);
	menu->addChild(yesItem);
	menu->addChild(noItem);
	menu->addChild(giveMyItem);
	menu->addChild(giveMyItemYes);
	menu->addChild(giveMyItemNo);

	regretTextItem->setVisible(false);
	yesItem->setVisible(false);
	noItem->setVisible(false);
	giveMyItem->setVisible(false);
	giveMyItemYes->setVisible(false);
	giveMyItemNo->setVisible(false);

	moveBy(ccp(-180, 120), regretTextItem);//����λ��
	moveBy(ccp(-280, -100), yesItem);//����λ��
	moveBy(ccp(-80, -100), noItem);//����λ��
	moveBy(ccp(-180, 120), giveMyItem);//����λ��
	moveBy(ccp(-180, 120), giveMyItemYes);//����λ��
	moveBy(ccp(-180, 120), giveMyItemNo);//����λ��

	yesItem->setTarget(this, menu_selector(Game::yesregret));
	noItem->setTarget(this, menu_selector(Game::noregret));
	giveMyItemYes->setTarget(this, menu_selector(Game::waitYes));
	giveMyItemNo->setTarget(this, menu_selector(Game::waitNo));
}
void Game::judgeRegret()
{
	regretTextItem->setVisible(true);
	yesItem->setVisible(true);
	noItem->setVisible(true);
}

//��ʾ�� ѡ�������
void Game::doSelectStone(int clickid)
{
	//��������ӱ����У������á��� ͼ�����Ǹ�λ����ʾ
	_selectFlag->setPosition(_stone[clickid]->getPosition());
	_selectFlag->setVisible(true);
	_selectid = clickid;
}
//���ڰ�һ��id�ӵ�ǰλ�ã��ƶ��� ָ��λ��
void Game::doMoveStone(int selectid, int row, int col)
{
	int killid = getStoneIdByRowCol(row, col);
	//��¼������Ϣ
	Step* step = Step::create(selectid, _stone[selectid]->_row, _stone[selectid]->_col, row, col, killid);
	_steps->addObject(step);//���뵽������

	//����
	_stone[_selectid]->move(row, col);
	_selectFlag->setVisible(false);

	_selectid = -1;//�����һ��Ҫ���ã���Ȼ��һֱѡ��

	if (killid != -1)
	{
		_stone[killid]->_dead = true;
		_stone[killid]->setVisible(false);

		//�����Ϸ�Ƿ����
		checkGameOver(killid);
	}
	_redTurn = !_redTurn;
}
void Game::checkGameOver(int killid)
{
	Stone* s = _stone[killid];
	if (s->_type == SHUAI)//��Ϸ���� ������ɱ������˧�򽫣���ô�ʹ�ӡ
	{
		CCMenu* menu = CCMenu::create();
		addChild(menu);
		if (s->_red == Game::_redStart)//��ʼѡ�����ɱ������һ��������ô������
		{
			CCMenuItem* item1 = CCMenuItemImage::create("shuijiemian.png", "shuijiemian.png");
			menu->addChild(item1);
			item1->setPosition(ccp(-150, 50));
		}
		else//Ӯ��
		{
			CCMenuItem* item1 = CCMenuItemImage::create("yingjiemian.png", "yingjiemian.png");
			menu->addChild(item1);
			item1->setPosition(ccp(-150, 50));

			CCMenuItem* item = CCMenuItemImage::create("new.jpg", "new2.jpg");
			menu->addChild(item);
			item->setTarget(this, menu_selector(Game::gameOverCallback));
			item->setPosition(ccp(-150, -150));
		}
		regretItem->setEnabled(false);

		// �������λ�öԵ�
		Game::_redStart = !Game::_redStart;
	}
}
void  Game::gameOverCallback(CCObject*)
{
	char buf = 9;
	Net::getInstance()->Send(&buf, 1);
	CCDirector::sharedDirector()->replaceScene(CreateScene(Game::create()));
}
//��ȡ ���� �����ӵ�id
int Game::getClickStone(CCTouch* touch)
{
	int row, col;
	if (!getRowColByPos(row, col, touch->getLocation()))
		return -1;
	return getStoneIdByRowCol(row, col);
}
//ͨ������ת��������
bool Game::getRowColByPos(int &row, int &col, CCPoint pt)
{
	for (row = 0; row < 10; row++)
		for (col = 0; col < 9; col++)
		{
			//��ȡ�����ϵ����������
			CCPoint ptStone = getPosByRowCol(row, col);
			//�ж����������ϵĵ㡣���Ƿ�ʹ�����ľ���С��  ���ӿ�ȵ�һ��
			if (ptStone.getDistance(pt) < Stone::_myWidth / 2)
				return true;
		}
	return false;
}
//ͨ�����С����Ƿ����ҵ�ĳ������
int Game::getStoneIdByRowCol(int row, int col)
{
	for (int i = 0; i < 32; i++)
	{
		if (_stone[i]->_row == row && _stone[i]->_col == col && !_stone[i]->_dead)
			return i;
	}
	return -1;
}

//���� ת���� ��������
CCPoint Game::getPosByRowCol(int row, int col)
{
	float x = col*Stone::_myWidth;
	float y = row*Stone::_myWidth;
	return ccp(x, y) + Stone::_ptOff;
}

bool  Game::canMove(int moveid, int row, int col, int killid)
{
	switch (_stone[moveid]->_type)
	{
	case CHE:
		return canMoveChe(moveid, row, col, killid);
	case MA:
		return canMoveMa(moveid, row, col, killid);
	case XIANG:
		return canMoveXiang(moveid, row, col, killid);
	case SHI:
		return canMoveShi(moveid, row, col, killid);
	case SHUAI:
		return canMoveShuai(moveid, row, col, killid);
	case PAO:
		return canMovePao(moveid, row, col, killid);
	case BING:
		return canMoveBing(moveid, row, col, killid);
	default:
		break;
	}
	return false;
}
int Game::getStoneCountInLine(int row1, int col1, int row2, int col2)
{
	if (row1 != row2 &&col1 != col2)
		return -1;
	int t = 0;
	if (row1 == row2)
	{
		int min = col1 > col2 ? col2 : col1;
		int max = col1 + col2 - min;
		for (int col = min + 1; col < max; col++)
		{
			int id = getStoneIdByRowCol(row1, col);
			if (id != -1)
				t++;
		}
	}
	else
	{
		int min = row1 > row2 ? row2 : row1;
		int max = row1 + row2 - min;
		for (int row = min + 1; row < max; row++)
		{
			int id = getStoneIdByRowCol(row,col1);
			if (id != -1)//���������
				t++;
		}
	}
	return t;
}

bool Game::canMoveChe(int moveid, int row, int col, int killid)//moveid �� �ϴ�ѡ�е����� killid����ε��������
{
	if (getStoneCountInLine(row, col, _stone[moveid]->_row, _stone[moveid]->_col) == 0)
		return true;
	return false;
}
bool Game::canMoveMa(int moveid, int row, int col, int killid)
{
	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d != 12 && d != 21)
		return false;
	//�ж�����
	if (d == 21)//�����1�������2
	{
		int mr = (r + row) / 2;
		int mc = c;
		if (getStoneIdByRowCol(mr, mc)!=-1)//˵�����λ��������
			return false;
	}
	else
	{
		int mr = r;
		int mc = (c + col) / 2;
		if (getStoneIdByRowCol(mr, mc) != -1)//˵�����λ��������
			return false;
	}
	return true;
}
bool Game::canMoveXiang(int moveid, int row, int col, int killid)
{
	if (_stone[moveid]->_red == _redStart)//ֻҪ�������ӺͿ���ѡ������һ������ô�Ϳ������·���
	{
		if (row > 4)
		return false;
	}
	else
	{
		if (row < 5)
			return false;
	}
	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d != 22)
		return false;

	int mr = (row + r) / 2;
	int mc = (col + c) / 2;
	int id = getStoneIdByRowCol(mr, mc);
	if (id != -1)
		return false;
	return true;
}
bool Game::canMoveShi(int moveid, int row, int col, int killid)
{
	if (_stone[moveid]->_red == _redStart)//ֻҪ�������ӺͿ���ѡ������һ������ô�Ϳ������·���
	{
		if (row > 2)
		return false;
	}
	else
	{
		if (row < 7)
			return false;
	}
	if (col < 3 || col>5)
		return false;

	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d == 11)//����ȣ���ô��ֻ�ܲ�ֵΪ1
		return true;
	return false;
}
bool Game::canMoveShuai(int moveid, int row, int col, int killid)
{
	//�Ͻ�����
	if (killid != -1 && _stone[killid]->_type == SHUAI)
		return canMoveChe(moveid, row, col, killid);

	//����ѡ������һ�����·�
	if (_stone[moveid]->_red == _redStart)//ֻҪ�������ӺͿ���ѡ������һ������ô�Ϳ������·���
	{
		if (row > 2)
			return false;
	}
	else//����������ӣ��Ϳ���ѡ�����Ӳ�һ������ô�Ͷ����������
	{
		if (row < 7)
			return false;
	}
	if (col < 3 || col>5)
		return false;

	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d == 1 || d == 10)//����ȣ���ô��ֻ�ܲ�ֵΪ1
		return true;
	return false;
}
bool Game::canMovePao(int moveid, int row, int col, int killid)
{
	if (killid == -1)//˵��û�е�������
		return canMoveChe(moveid, row, col, killid);//�ͳ�һ����ʹ��
	//�������Ӻ��м�����һ��������
	int count = getStoneCountInLine(row, col, _stone[moveid]->_row, _stone[moveid]->_col);
	return  count == 1;//���м�ֻ�ܸ�һ��
}
bool Game::canMoveBing(int moveid, int row, int col, int killid)
{
	int r = _stone[moveid]->_row;
	int c = _stone[moveid]->_col;
	int d = abs(r - row) * 10 + abs(c - col);
	if (d != 1 && d != 10)//����ȣ���ô��ֻ�ܲ�ֵΪ1
		return false;
	
	if (_stone[moveid]->_red == _redStart)//ֻҪ�������ӺͿ���ѡ������һ������ô�Ϳ������·���
	{
		//���ܺ���
		if (row < r)//��Ҫ�ߵ��� ���� ���ڵ���
			return false;
		//û���Ӳ���ƽ��
		if (r <= 4 && col != c)
			return false;
	}
	else
	{
		//���ܺ���
		if (row > r)//��Ҫ�ߵ��� ���� ���ڵ���
			return false;
		//û���Ӳ���ƽ��
		if (r >= 5  && col != c)
			return false;
	}
	return true;
}

void Game::yesregret(CCObject*)
{
	char buf = 'y';
	Net::getInstance()->Send(&buf, 1);
	regret();
	yesItem->setVisible(false);
	noItem->setVisible(false);
	regretTextItem->setVisible(false);
}
void Game::noregret(CCObject*)
{
	char buf = 'n';
	Net::getInstance()->Send(&buf, 1);
	yesItem->setVisible(false);
	noItem->setVisible(false);
	regretTextItem->setVisible(false);
}
void Game::waitRegretYes()
{
	giveMyItem->setVisible(false);

	giveMyItemYes->setVisible(true);
	regret();
}
void Game::waitRegretNo()
{
	giveMyItem->setVisible(false);

	giveMyItemNo->setVisible(true);
}
void Game::waitYes(CCObject*)
{
	giveMyItemYes->setVisible(false);
	//�򿪴���
	setTouchEnabled(true);
	//��������ʧ
	regretFlag = false;
}
void Game::waitNo(CCObject*)
{
	giveMyItemNo->setVisible(false);
	//�򿪴���
	setTouchEnabled(true);
	//��������ʧ
	regretFlag = false;
}
//�ڻ���֮ǰ�����жϣ��Է���ʱ���Լ����ܻ���
void Game::preregret(CCObject*)
{
	if (Game::_redStart != Game::_redTurn)
		return;

	//����Ϣ���Է�
	char buf = 3;
	Net::getInstance()->Send(&buf, 1);
	giveMyItem->setVisible(true);

	//�����Լ��Ĵ���
	setTouchEnabled(false);

	//���Խ�����Ϣ
	regretFlag = true;
 }
void Game::regret()
{
	for (size_t i = 0; i < 2; i++)
	{
		if (_steps->count() == 0)
			return;
		//ȡ����һ�ε����� ��λ��
		CCObject* obj = _steps->lastObject();
		Step* step = (Step*)obj;

		//��������ӣ��ָ���ԭ��λ��
		Stone* sMove = _stone[step->_moveid];//��ȡ��һ�����ӵ�id
		sMove->move(step->_rowFrom, step->_colFrom);

		//��������������ӣ�Ӧ�ø���
		if (step->_killid != -1)
		{
			Stone* sKill = _stone[step->_killid];
			sKill->_dead = false;//����
			sKill->setVisible(true);//�����ֳ���
		}

		//��ʼ��һЩ���ƶ���صı���
		_selectid = -1;
		_selectFlag->setVisible(false);
		_redTurn = !_redTurn;

		//��Array�������һ������ɾ��
		_steps->removeLastObject();
	}
}

//�������尴ť
void Game::initRegretButton()
{
	regretItem = CCMenuItemImage::create("regret.png", "regret2.png");
	menu->addChild(regretItem);
	regretItem->setTarget(this, menu_selector(Game::preregret));//���ô�������
	moveBy(ccp(120, 0), regretItem);//����λ��
}

void Game::moveBy(CCPoint pt, CCNode* node)
{
	node->setPosition(pt + node->getPosition());
}

void Game::closeGame(CCObject*)
{
	if (Net::getInstance()->isServer())
#ifdef WIN32
		closesocket(Net::getInstance()->getServerSocket());
#else
		close(Net::getInstance()->getServerSocket());
#endif
	else
#ifdef WIN32
		closesocket(Net::getInstance()->getClientSocket());
#else
		close(Net::getInstance()->getClientSocket());
#endif
	exit(0);
}