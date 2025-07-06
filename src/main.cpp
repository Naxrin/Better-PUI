#include <Geode/Geode.hpp>

using namespace geode::prelude;

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

	/*
	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {

	}
	void ccTouchEnded(CCTouch* touch, CCEvent* event) override {

	}
	
	void onPreview(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}*/
};

// rewrite
#include <Geode/modify/UIPOptionsLayer.hpp>
class $modify(PracticeOptionsLayer, UIPOptionsLayer) {
	struct Fields {
		CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	};

	bool init() override {
		if (!UIPOptionsLayer::init())
			return false;

		// bg color
		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bgcolor"));
		this->setOpacity(Mod::get()->getSettingValue<int64_t>("bgopacity"));

		this->m_buttonMenu->setID("button-menu");

		auto previewSpr = ButtonSprite::create("Preview");
		auto previewBtn = CCMenuItemSpriteExtra::create(previewSpr, this, menu_selector(PracticeOptionsLayer::onPreview));
		this->m_buttonMenu->addChild(previewBtn);

		return true;
		// test
		auto center = ccp(m_fields->winSize.width / 2, m_fields->winSize.height / 2);

		auto menu = CCMenu::create();

		// grid
		auto batch = CCNode::create();
		batch->setPosition(center);
		batch->setID("grid");
		this->addChild(batch);

		auto v = CCLayerColor::create(ccc4(255, 255, 255, 240));
		v->setPosition(ccp(0, 0));
		v->setContentSize(ccp(1.5, m_fields->winSize.height));
		v->ignoreAnchorPointForPosition(false);
		batch->addChild(v);

		auto h = CCLayerColor::create(ccc4(255, 255, 255, 240));
		h->setPosition(ccp(0, 0));
		h->setContentSize(ccp(m_fields->winSize.width, 1.5));
		h->ignoreAnchorPointForPosition(false);
		batch->addChild(h);

		float x;
		while (center.x - x > 0) {
			x += 30.f;
			auto l = CCLayerColor::create(ccc4(255, 255, 255, 144));
			l->setPosition(ccp(-x, 0));
			l->setContentSize(ccp(0.5, m_fields->winSize.height));
			l->ignoreAnchorPointForPosition(false);
			batch->addChild(l);

			auto r = CCLayerColor::create(ccc4(255, 255, 255, 144));
			r->setPosition(ccp(+x, 0));
			r->setContentSize(ccp(0.5, m_fields->winSize.height));
			r->ignoreAnchorPointForPosition(false);
			batch->addChild(r);
		}

		float y;
		while (center.y - y > 0) {
			y += 30.f;

			auto t = CCLayerColor::create(ccc4(255, 255, 255, 144));
			t->setPosition(ccp(0, -y));
			t->setContentSize(ccp(m_fields->winSize.width, 0.5));
			t->ignoreAnchorPointForPosition(false);
			batch->addChild(t);

			auto b = CCLayerColor::create(ccc4(255, 255, 255, 144));
			b->setPosition(ccp(0, +y));
			b->setContentSize(ccp(m_fields->winSize.width, 0.5));
			b->ignoreAnchorPointForPosition(false);
			batch->addChild(b);
		}

		return true;
	}

	void valueDidChange(int i, float f) override {
		UIPOptionsLayer::valueDidChange(i, f);
		log::debug("int = {}, float = {}", i, f);
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
		return false;
	}

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override {

	}

	void ccTouchEnded(CCTouch* touch, CCEvent* event) override {

	}
	
	void onPreview(CCObject*) {
		FLAlertLayer::create("Geode", "Hello from my custom mod!", "OK")->show();
	}
};

/*
#include <Geode/modify/SetupTriggerPopup.hpp>
class $modify(SetupTriggerPopup) {
	bool init(EffectGameObject* obj, CCArray* triggers, float width, float height, int bg) {
		log::debug("obj = {}, width = {}, height = {}, bg = {}", obj == nullptr, width, height, bg);
		return SetupTriggerPopup::init(obj, triggers, width, height, bg);
	}
};*/