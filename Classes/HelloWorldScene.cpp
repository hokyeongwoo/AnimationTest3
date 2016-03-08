#include "HelloWorldScene.h"

USING_NS_CC;

HelloWorld::~HelloWorld()
{
	for (auto _targets : this->_targets)
	{
		_targets->release();
		_targets = NULL;
	}

	for (auto _projectiles : this->_projectiles)
	{
		_projectiles->release();
		_projectiles = NULL;
	}

	// cpp don't need to call super dealloc
	// virtual destructor will do this
}

HelloWorld::HelloWorld()
	:_targets(1000)
	, _projectiles(1000)
{
}

void HelloWorld::update(float dt)
{
	cocos2d::Vector<cocos2d::Sprite *> projectilesToDelete(1000);
	cocos2d::Vector<cocos2d::Sprite *> targetsToDelete(1000);

	//  loop through bullets
	for (auto projectile : this->_projectiles)
	{
		CCRect projectileRect = CCRectMake(
			projectile->getPosition().x - (projectile->getContentSize().width / 2),
			projectile->getPosition().y - (projectile->getContentSize().height / 2),
			projectile->getContentSize().width,
			projectile->getContentSize().height);
		for (auto target : this->_targets)
		{
			CCRect targetRect = CCRectMake(
				target->getPosition().x - (target->getContentSize().width / 2),
				target->getPosition().y - (target->getContentSize().height / 2),
				target->getContentSize().width,
				target->getContentSize().height);

			if (projectileRect.intersectsRect(targetRect))
			{
				targetsToDelete.pushBack(target);
				projectilesToDelete.pushBack(projectile);
			}
		}

	}

	for (auto target : targetsToDelete)
	{
		_targets.eraseObject(target);
		this->removeChild(target, true);
	}

	for (auto projectile : projectilesToDelete)
	{
		_projectiles.eraseObject(projectile);
		this->removeChild(projectile, true);
	}
}

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	bool bRet = false;

	do
	{
		//////////////////////////////
		// 1. super init first
		// I have to change the background to white.
		if (!CCLayerColor::initWithColor(ccc4(255, 255, 255, 255)))
		{
			return false;
		}

		Size visibleSize = Director::getInstance()->getVisibleSize();
		Vec2 origin = Director::getInstance()->getVisibleOrigin();

		/////////////////////////////
		// 2. add a menu item with "X" image, which is clicked to quit the program
		//    you may modify it.

		// add a "close" icon to exit the progress. it's an autorelease object
		auto closeItem = MenuItemImage::create(
			"CloseNormal.png",
			"CloseSelected.png",
			CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));

		closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
			origin.y + closeItem->getContentSize().height / 2));

		// create menu, it's an autorelease object
		auto menu = Menu::create(closeItem, NULL);
		menu->setPosition(Vec2::ZERO);
		this->addChild(menu, 1);

		/////////////////////////////
		// 3. add your codes below...
		Size winSize = Director::sharedDirector()->getWinSize();
		Sprite* player = Sprite::create("Player.png", CCRectMake(0, 0, 27, 40));
		player->setPosition((player->getContentSize().width / 2), winSize.height / 2);
		this->addChild(player);
		bRet = true;

		this->schedule(schedule_selector(HelloWorld::gameLogic), 1.0);

		auto listener = EventListenerTouchAllAtOnce::create();
		listener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

		this->schedule(schedule_selector(HelloWorld::update));
	} while (0);

	return bRet;
 }

void HelloWorld::addTarget()
{
	Sprite *target = Sprite::create("Target.png",
		CCRectMake(0, 0, 27, 40));
	
	target->setTag(1);
	_targets.pushBack(target);

	// Determine where to spawn the target along the Y axis
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int minY = target->getContentSize().height / 2;
	int maxY = winSize.height
		- target->getContentSize().height / 2;
	int rangeY = maxY - minY;
	// srand( TimGetTicks() );
	int actualY = (rand() % rangeY) + minY;

	// Create the target slightly off-screen along the right edge,
	// and along a random position along the Y axis as calculated
	target->setPosition(
		ccp(winSize.width + (target->getContentSize().width / 2),
			actualY));
	this->addChild(target);

	// Determine speed of the target
	int minDuration = (int)2.0;
	int maxDuration = (int)4.0;
	int rangeDuration = maxDuration - minDuration;
	// srand( TimGetTicks() );
	int actualDuration = (rand() % rangeDuration)
		+ minDuration;

	// Create the actions
	FiniteTimeAction* actionMove =
		MoveTo::create((float)actualDuration,
			ccp(0 - target->getContentSize().width / 2, actualY));
	FiniteTimeAction* actionMoveDone =
		CallFuncN::create(this,
			callfuncN_selector(HelloWorld::spriteMoveFinished));
	target->runAction(Sequence::create(actionMove,
		actionMoveDone, NULL));
}

void HelloWorld::gameLogic(float dt)
{
	this->addTarget();
}

void HelloWorld::onTouchesEnded(const std::vector<cocos2d::Touch*>& pTouches, cocos2d::Event *pEvent)
{
	// Choose one of the touches to work with
	for (auto &touch : pTouches)
	{
		Point location = touch->getLocationInView();
		location = Director::sharedDirector()->convertToGL(location);

		// Set up initial location of projectile
		Size winSize = Director::sharedDirector()->getWinSize();
		Sprite *projectile = Sprite::create("Projectile.png",
			CCRectMake(0, 0, 20, 20));
		projectile->setPosition(ccp(20, winSize.height / 2));

		projectile->setTag(2);
		_projectiles.pushBack(projectile);

		// Determinie offset of location to projectile
		int offX = location.x - projectile->getPosition().x;
		int offY = location.y - projectile->getPosition().y;

		// Bail out if we are shooting down or backwards
		if (offX <= 0) return;

		// Ok to add now - we've double checked position
		this->addChild(projectile);

		// Determine where we wish to shoot the projectile to
		int realX = winSize.width
			+ (projectile->getContentSize().width / 2);
		float ratio = (float)offY / (float)offX;
		int realY = (realX * ratio) + projectile->getPosition().y;
		Point realDest = ccp(realX, realY);

		// Determine the length of how far we're shooting
		int offRealX = realX - projectile->getPosition().x;
		int offRealY = realY - projectile->getPosition().y;
		float length = sqrtf((offRealX * offRealX)
			+ (offRealY*offRealY));
		float velocity = 480 / 1; // 480pixels/1sec
		float realMoveDuration = length / velocity;

		// Move projectile to actual endpoint
		projectile->runAction(CCSequence::create(
			MoveTo::create(realMoveDuration, realDest),
			CallFuncN::create(this, callfuncN_selector(HelloWorld::spriteMoveFinished)), NULL));
	}
}

void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
