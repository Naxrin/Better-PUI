#include "head.hpp"
#include <Geode/ui/GeodeUI.hpp>

// platform
#include <Geode/modify/UIOptionsLayer.hpp>
class $modify(PlatformOptionsLayer, UIOptionsLayer) {
	struct Fields {
		const CCSize size = CCDirector::sharedDirector()->getWinSize();

		UIButtonConfig* config[5] = {&gm->m_dpad1, &gm->m_dpad2, &gm->m_dpad3, &gm->m_dpad4, &gm->m_dpad5};
		// ui to hide
		GameOptionsLayer* opl;

		// pages
		//std::vector<Pages> pages;

		// on browsing preview
		int slpage;
		// current id
		int id;
		// in preview
		bool inprev;

		// title
          CCLabelBMFont* titleLabel;
		// planar menus
		PosInputBundle* posMenu;
		// input slider menus
		InputSliderBundle* widthMenu, * heightMenu, * scaleMenu, * opacityMenu, * deadzoneMenu, * radiusMenu;
		// map
		PlatformPreviewFrame* map, * preview;

		// some sprites
		CCMenuItemSpriteExtra* dualBtn, * snapBtn;

		// labels
		CCLabelBMFont* modeLabel, * snapLabel, * splitLabel, * jumplLabel, * symmetryLabel;

		// the btns from label sprites
		CCMenuItemSpriteExtra* modeBtn, * snap0Btn, * splitBtn, * jumplBtn, * symmetryBtn;

		// slots
		SlotFrame* slots[3];
		// radios
		ListenerHandle radioReal, radioPos;
	};

	bool init(bool p) {
		if (!SetupTriggerPopup::init(nullptr, nullptr, 420.f, 280.f, 1))
			return false;
		
		this->m_fields->radioReal = Signal().listen(
            [this](int tag, float value) -> ListenerResult {
			this->handleSignal(tag, value);
            return ListenerResult::Stop;
		});

		this->m_fields->radioPos = PosSignal().listen(
			[this] (int tag, CCPoint pos) -> ListenerResult {
				//log::debug("pos signal {}", event->tag);
                this->m_fields->posMenu->setValue(pos);
                this->m_fields->config[m_fields->id]->m_position = m_fields->id == 2 || m_fields->id == 4 ? ccp(m_fields->size.width - pos.x, pos.y) : pos;
				// symmetric dual
				if (!tag || !m_fields->id || !Mod::get()->getSavedValue<bool>("symmetry"))
					return ListenerResult::Stop;
				auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
				//log::debug("mirror = {}", mirror);
				this->m_fields->config[mirror]->m_position = m_fields->id == 2 || m_fields->id == 4 ? ccp(m_fields->size.width - pos.x, pos.y) : pos;
				this->m_fields->map->placeNode(mirror, ccp(m_fields->size.width - pos.x, pos.y), 0.2);
                return ListenerResult::Stop;
			}
		);
		
		log::debug("m_dpadLayout1 = {}", gm->m_dpadLayout1);
		log::debug("m_dpadLayout2 = {}", gm->m_dpadLayout2);
		log::debug("m_dpadLayout3 = {}", gm->m_dpadLayout3);
		log::debug("m_dpadLayoutDual1 = {}", gm->m_dpadLayoutDual1);
		log::debug("m_dpadLayoutDual2 = {}", gm->m_dpadLayoutDual2);
		log::debug("m_dpadLayoutDual3 = {}", gm->m_dpadLayoutDual3);
		
		// game options layer below
		m_fields->opl = CCScene::get()->getChildByType<GameOptionsLayer>(0);

		// bg color
		this->setColor(Mod::get()->getSettingValue<ccColor3B>("bg-color"));
		this->setOpacity(0);

		m_fields->id = Mod::get()->getSavedValue<bool>("dual");

		// hide the frame
		this->m_mainLayer->getChildByTag(1)->setVisible(false);

        m_fields->titleLabel = CCLabelBMFont::create("Platform UI Modifier", "goldFont.fnt");
        m_fields->titleLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        m_fields->titleLabel->setPosition(ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 80.f));
        m_fields->titleLabel->setScale(0.35);
        this->m_mainLayer->addChild(m_fields->titleLabel);

		// the center map who can be scaled in game
		m_fields->map = PlatformPreviewFrame::create(false);
        m_fields->map->setPosition(ccp(100.f, 30.f));
        m_fields->map->setScale(0.25);
		m_fields->map->setTag(114);
		this->m_mainLayer->addChild(m_fields->map);

		// the preview of slot data
		m_fields->preview = PlatformPreviewFrame::create(true);
        m_fields->preview->setPosition(ccp(0.f, 30.f));
        m_fields->preview->setScale(0);
		m_fields->preview->setTag(514);
		this->m_mainLayer->addChild(m_fields->preview);

		this->m_uiNode1 = this->m_fields->map->getTargetNode(m_fields->id);
		this->m_uiNode2 = this->m_fields->map->getTargetNode(2);
		this->m_uiNode3 = this->m_fields->map->getTargetNode(3);
		this->m_uiNode4 = this->m_fields->map->getTargetNode(4);

