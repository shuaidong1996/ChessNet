#ifndef __SCENESTART_H__
#define __SCENESTART_H__

#include "Common.h"
 
class SceneStart :public CCLayer
{
private:
	CCSprite* _red;//�쾫��
	CCSprite* _black;//�ھ���
	bool _bred;//��һ���ߣ���Ϊtrue
public:
	CREATE_FUNC(SceneStart);
	bool init();
	bool ccTouchBegan(CCTouch*, CCEvent*);
	void update(float);//֡ѭ��
};

#endif