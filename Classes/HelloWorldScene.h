#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

class HelloWorld : public cocos2d::CCLayerColor
{
public:
	HelloWorld();
	~HelloWorld();

public:
    static cocos2d::Scene* createScene();

	void update(float dt);
    virtual bool init();

	void addTarget();
	virtual void onTouchesEnded(const std::vector<cocos2d::Touch*>& pTouches, cocos2d::Event *pEvent);
public:
	// a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);

	void spriteMoveFinished(cocos2d::Node* sender)
	{
		cocos2d::Sprite *sprite = (cocos2d::Sprite *)sender;
		this->removeChild(sprite, true);

		if (sprite->getTag() == 1)  // target
		{
			_targets.eraseObject(sprite);
		}
		else if (sprite->getTag() == 2) // projectile
		{
			_projectiles.eraseObject(sprite);
		}
	}
	void gameLogic(float dt);

    // implement the "static create()" method manually
	CREATE_FUNC(HelloWorld);

protected:
	cocos2d::Vector<cocos2d::Sprite *> _targets;
	cocos2d::Vector<cocos2d::Sprite *> _projectiles;
};

#endif // __HELLOWORLD_SCENE_H__