        // pos menu
		m_fields->posMenu = PosInputBundle::create();
		m_fields->posMenu->setPositionY(m_fields->size.height / 4 - 130.f);
		m_fields->posMenu->setScale(0.5);
		this->m_mainLayer->addChild(m_fields->posMenu);

		auto panel = CCMenu::create();
		//panel->setContentSize({0, 0});
		panel->setPosition(ccp(-80.f, m_fields->size.height / 2 + 30.f));
		panel->setContentSize(ccp(0.f, 0.f));
		panel->setScale(0.35);
		panel->setID("panel");
		this->m_mainLayer->addChild(panel);

		this->m_nameLabel = CCLabelBMFont::create(m_fields->id ? "P1 Move" : "Single Mode", "bigFont.fnt");
		this->m_nameLabel->setPosition(ccp(m_fields->size.width / 2, 100.f));
		this->m_nameLabel->setScale(0.7);
		panel->addChild(this->m_nameLabel);

        // width menu
		m_fields->widthMenu = InputSliderBundle::create("Width", 0, 400.f, 0);
		m_fields->widthMenu->setPositionY(60.f);
		m_fields->widthMenu->setTag(-1);
		panel->addChild(m_fields->widthMenu);

		// height menu
		m_fields->heightMenu = InputSliderBundle::create("Height", 0, 400.f, 0);
		m_fields->heightMenu->setPositionY(30.f);
		m_fields->heightMenu->setTag(-5);
		panel->addChild(m_fields->heightMenu);

		// scale menu
		m_fields->scaleMenu = InputSliderBundle::create("Scale", 0, 2, 2);
		m_fields->scaleMenu->setPositionY(0.f);
		m_fields->scaleMenu->setTag(-9);
		panel->addChild(m_fields->scaleMenu);

		// opacity menu
		m_fields->opacityMenu = InputSliderBundle::create("Opacity", 0, 1, 2);
		m_fields->opacityMenu->setPositionY(-30.f);
		m_fields->opacityMenu->setTag(-13);
		panel->addChild(m_fields->opacityMenu);

		// deadzone menu
		m_fields->deadzoneMenu = InputSliderBundle::create("Deadzone", 0, 10, 1);
		m_fields->deadzoneMenu->setPositionY(-60.f);
		m_fields->deadzoneMenu->setTag(-21);
		panel->addChild(m_fields->deadzoneMenu);

		// radius menu
		m_fields->radiusMenu = InputSliderBundle::create("Radius", 0, 50, 1);
		m_fields->radiusMenu->setPositionY(-90.f);
		m_fields->radiusMenu->setTag(-19);
		panel->addChild(m_fields->radiusMenu);

