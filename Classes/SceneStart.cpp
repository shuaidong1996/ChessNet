#include "SceneStart.h"
#include "SceneGame.h"

bool SceneStart::init()
{
	CCLayer::init();
	//����ͼƬ
	CCSprite* s = CCSprite::create("desk.png");
	addChild(s);
	s->setPosition(ccp(winSize.width / 2, winSize.height / 2));

	//������ʼ��ť
	CCSprite* sprite1 = CCSprite::create("bkg1.png");
	CCSprite* sprite2 = CCSprite::create("bkg2.png");
	addChild(sprite1);
	addChild(sprite2);
	sprite2->setPosition(ccp(winSize.width/4, winSize.height / 2));
	sprite1->setPosition(ccp(winSize.width - winSize.width/4, winSize.height / 2));
	
	_black = sprite1;
	_red = sprite2;

	//ʹ�ܴ�����ʹ�õ���
	setTouchEnabled(true);
	setTouchMode(kCCTouchesOneByOne);

	//����֡ѭ����ʱ��(��Ҫ���� ccTouchBegan ���棬��Ȼ��һ��Bug���ǣ�������Σ���������٣����һ��������ζ�ʱ��)
	scheduleUpdate();

	return true;
}

bool SceneStart::ccTouchBegan(CCTouch* touch, CCEvent*)
{
	//��ȡ����λ��
	CCPoint pt = touch->getLocation();
	//�ж��Ƿ�����  �ڰװ�ť
	if (_red->boundingBox().containsPoint(pt))
	{
		_bred = true;
	}
	else if (_black->boundingBox().containsPoint(pt))
	{
		_bred = false;
	}
	else return 0;

	//�������Ӻ��Ч��
	CCMoveTo* move1 = CCMoveTo::create(0.5, ccp(winSize.width / 2 + 10, winSize.height / 2));
	CCMoveTo* move2 = CCMoveTo::create(0.5, ccp(winSize.width / 2, winSize.height / 2));
	CCRotateBy* rotate1 = CCRotateBy::create(2, 360);
	CCRotateBy* rotate2 = CCRotateBy::create(2, -360);
	_red->runAction(CCSpawn::createWithTwoActions(move1,rotate1));
	_black->runAction(CCSpawn::createWithTwoActions(move2, rotate2));

	return true;
}

void SceneStart::update(float dt)
{
	//Ҫ���������ӽ�����ײ���  _red->getContentSize().width  ��ͼƬ�Ŀ��
	//if (_red->boundingBox().intersectsRect(_black->boundingBox()))//�жϾ����Ƿ�Ӵ�  intersectsRect
	if (_red->getPositionX() - _black->getPositionX() > 0 )
	{
		Game::_redStart = _bred;
		CCDirector::sharedDirector()->replaceScene(CreateScene(SceneGame::create()));
	}

}
