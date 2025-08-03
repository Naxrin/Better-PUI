#include <Geode/ui/GeodeUI.hpp>
#include "head.hpp"

#include <Geode/modify/UIOptionsLayer.hpp>
class $modify(UIOptionsLayer) {
	struct Fields {
		const CCSize size = CCDirector::sharedDirector()->getWinSize();

		// ui to hide
		GameOptionsLayer* opl;

		// title
        CCLabelBMFont* titleLabel;
		// input menu
		PosInputBundle* posMenu;
		// several slider menus
		InputSliderBundle* opacityMenu;
		// map
		PracticePreviewFrame* map;

		// radio
		EventListener<EventFilter<Signal>> radio;
        // pos radio
        EventListener<EventFilter<PosSignal>> radioPos;
	};

	bool init(bool p) {
		if (!UIOptionsLayer::init(p))
			return false;

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
		return SetupTriggerPopup::ccTouchBegan(touch, event);
	}

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
		
	}
	void ccTouchEnded(CCTouch* touch, CCEvent* event) override {

	}
	
    void onOptions(CCObject*) {
		geode::openSettingsPopup(Mod::get(), false);
	}

	void onPreview(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}

	void onSnap(CCObject*) { }

	void onReset(CCObject*) { }

	void onSaveLoad(CCObject*) { }

	void onClose(CCObject* obj) override {
		UIOptionsLayer::onClose(obj);
	}

	void keyBackClicked() override {
		UIOptionsLayer::keyBackClicked();
	}
};

/*
#include <Geode/modify/UIOptionsLayer.hpp>
class $modify(UIOptionsLayer) {

	bool init(bool p) {
		if (!UIOptionsLayer::init(p))
			return false;

		
		if (p) {
			this->m_uiNode1->setID("ui-node-1");
			this->m_uiNode2->setID("ui-node-2");
			this->m_uiNode3->setID("ui-node-3");
			this->m_uiNode4->setID("ui-node-4");
			this->m_nameLabel->setID("name-label");
		}

		//

		return true;
	}

	void valueDidChange(int tag, float val) override {
		UIOptionsLayer::valueDidChange(tag, val);
		log::debug("valueDidChanged tag = {} val = {}", tag, val);
	}

	float getValue(int tag) override {
		float ret = UIOptionsLayer::getValue(tag);
		log::debug("getValue param = {}, ret = {}", tag, ret);
		return ret;
	}

	void toggleUIGroup(int p) {
		UIOptionsLayer::toggleUIGroup(p);
		log::debug("toggle ui group param = {}", p);
	}
};
*/