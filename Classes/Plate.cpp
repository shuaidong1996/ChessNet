#include "Plate.h"


bool Plate::init()
{
	CCSprite::initWithFile("background.png");

	setPosition(ccp(0, 0));
	//����ê��Ϊ��0��0��
	setAnchorPoint(ccp(0, 0));
	//����
	setScale(winSize.height / getContentSize().height);

	return true;
}