		auto optMenu = CCMenu::create();
		optMenu->setPosition(ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 130.f));
		optMenu->setContentSize(ccp(200.f, 40.f));
		optMenu->setScale(0.f);
		optMenu->setID("official-options");
		this->m_mainLayer->addChild(optMenu);

		m_fields->modeLabel = CCLabelBMFont::create("Mode B", "bigFont.fnt");
		m_fields->modeLabel->setScale(0.6);
		m_fields->modeBtn = CCMenuItemSpriteExtra::create(m_fields->modeLabel, this, menu_selector(PlatformOptionsLayer::onMode));
		m_fields->modeBtn->setTag(-17);
		optMenu->addChildAtPosition(m_fields->modeBtn, Anchor::Center, ccp(-180.f, 0.f));

		m_fields->snapLabel = CCLabelBMFont::create("Snap", "bigFont.fnt");
		m_fields->snapLabel->setScale(0.6);
		m_fields->snap0Btn = CCMenuItemSpriteExtra::create(m_fields->snapLabel, this, menu_selector(PlatformOptionsLayer::onSnap0));
		m_fields->snap0Btn->setTag(-24);
		optMenu->addChildAtPosition(m_fields->snap0Btn, Anchor::Center, ccp(-60.f, 0.f));

		m_fields->splitLabel = CCLabelBMFont::create("Split", "bigFont.fnt");
		m_fields->splitLabel->setScale(0.6);
		m_fields->splitBtn = CCMenuItemSpriteExtra::create(m_fields->splitLabel, this, menu_selector(PlatformOptionsLayer::onSplit));
		m_fields->splitBtn->setTag(-27);
		optMenu->addChildAtPosition(m_fields->splitBtn, Anchor::Center, ccp(60.f, 0.f));

		m_fields->jumplLabel = CCLabelBMFont::create("Jump L", "bigFont.fnt");
		m_fields->jumplLabel->setScale(0.6);
		m_fields->jumplBtn = CCMenuItemSpriteExtra::create(m_fields->jumplLabel, this, menu_selector(PlatformOptionsLayer::onJumpL));
		m_fields->jumplBtn->setTag(-26);
		optMenu->addChildAtPosition(m_fields->jumplBtn, Anchor::Center, ccp(180.f, 0.f));

		m_fields->symmetryLabel = CCLabelBMFont::create("Symmetry", "bigFont.fnt");
		m_fields->symmetryLabel->setScale(0.6);
		m_fields->symmetryBtn = CCMenuItemSpriteExtra::create(m_fields->symmetryLabel, this, menu_selector(PlatformOptionsLayer::onSymmetry));
		optMenu->addChildAtPosition(m_fields->symmetryBtn, Anchor::Center, ccp(180.f, 0.f));

		m_fields->jumplLabel->setColor(this->getValue(-26) ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
		m_fields->symmetryLabel->setColor(Mod::get()->getSavedValue<bool>("symmetry") ? ccc3(128, 255, 128) : ccc3(255, 128, 128));

		m_fields->jumplBtn->setScale(m_fields->id == 0);
		m_fields->symmetryBtn->setScale(m_fields->id != 0);

		auto slotNode = CCNode::create();
		slotNode->setID("save-slots");
		slotNode->setPosition(this->m_fields->size / 2);
		this->m_mainLayer->addChild(slotNode);

		// slots
		this->m_fields->slots[0] = SlotFrame::create(1);
		this->m_fields->slots[0]->setPosition(ccp(0.f, 80.f));
		slotNode->addChild(this->m_fields->slots[0]);
		
		this->m_fields->slots[1] = SlotFrame::create(2);
		this->m_fields->slots[1]->setPosition(ccp(0.f, 0.f));
		slotNode->addChild(this->m_fields->slots[1]);

		this->m_fields->slots[2] = SlotFrame::create(3);
		this->m_fields->slots[2]->setPosition(ccp(0.f, -80.f));
		slotNode->addChild(this->m_fields->slots[2]);

		// buttons below
        std::map<int, std::pair<const char*, SEL_MenuHandler>> btnIndexes = {
            {-120, {"optionsBtn.png"_spr, menu_selector(PlatformOptionsLayer::onOptions)}},			
			{-80, {"dualBtn.png"_spr, menu_selector(PlatformOptionsLayer::onDual)}},
			{-40, {"slotBtn.png"_spr, menu_selector(PlatformOptionsLayer::onBrowseSlot)}},
            {0, {"fullscreenBtn.png"_spr, menu_selector(PlatformOptionsLayer::onPreview)}},
            {40, {"snapBtn.png"_spr, menu_selector(PlatformOptionsLayer::onSnap)}},
            {80, {"resetBtn.png"_spr, menu_selector(PlatformOptionsLayer::onResetNew)}},
            {120, {"applyBtn.png"_spr, menu_selector(PlatformOptionsLayer::onClose)}}
        };


		auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

        for (auto [k, v] : btnIndexes) {
            auto spr = CCSprite::create(v.first);
            spr->setScale(0.6);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, v.second);
			btn->setPositionX(k);

            this->m_buttonMenu->addChild(btn);

			if (v.first == "dualBtn.png"_spr) {
				this->m_fields->dualBtn = btn;
				btn->setTag(10);
				btn->setColor(m_fields->id ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
			}

			else if (v.first == "snapBtn.png"_spr) {
				this->m_fields->snapBtn = btn;
				btn->setTag(10);
				btn->setColor(Mod::get()->getSavedValue<bool>("snap") ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
			}

			else
				btn->setColor(color);
        }

		this->m_buttonMenu->setPositionY(-5.f);
		this->m_buttonMenu->setContentSize(ccp(0.f, 0.f));
		this->m_buttonMenu->setScale(0.5);
        this->m_buttonMenu->setID("button-menu");

		// value
		this->refreshValue();
		// transition
		this->Transition(true, true);

		
		if (this->m_fields->id) {
			this->m_fields->map->updateState(1, gm->m_dpad2);
			this->m_fields->map->updateState(2, gm->m_dpad3);
			this->m_fields->map->updateState(3, gm->m_dpad4);
			this->m_fields->map->updateState(4, gm->m_dpad5);
		} else
			this->m_fields->map->updateState(0, gm->m_dpad1);

		if (Mod::get()->getSettingValue<bool>("dont-crash"))
			return true;

		/*
		listenForSettingChangesV3("bg-color", [this] (ccColor3B val) { this->runAction(CCTintTo::create(0.2, val.r, val.g, val.b)); });
		listenForSettingChangesV3("bg-opacity", [this] (int val) { this->runAction(CCFadeTo::create(0.2, val * 255 / 100.f)); });
		listenForSettingChangesV3("ui-color", [this] (ccColor3B val) {
			for (auto child : CCArrayExt<CCMenuItemSpriteExtra*>(this->m_buttonMenu->getChildren()))
				if (child->getTag() != 10)
					child->setColor(val);
		});
		*/
		return true;
	}

	void handleSignal(int tag, float value) {
		//log::debug("handle signal {} {}", event->tag, event->value);
		// escape from fullscreen preview
		if (tag == -100) {
			if (value == -514 && !this->m_fields->slpage)
				return;
			// will drag a node
			if (value > 0) {
				// dual mode only
				// should switch ui info to the dragged node
				if (this->m_fields->id) {
					this->m_fields->id = value;
					//log::info("switch id = {}", m_fields->id);
					bool p2 = value == 2 || value == 4;
					this->m_nameLabel->setString(fmt::format("P{} {}", p2 ? "2" : "1", value < 3 ? "Move" : "Jump").c_str());
					this->refreshValue();
					// input slider bundles tag
					this->m_fields->widthMenu->setTag(-value);
					this->m_fields->heightMenu->setTag(-value - 4);
					this->m_fields->scaleMenu->setTag(-value - 8);
					this->m_fields->opacityMenu->setTag(-value - 12);
					this->m_fields->deadzoneMenu->setTag(-int(p2) - 21);
					this->m_fields->radiusMenu->setTag(-int(p2) - 19);

					static_cast<TextInput*>(this->m_fields->deadzoneMenu->getChildByID("inputer"))->setEnabled(value < 3);
					static_cast<TextInput*>(this->m_fields->radiusMenu->getChildByID("inputer"))->setEnabled(value < 3);
					this->m_fields->deadzoneMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, value < 3)));
					this->m_fields->radiusMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, value < 3)));
					// togglers
					this->m_fields->modeBtn->setTag(-17 - p2);
					this->m_fields->snap0Btn->setTag(-24 - p2);
					this->m_fields->splitBtn->setTag(-27 - p2);
					// buttons
					this->m_fields->modeBtn->setEnabled(value < 3);
					this->m_fields->snap0Btn->setEnabled(value < 3);
					this->m_fields->splitBtn->setEnabled(value < 3);
					if (value < 3) {
						this->m_fields->modeLabel->runAction(this->m_fields->config[m_fields->id]->m_modeB ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
						this->m_fields->snapLabel->runAction(this->m_fields->config[m_fields->id]->m_snap ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
						this->m_fields->splitLabel->runAction(this->m_fields->config[m_fields->id]->m_split ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
					} else {
						this->m_fields->modeLabel->runAction(CCTintTo::create(0.4, 128, 128, 128));
						this->m_fields->snapLabel->runAction(CCTintTo::create(0.4, 128, 128, 128));
						this->m_fields->splitLabel->runAction(CCTintTo::create(0.4, 128, 128, 128));
					}
				}
			}
			// escape fullscreen preview
			else {
				//log::debug("escape fullscreen preview {} {}", value, this->m_fields->slpage);

				if (value == -114 && !this->m_fields->slpage && this->m_fields->inprev)
					this->Transition(true, false);
				else if (value == -514 && this->m_fields->slpage && this->m_fields->inprev)
					this->TransitionSlots(true, true);
			}
		}
		// x pos
		else if (tag == 114) {
            auto m = this->m_fields->map;
			this->m_fields->config[m_fields->id]->m_position.x = m_fields->id == 2 || m_fields->id == 4 ? m_fields->size.width - value : value;
			m->placeNode(m_fields->id, ccp(value, m->getChildByTag(m_fields->id)->getPositionY()), 0.2);
			// symmetric dual
			if (!m_fields->id || !Mod::get()->getSavedValue<bool>("symmetry"))
				return;
			auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
			this->m_fields->config[mirror]->m_position = ccp(m_fields->id == 2 || m_fields->id == 4 ? m_fields->size.width - value : value, this->m_fields->config[m_fields->id]->m_position.y);
			this->m_fields->map->placeNode(mirror, ccp(m_fields->size.width - value, this->m_fields->config[m_fields->id]->m_position.y), 0.2);
		}
		// y pos
		else if (tag == 514) {
            auto m = this->m_fields->map;
			this->m_fields->config[m_fields->id]->m_position.y = value;
			m->placeNode(m_fields->id, ccp(m->getChildByTag(m_fields->id)->getPositionX(), value), 0.2);
			// symmetric dual
			if (!m_fields->id || !Mod::get()->getSavedValue<bool>("symmetry"))
				return;
			auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
			this->m_fields->config[mirror]->m_position = ccp(this->m_fields->config[m_fields->id]->m_position.x, value);
			this->m_fields->map->placeNode(mirror, ccp(this->m_fields->config[m_fields->id]->m_position.x, value), 0.2);
		}
		// launch slot preview
		else if (tag == 1919) {
			if (value == -1) {
				this->TransitionSlots(true, true);
				return;
			}
			else if (!value) {
				this->showNotify("Empty Slot!", true);
				return;
			}
			//auto slot = value ? value : this->m_fields->slpage;
			this->m_fields->slpage = value;
			auto slot = static_cast<SlotFrame*>(this->m_mainLayer->getChildByID("save-slots")->getChildByTag(value));
			//log::debug("signal 1919 id = {}", m_fields->id);
			if (this->m_fields->id) {
				this->m_fields->preview->updateState(1, slot->p1m);	
				this->m_fields->preview->updateState(2, slot->p2m);	
				this->m_fields->preview->updateState(3, slot->p1j);	
				this->m_fields->preview->updateState(4, slot->p2j);	
			} else
				this->m_fields->preview->updateState(0, slot->p1m);	

			this->TransitionSlots(false, true);
		}
		// load a slot
		else if (tag == 810) {
			if (value) {
				this->refreshValue();
				if (this->m_fields->slpage > 0)
					this->TransitionSlots(true, true);
				// notify
				this->showNotify(fmt::format("Config inside Slot {} has been applied as current.", value).c_str());				
			} else
				this->showNotify("Empty Slot!", true);
		}
		// save to slot
		else if (tag == 94) {
			// currently showing the slot
			if (value > 3) {
				if (this->m_fields->id) {
					this->m_fields->preview->updateState(1, gm->m_dpad2, 0.2);
					this->m_fields->preview->updateState(2, gm->m_dpad3, 0.2);
					this->m_fields->preview->updateState(3, gm->m_dpad4, 0.2);
					this->m_fields->preview->updateState(4, gm->m_dpad5, 0.2);
				} else
					this->m_fields->preview->updateState(0, gm->m_dpad1, 0.2);				
			}
			// FLAertLayer
			this->showNotify(fmt::format("Current config has been dumped to Slot {}.", (int)value % 3).c_str());
		}
		// opacity and more
		else if (tag < 0) {
			if (tag >= -4 && tag <= -1) {
				this->m_fields->config[m_fields->id]->m_width = value;
				this->valueDidChange(tag, value);
				if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
					auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
					this->m_fields->config[mirror]->m_width = value;
					this->valueDidChange(tag > -3 ? -3 - tag : -7 - tag, value);			
				}
			}
			if (tag >= -8 && tag <= -5) {
				this->m_fields->config[m_fields->id]->m_height = value;
				this->valueDidChange(tag, value);
				if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
					auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
					this->m_fields->config[mirror]->m_height = value;
					this->valueDidChange(tag > -7 ? -11 - tag : -15 - tag, value);
				}
			}
			if (tag >= -12 && tag <= -9) {
				this->m_fields->config[m_fields->id]->m_scale = value;
				this->m_fields->map->scaleNode(m_fields->id, value);
				if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
					auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
					this->m_fields->config[mirror]->m_scale = value;
					this->m_fields->map->scaleNode(mirror, value);
				}
			}
			if (tag >= -16 && tag <= -13) {
				this->m_fields->config[m_fields->id]->m_opacity = 255 * value;
				this->m_fields->map->alphaNode(m_fields->id, 255 * value);
				if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
					auto mirror = m_fields->id < 3 ? 3 - m_fields->id : 7 - m_fields->id;
					this->m_fields->config[mirror]->m_opacity = 255 * value;
					this->m_fields->map->alphaNode(mirror, 255 * value);
				}
			}
			if (tag == -21 || tag == -22) {
				this->m_fields->config[m_fields->id]->m_deadzone = value;
				this->valueDidChange(tag, value);
				if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
					auto mirror = 3 - m_fields->id;
					this->m_fields->config[mirror]->m_deadzone = value;
					this->valueDidChange(-43 - tag, value);
				}
			}
			if (tag == -19 || tag == -20) {
				this->m_fields->config[m_fields->id]->m_radius = value;
				//this->m_fields->map->radiusNode(m_fields->id, value);
				this->valueDidChange(tag, value);
				if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
					auto mirror = 3 - m_fields->id;
					this->m_fields->config[mirror]->m_radius = value;
					this->valueDidChange(-39 - tag, value);
					//this->m_fields->map->radiusNode(mirror, value);
				}
			}
		}
	}
	
	// transition for main menu
	// whole means enter or exit the whole ui config menu
	void Transition(bool in, bool whole) {
		if (whole) {
			this->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, in * Mod::get()->getSettingValue<int64_t>("bg-opacity") * 255 / 100)));
			
			this->m_fields->map->stopAllActions();
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.25 * (in + 1))));
			m_fields->map->helpTransition(in);
		} else {
			this->m_fields->inprev = !in;
			m_fields->opl->setVisible(in);
			this->m_fields->map->stopAllActions();
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 1 - 0.5 * in)));
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(100.f * in, 30.f * in))));
		}

		this->m_fields->titleLabel->stopAllActions();
		this->m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.35 * (in + 1))));
		this->m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 100.f - in * 50.f))));

		this->m_fields->posMenu->stopAllActions();
		this->m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5 * (in + 1))));
		this->m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 110.f))));

		this->m_mainLayer->getChildByID("panel")->stopAllActions();
		this->m_mainLayer->getChildByID("panel")->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.7 * in)));

		this->m_mainLayer->getChildByID("official-options")->stopAllActions();
		this->m_mainLayer->getChildByID("official-options")->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 80.f))));
		this->m_mainLayer->getChildByID("official-options")->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, in)));

		this->m_buttonMenu->stopAllActions();
		m_buttonMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5 * (in + 1))));
		m_buttonMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + in * 45.f))));
	
		if (!whole) {
			static_cast<TextInput*>(this->m_fields->widthMenu->getChildByID("inputer"))->setEnabled(in);
			static_cast<TextInput*>(this->m_fields->heightMenu->getChildByID("inputer"))->setEnabled(in);
			static_cast<TextInput*>(this->m_fields->scaleMenu->getChildByID("inputer"))->setEnabled(in);
			static_cast<TextInput*>(this->m_fields->opacityMenu->getChildByID("inputer"))->setEnabled(in);
			static_cast<TextInput*>(this->m_fields->deadzoneMenu->getChildByID("inputer"))->setEnabled(in && m_fields->id < 3);
			static_cast<TextInput*>(this->m_fields->radiusMenu->getChildByID("inputer"))->setEnabled(in && m_fields->id < 3);
		}
	}

	// transition for slots
	void TransitionSlots(bool in, bool prev) {

		// go to preview
		if (prev) {
			this->m_fields->inprev = !in;
			this->m_fields->preview->setTouchEnabled(!in);
			//m_fields->opl->setVisible(in);
			this->m_fields->titleLabel->stopAllActions();
			this->m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.35 * (in + 1))));
			this->m_fields->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height * 3 / 4 + 100.f - in * 50.f))));

			for (auto slot : this->m_fields->slots) {
				slot->stopAllActions();				
				if (in)
					slot->resume();
				if (slot->getTag() == this->m_fields->slpage)
					continue;					

				slot->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(0, 80.f * (2 - slot->getTag()) + 120.f * (!in) / (slot->getTag() - this->m_fields->slpage))))); // what the fuck?
				slot->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, in)));
				slot->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 255 * in)));
			}

			this->m_fields->preview->stopAllActions();
			this->m_fields->preview->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5 * (!in))));
			this->m_fields->preview->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(0.f, 30.f * !in))));
			this->m_fields->preview->helpTransition(!in);

			if (in) 
				this->m_fields->slpage = -1;
		}
		// from main menu
		else {
			this->m_fields->map->setTouchEnabled(!in);

			static_cast<TextInput*>(this->m_fields->widthMenu->getChildByID("inputer"))->setEnabled(!in);
			static_cast<TextInput*>(this->m_fields->heightMenu->getChildByID("inputer"))->setEnabled(!in);
			static_cast<TextInput*>(this->m_fields->scaleMenu->getChildByID("inputer"))->setEnabled(!in);
			static_cast<TextInput*>(this->m_fields->opacityMenu->getChildByID("inputer"))->setEnabled(!in);
			static_cast<TextInput*>(this->m_fields->deadzoneMenu->getChildByID("inputer"))->setEnabled(!in && m_fields->id < 3);
			static_cast<TextInput*>(this->m_fields->radiusMenu->getChildByID("inputer"))->setEnabled(!in && m_fields->id < 3);	

			if (in) 
				this->m_fields->slpage = -1;
			else {
				this->m_fields->slpage = 0;
				if (this->m_fields->id) {
					this->m_fields->map->updateState(1, gm->m_dpad2);
					this->m_fields->map->updateState(2, gm->m_dpad3);
					this->m_fields->map->updateState(3, gm->m_dpad4);
					this->m_fields->map->updateState(4, gm->m_dpad5);
				} else
					this->m_fields->map->updateState(0, gm->m_dpad1);	
			}

			for (auto slot : this->m_fields->slots) {
				slot->stopAllActions();
				slot->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, in)));
				slot->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 255 * in)));
			}

			this->m_fields->map->stopAllActions();
			this->m_fields->map->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.25 * (!in + 1))));
			this->m_fields->map->helpTransition(!in);

			this->m_fields->posMenu->stopAllActions();
			this->m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5 * (!in + 1))));
			this->m_fields->posMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + !in * 110.f))));

			this->m_mainLayer->getChildByID("panel")->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.7 * !in)));

			this->m_mainLayer->getChildByID("official-options")->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + !in * 80.f))));
			this->m_mainLayer->getChildByID("official-options")->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, !in)));

			this->m_buttonMenu->stopAllActions();
			this->m_buttonMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5 * (!in + 1))));
			this->m_buttonMenu->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(m_fields->size.width / 2, m_fields->size.height / 4 - 100.f + !in * 45.f))));
		}

	}

	void showNotify(const char* str, bool warning = false) {
		auto notifyWidget = CCNode::create();
		notifyWidget->setPosition(ccp(m_fields->size.width / 2, -6.f));
		notifyWidget->setContentSize(ccp(0.f, 0.f));
		notifyWidget->setID("notify-widget");
		this->m_mainLayer->addChild(notifyWidget);

		auto notifyBar = CCLayerColor::create(ccc4(255, 255, 255, 144));
		notifyBar->setContentSize(ccp(m_fields->size.width, 12.f));
		notifyBar->setAnchorPoint(ccp(0.5, 0.5));
		notifyBar->ignoreAnchorPointForPosition(false);
		notifyWidget->addChild(notifyBar);

		auto notifyLabel = CCLabelBMFont::create("notify", "chatFont.fnt");
        notifyLabel->setAlignment(CCTextAlignment::kCCTextAlignmentCenter);
        notifyLabel->setScale(0.4);
		notifyLabel->setColor(warning ? ccRED : ccGREEN);		
        notifyWidget->addChild(notifyLabel);
		notifyLabel->setString(str);

		notifyWidget->runAction(CCSequence::create(
			CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(m_fields->size.width / 2, 6.f))),
			CCDelayTime::create(2.f),
			CCEaseExponentialOut::create(CCMoveTo::create(0.2, ccp(m_fields->size.width / 2, -6.f))),
			CallFuncExt::create([notifyWidget]() { notifyWidget->removeFromParentAndCleanup(true); }),
			nullptr
		));
	}

	bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
		return true;
	}

	void ccTouchMoved(CCTouch* touch, CCEvent* event) override { }

	void ccTouchEnded(CCTouch* touch, CCEvent* event) override { }

	// refresh status in menu, will NOT affect the preview frame
	// called when: dual switch, loaded from a slot
	void refreshValue() {
		auto pos = this->m_fields->config[m_fields->id]->m_position;
		m_fields->posMenu->setValue(m_fields->id == 2 || m_fields->id == 4 ? ccp(m_fields->size.width - pos.x, pos.y) : pos);
		m_fields->widthMenu->setValue(this->m_fields->config[m_fields->id]->m_width);
		m_fields->heightMenu->setValue(this->m_fields->config[m_fields->id]->m_height);
		m_fields->scaleMenu->setValue(this->m_fields->config[m_fields->id]->m_scale);
		m_fields->opacityMenu->setValue(float(this->m_fields->config[m_fields->id]->m_opacity) / 255);
		m_fields->deadzoneMenu->setValue(this->m_fields->config[m_fields->id]->m_deadzone);
		m_fields->radiusMenu->setValue(this->m_fields->config[m_fields->id]->m_radius);

		m_fields->modeLabel->setColor(this->m_fields->config[m_fields->id]->m_modeB ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
		m_fields->snapLabel->setColor(this->m_fields->config[m_fields->id]->m_snap ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
		m_fields->splitLabel->setColor(this->m_fields->config[m_fields->id]->m_split ? ccc3(128, 255, 128) : ccc3(255, 128, 128));
	}

	// switch dual mode
	void onDual(CCObject*) {
		if (this->m_fields->id > 2) {
			static_cast<TextInput*>(this->m_fields->deadzoneMenu->getChildByID("inputer"))->setEnabled(true);
			static_cast<TextInput*>(this->m_fields->radiusMenu->getChildByID("inputer"))->setEnabled(true);
			this->m_fields->deadzoneMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 1)));
			this->m_fields->radiusMenu->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 1)));
		}
		// switch and save
		this->m_fields->id = !this->m_fields->id;
		Mod::get()->setSavedValue("dual", bool(this->m_fields->id));
		// tint button
		this->m_fields->dualBtn->runAction(this->m_fields->id ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
		// title label
		this->m_nameLabel->setString(m_fields->id ? "P1 Move" : "Single Mode");
		// input slider bundles tag
		this->m_fields->widthMenu->setTag(-1);
		this->m_fields->heightMenu->setTag(-5);
		this->m_fields->scaleMenu->setTag(-9);
		this->m_fields->opacityMenu->setTag(-13);
		this->m_fields->deadzoneMenu->setTag(-21);
		this->m_fields->radiusMenu->setTag(-19);
		// togglers
		this->m_fields->modeBtn->setTag(-17);
		this->m_fields->snap0Btn->setTag(-24);
		this->m_fields->splitBtn->setTag(-27);
		// official options
		m_fields->jumplBtn->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, m_fields->id == 0)));
		m_fields->symmetryBtn->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, m_fields->id != 0)));
		// ui values
		this->refreshValue();
		// map
		this->m_fields->map->switchDual();
		this->m_fields->preview->switchDual();
	}
	
	// switch dual mode
	void onMode(CCObject*) {
		int t = -17 - (m_fields->id == 2);
		this->m_fields->config[m_fields->map->getCurrent()]->m_modeB = !this->getValue(t);
		this->valueDidChange(t, !this->getValue(t));
		this->m_fields->modeLabel->runAction(this->getValue(t) ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
		// dual symmetry
		if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
			auto mirror = 3 - m_fields->id;
			this->m_fields->config[mirror]->m_modeB = this->getValue(t);
			this->valueDidChange(-35 - t, this->getValue(t));
		}
	}

	// switch robtop's snap option
	void onSnap0(CCObject*) {
		int t = -24 - (m_fields->id == 2);
		this->valueDidChange(t, !this->getValue(t));
		this->m_fields->snapLabel->runAction(this->getValue(t) ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
		// dual symmetry
		if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
			auto mirror = 3 - m_fields->id;
			this->m_fields->config[mirror]->m_snap = this->getValue(t);
			this->valueDidChange(-49 - t, this->getValue(t));
		}
	}

	// switch split option
	void onSplit(CCObject*) {
		int t = -27 - (m_fields->id == 2);
		this->valueDidChange(t, !this->getValue(t));
		this->m_fields->splitLabel->runAction(this->getValue(t) ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
		// dual symmetry
		if (m_fields->id && Mod::get()->getSavedValue<bool>("symmetry")) {
			auto mirror = 3 - m_fields->id;
			this->m_fields->config[mirror]->m_split = this->getValue(t);
			this->valueDidChange(-55 - t, this->getValue(t));
		}
	}

	// switch jumpL option (single mode only)
	void onJumpL(CCObject*) {
		gm->setGameVariable("0113", !gm->getGameVariable("0113"));
		//this->valueDidChange(-26, !this->getValue(-26));
		this->m_fields->jumplLabel->runAction(gm->getGameVariable("0113") ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
	}
	
	// always symmetric dual ui status (dual mode only)
	void onSymmetry(CCObject*) {
		Mod::get()->setSavedValue("symmetry", !Mod::get()->getSavedValue<bool>("symmetry"));
		m_fields->symmetryLabel->runAction(Mod::get()->getSavedValue<bool>("symmetry") ? CCTintTo::create(0.4, 128, 255, 128) : CCTintTo::create(0.4, 255, 128, 128));
		// do something
	}
	
	// options
    void onOptions(CCObject*) {
		geode::openSettingsPopup(Mod::get(), false);
	}

	// preview
	void onPreview(CCObject*) {
		if (this->m_fields->slpage || this->m_fields->inprev)
			return;
		this->Transition(false, false);
	}

	// snap (my own grid snap)
	void onSnap(CCObject*) {
        bool on = Mod::get()->getSavedValue<bool>("snap");
        Mod::get()->setSavedValue("snap", !on);
		this->m_fields->map->setGridVisibility(!on);
		// tint color
		this->m_fields->snapBtn->runAction(CCTintTo::create(0.4, 128 + 127 * on, 255 - 127 * on, 128));
	}

	void onResetNew(CCObject*) {
		
		gm->m_dpad1 = UIButtonConfig{
			.m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
			.m_snap = false, .m_position = ccp(95.f, 36.f), .m_oneButton = false, .m_player2 = false, .m_split = false
		};
		gm->m_dpad2 = UIButtonConfig{
			.m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
			.m_snap = false, .m_position = ccp(95.f, 36.f), .m_oneButton = false, .m_player2 = false, .m_split = false
		};
		gm->m_dpad3 = UIButtonConfig{
			.m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
			.m_snap = false, .m_position = ccp(95.f, 36.f), .m_oneButton = false, .m_player2 = true, .m_split = false
		};
		gm->m_dpad4 = UIButtonConfig{
			.m_width = 200, .m_height = 200, .m_scale = 1.f, .m_opacity = 255, .m_position = ccp(95.f, 196.f), .m_oneButton = true, .m_player2 = false
		};
		gm->m_dpad5 = UIButtonConfig{
			.m_width = 200, .m_height = 200, .m_scale = 1.f, .m_opacity = 255, .m_position = ccp(95.f, 196.f), .m_oneButton = true, .m_player2 = true
		};

		this->refreshValue();
		// dual mode
		if (this->m_fields->id) {
			this->m_fields->map->updateState(1, gm->m_dpad2, 0.2);
			this->m_fields->map->updateState(2, gm->m_dpad3, 0.2);
			this->m_fields->map->updateState(3, gm->m_dpad4, 0.2);
			this->m_fields->map->updateState(4, gm->m_dpad5, 0.2);
		} else
			this->m_fields->map->updateState(0, gm->m_dpad1, 0.2);
	}

	void onBrowseSlot(CCObject*) {
		if (this->m_fields->slpage || this->m_fields->inprev)
			return;
		this->m_fields->slpage = -1;
		this->m_fields->titleLabel->setString("Browse Layouts");

		for (auto slot : this->m_fields->slots)
			slot->setDualStatus(this->m_fields->id);
		this->TransitionSlots(true, false);
	}

	void onClose(CCObject* obj) override {
		//log::debug("inprev = {} slpage = {}", this->m_fields->inprev, this->m_fields->slpage);
		if (this->m_fields->inprev) {
			if (this->m_fields->slpage)
				this->TransitionSlots(true, true);
			else 
				this->Transition(true, false);
		}
		else if (this->m_fields->slpage)
			this->TransitionSlots(this->m_fields->slpage > 0, this->m_fields->slpage > 0);
		else
			this->runAction(CCSequence::create(
				CallFuncExt::create([this] () { this->Transition(false, true); }),
				CCDelayTime::create(0.3),
				CallFuncExt::create([this, obj] () { this->SetupTriggerPopup::onClose(obj); }),
				nullptr
			));
	}

	void keyBackClicked() override {
		//log::debug("inprev = {} slpage = {}", this->m_fields->inprev, this->m_fields->slpage);
		if (this->m_fields->inprev) {
			if (this->m_fields->slpage)
				this->TransitionSlots(true, true);
			else 
				this->Transition(true, false);
		}
		else if (this->m_fields->slpage)
			this->TransitionSlots(this->m_fields->slpage > 0, this->m_fields->slpage > 0);
		else
			this->runAction(CCSequence::create(
				CallFuncExt::create([this] () { this->Transition(false, true); }),
				CCDelayTime::create(0.3),
				CallFuncExt::create([this] () { this->SetupTriggerPopup::keyBackClicked(); }),
				nullptr
			));
	}
};