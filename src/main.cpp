#include "Geode/cocos/cocoa/CCObject.h"
#include "head.hpp"

/*
void makePosInputBundle(auto target, SEL_MenuHandler callback, ) {

}
*/

/*
#include <Geode/modify/UIOptionsLayer.hpp>
class $modify(UIOptionsLayer) {

	bool init(bool p) {
		if (!UIOptionsLayer::init(p))
			return false;

		log::debug("UIOptionsLayer::init p = {}", p);

		auto winSize = CCDirector::sharedDirector()->getWinSize();

		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bgcolor"));
		this->setOpacity(Mod::get()->getSettingValue<int64_t>("bgopacity"));

		return true;
	}
	
	
	void valueDidChange(int i, float f) override {
		UIOptionsLayer::valueDidChange(i, f);
		log::debug("int = {}, float = {}", i, f);
	}


	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {

	}
	void ccTouchEnded(CCTouch* touch, CCEvent* event) override {

	}
	
	void onPreview(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};*/

// practice
#include <Geode/modify/UIPOptionsLayer.hpp>
class $modify(PracticeOptionsLayer, UIPOptionsLayer) {
	struct Fields {
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
		// input menu
		PosInputBundle* posMenu;
		// opacity menu
		InputSliderBundle* opacityMenu;
		// node
		PreviewFrame* map;

		// radio
		EventListener<EventFilter<Signal>> radio;
	};

	bool init() override {
		if (!SetupTriggerPopup::init(nullptr, nullptr, 420.f, 280.f, 1))
			return false;

		this->m_fields->radio = EventListener<EventFilter<Signal>>(
            [this](Signal* event) -> ListenerResult { return this->handleSignal(event); });

		// bg color
		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bgcolor"));
		this->setOpacity(Mod::get()->getSettingValue<int64_t>("bgopacity"));

		// hide the frame
		this->m_mainLayer->getChildByTag(1)->setVisible(false);

		// the center map who can be scaled in game
		m_fields->map = PreviewFrame::create();
		m_fields->map->setPosition(ccp(0.f, 0.f));
		m_fields->map->placeNode(gm->m_practicePos);
		this->addChild(m_fields->map);

		// relocate the practice node
		this->m_practiceNode = m_fields->map->getTargetNode();

		m_fields->posMenu = PosInputBundle::create();
		m_fields->posMenu->setPositionY(80.f);
		this->addChild(m_fields->posMenu);

		m_fields->opacityMenu = InputSliderBundle::create("Opacity = ", -29, 0, 1, 2);
		m_fields->opacityMenu->setPositionY(60.f);
		this->addChild(m_fields->opacityMenu);

		return true;
	}

	ListenerResult handleSignal(Signal* event) {
		log::debug("signal handled!");
		// opacityß
		if (event->tag == -29) {
			this->valueDidChange(-29, event->value);
			this->m_fields->map->alphaNode(event->value);
			log::debug("opacity is set to {}", event->value);
		}
		// x pos
		else if (event->tag == 114) {
			GameManager::sharedState()->m_practicePos.x = event->value;
			this->m_practiceNode->setPositionX(event->value);
		}
		// y pos
		else if (event->tag == 514) {
			GameManager::sharedState()->m_practicePos.y = event->value;	
			this->m_practiceNode->setPositionY(event->value);
		}
		
		return ListenerResult::Stop;		
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override { return false; }

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override { }

	void ccTouchEnded(CCTouch* touch, CCEvent* event) override { }
	
	void textChanged(CCTextInputNode* input) override { }

	void onPreview(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}

	void onClose(CCObject* obj) override {
		SetupTriggerPopup::onClose(obj);
	}
};