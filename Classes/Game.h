#ifndef __GAME_H__
#define __GAME_H__

#include "Common.h"
#include "Stone.h"
#include "Plate.h"
#include "Step.h"

class Game:public CCLayer
{
	int _selectid;//��¼ѡ�����ӵ�id
	bool _isOver;//��Ϸ�Ƿ����
	bool regretFlag;//�Ƿ���ȷ���»���
	CCSprite* _selectFlag;//ѡ��ʱ�ľ���ͼƬ
	CCMenu* menu;
	CCMenuItem* regretItem;
	CCMenuItem* regretTextItem;
	CCMenuItem* yesItem;
	CCMenuItem* noItem;
	CCMenuItem* giveMyItem;
	CCMenuItem* giveMyItemYes;
	CCMenuItem* giveMyItemNo;
public:
	static bool _redStart;//���忪ʼ
	static bool _redTurn;//�ò��ú��� ���ֵ�˭���ˣ�
	Plate* _plate;//����
	Stone* _stone[32];//����
	CCArray* _steps;//��¼�������Ϣ
	

	CREATE_FUNC(Game);
	bool init();

	virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
	virtual void ccTouchEnded(CCTouch *pTouch, CCEvent *pEvent);
	//����㵽�յ��ϣ�����-1
	int getClickStone(CCTouch*);
	CCPoint getPosByRowCol(int, int);
	bool getRowColByPos(int &row, int &col, CCPoint pt);
	int getStoneIdByRowCol(int row, int col);
	bool canMove(int moveid, int row, int col, int killed);
	bool canMoveChe(int moveid, int row, int col, int killed);
	bool canMoveMa(int moveid, int row, int col, int killed);
	bool canMoveXiang(int moveid, int row, int col, int killed);
	bool canMoveShi(int moveid, int row, int col, int killed);
	bool canMoveShuai(int moveid, int row, int col, int killed);
	bool canMovePao(int moveid, int row, int col, int killed);
	bool canMoveBing(int moveid, int row, int col, int killed);

	int getStoneCountInLine(int row1, int col1, int row2, int col2);
	void onEnter();
	void onExit();
	void regret();
	void checkGameOver(int killid);
	void gameOverCallback(CCObject*);
	void closeGame(CCObject*);
	void preregret(CCObject*);
	void yesregret(CCObject*);
	void noregret(CCObject*);
	void waitYes(CCObject*);
	void waitNo(CCObject*);
	void initItem();


	void update(float);
	void doSelectStone(int id);
	void doMoveStone(int id, int row, int col);
	void selectStone(int id);
	void moveStone(int id, CCPoint pos);
	void sendSelectStoneToNet();
	void sendMoveStoneToNet(int row,int col);
	void initRegretButton();
	void moveBy(CCPoint pt, CCNode* node);
	void judgeRegret();
	void waitRegretYes();
	void waitRegretNo();
};

#endif