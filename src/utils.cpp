#include "head.hpp"
#include <geode.devtools/include/API.hpp>
#include <regex>
#include <Geode/loader/Dispatch.hpp>

bool PosInputBundle::init() {
    if (!CCMenu::init())
        return false;

    this->setContentSize(ccp(0.f, 0.f));
    auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

    this->m_labelX = CCLabelBMFont::create("X = ", "chatFont.fnt");
    m_labelX->setPosition(ccp(-150.f, 0.f));
    m_labelX->setColor(color);
    this->addChild(m_labelX);

    this->m_inputX = TextInput::create(60, "X Pos");
    m_inputX->setFilter("-0123456789.");
    m_inputX->setPosition(ccp(-90.f, 0.f));
    m_inputX->getInputNode()->setTag(114);
    m_inputX->setDelegate(this);
    this->addChild(m_inputX);

    this->m_labelY = CCLabelBMFont::create("Y = ", "chatFont.fnt");
    m_labelY->setPosition(ccp(60.f, 0.f));
    m_labelY->setColor(color);
    this->addChild(m_labelY);

    this->m_inputY = TextInput::create(60, "Y Pos");
    m_inputY->setFilter("-0123456789.");
    m_inputY->setPosition(ccp(120.f, 0.f));
    m_inputY->getInputNode()->setTag(514);
    m_inputY->setDelegate(this);
    this->addChild(m_inputY);
    
    if (Mod::get()->getSettingValue<bool>("dont-crash"))
        return true;

    listenForSettingChangesV3("ui-color", [this] (ccColor3B color) {
        this->m_labelX->setColor(color);
        this->m_labelY->setColor(color);
    });

    return true;
}

void PosInputBundle::textChanged(CCTextInputNode* input) {
    std::string str = input->getString();
    Signal(input->getTag(), numFromString<float>(str).unwrapOrDefault()).post();
}


void PosInputBundle::setValue(const CCPoint &pos) {
    this->m_inputX->setString(numToString<float>(pos.x, 2));
    this->m_inputY->setString(numToString<float>(pos.y, 2));
}

bool InputSliderBundle::init(const char* title, float min, float max, int accu) {
    if (!CCMenu::init())
        return false;

    this->min = min;    
    this->max = max;
    this->accu = accu;
    // only opacity value does not accept overflow value
    this->force = std::string(title) == "Opacity";
    this->setContentSize(ccp(0.f, 0.f));

    this->m_label = CCLabelBMFont::create(fmt::format("{} = ", title).c_str(), "chatFont.fnt");
    m_label->setPosition(ccp(-100.f, 0.f));
    m_label->setColor(Mod::get()->getSettingValue<ccColor3B>("ui-color"));
    this->addChild(m_label);

    this->m_input = TextInput::create(60, "0 ~ 1");
    m_input->setFilter("0123456789.");
    m_input->setPosition(ccp(-30.f, 0.f));
    m_input->setDelegate(this);
    m_input->setID("inputer");
    m_input->setScale(0.9);
    this->addChild(m_input);

    this->m_slider = Slider::create(this, menu_selector(InputSliderBundle::onSlider));
    m_slider->setPosition(ccp(80.f, 0.f));
    m_slider->setContentSize(ccp(0.f, 0.f));
    m_slider->setScale(0.6f);
    this->addChild(m_slider);

    if (Mod::get()->getSettingValue<bool>("dont-crash"))
        return true;

    listenForSettingChangesV3("ui-color", [this] (ccColor3B color) {
        this->m_label->setColor(color);
    });

    return true;
}

void InputSliderBundle::textChanged(CCTextInputNode* input) {
    std::string str = input->getString();
    float val = numFromString<float>(str).unwrapOrDefault();
    val = (val > this->max) && this->force ? this->max : (val < this->min ? this->min : val);
    this->m_slider->setValue((val - this->min) / (this->max - this->min));
    Signal(this->getTag(), val).post();
}

void InputSliderBundle::onSlider(CCObject* sender) {
    float val = this->min + (this->max - this->min) * this->m_slider->getValue();
    std::string str;
    if (accu)
        str = numToString<float>(val, accu);
    else
        str = numToString<int>(val);
    this->m_input->setString(str.c_str());
    Signal(this->getTag(), val).post();
}

void InputSliderBundle::registerDevTools() {
    devtools::registerNode<InputSliderBundle>([](InputSliderBundle* node) {
        devtools::label("InputSliderBundle");
        devtools::property("min", node->min);        
        devtools::property("max", node->max);
        devtools::property("precision", node->accu);
        devtools::property("force ranged", node->force);
    });
}

void InputSliderBundle::setValue(float val) {
    std::string str = numToString<float>(val, accu);
    this->m_input->setString(str.c_str());
    this->m_slider->setValue((val - this->min) / (this->max - this->min));
}

bool PreviewFrame::init() {
    if (!CCLayer::init())
        return false;

    auto size = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(size);
    // center
    auto center = size / 2; 
    this->setAnchorPoint(ccp(0.5, 0.5));

    // border
    this->m_border = CCSprite::create();
    m_border->setPosition(center);
    //m_border->setC(center);
    m_border->setID("border");
    //m_border->setOpacity(0);
    //m_border->setCascadeOpacityEnabled(true);
    this->addChild(m_border);

    // grid
    this->m_grid = CCSprite::create();
    m_grid->setPosition(center);
    m_grid->setID("grid");
    //m_grid->setOpacity(0);
    //m_grid->setCascadeOpacityEnabled(true);
    this->addChild(m_grid);

    auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

    float tripleX[3] = {-center.width, 0, center.width};
    for (float x : tripleX) {
        auto v = CCLayerColor::create(ccc4(255, 255, 255, 0));
        v->setPosition(ccp(x, 0));
        v->setContentSize(ccp(1.2, size.height));
        v->ignoreAnchorPointForPosition(false);
        v->setColor(color);
        //v->setCascadeOpacityEnabled(true);
        if (x)
            m_border->addChild(v);
        else {
            v->setTag(1919);
            m_grid->addChild(v);
            this->vert.push_back(v);            
        }
    }

    float tripleY[3] = {-center.height, 0, center.height};
    for (float y : tripleY) {
        auto h = CCLayerColor::create(ccc4(255, 255, 255, 0));
        h->setPosition(ccp(0, y));
        h->setContentSize(ccp(size.width, 1.2));
        h->ignoreAnchorPointForPosition(false);
        h->setColor(color);
        //h->setCascadeOpacityEnabled(true);
        if (y)
            m_border->addChild(h);
        else {
            h->setTag(810);
            m_grid->addChild(h);
            this->hori.push_back(h);
        }
    }

    this->reGridX(false);
    this->reGridY(false);

    this->setTouchEnabled(true);
    this->setTouchMode(ccTouchesMode::kCCTouchesOneByOne);

    if (Mod::get()->getSettingValue<bool>("dont-crash"))
        return true;

    listenForSettingChangesV3("hori-distance", [this] (int) {
        this->vert.clear();
        this->vert.push_back(static_cast<CCLayerColor*>(this->m_grid->getChildByTag(1919)));
        int i = 0;
        while (i < m_grid->getChildrenCount()) {
            auto child = m_grid->getChildByIndex(i);
            if (child->getTag() == 114)
                child->removeFromParentAndCleanup(true);
            else
                i ++;
        }
        /*
        for (auto child : CCArrayExt<CCNode*>(m_grid->getChildren()))
            if (child->getTag() == 114)
                child->removeFromParentAndCleanup(true);
        */
        this->reGridX(true);
    });

    listenForSettingChangesV3("vert-distance", [this] (int) {
        this->hori.clear();
        this->hori.push_back(static_cast<CCLayerColor*>(this->m_grid->getChildByTag(810)));
        int j = 0;
        while (j < m_grid->getChildrenCount()) {
            auto child = m_grid->getChildByIndex(j);
            if (child->getTag() == 514)
                child->removeFromParentAndCleanup(true);
            else
                j ++;
        }
        /*
        for (auto child : CCArrayExt<CCNode*>(m_grid->getChildren()))
            if (child->getTag() == 514)
                child->removeFromParentAndCleanup(true);
        */
        this->reGridY(true);
    });

    listenForSettingChangesV3("ui-color", [this] (ccColor3B color) {
        for (auto child : CCArrayExt<CCLayerColor*>(m_border->getChildren()))
            child->setColor(color);
        for (auto child : CCArrayExt<CCLayerColor*>(m_grid->getChildren()))
            child->setColor(color);
    });

    return true;
}

void PreviewFrame::reGridX(bool visible) {
    auto size = CCDirector::sharedDirector()->getWinSize();
    // center
    auto center = size / 2; 

    int hd = Mod::get()->getSettingValue<int64_t>("hori-distance");
    auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

    float x = 0;
    while (center.width - x > hd) {
        x += hd;
        auto l = CCLayerColor::create(ccc4(255, 255, 255, visible * 255));
        l->setPosition(ccp(-x, 0));
        l->setContentSize(ccp(0.6, size.height));
        l->ignoreAnchorPointForPosition(false);
        l->setColor(color);
        l->setTag(114);
        m_grid->addChild(l);
        this->vert.push_back(l);

        auto r = CCLayerColor::create(ccc4(255, 255, 255, visible * 255));
        r->setPosition(ccp(+x, 0));
        r->setContentSize(ccp(0.6, size.height));
        r->ignoreAnchorPointForPosition(false);
        r->setColor(color);
        r->setTag(114);
        m_grid->addChild(r);
        this->vert.push_back(r);
    }
}

void PreviewFrame::reGridY(bool visible) {
    auto size = CCDirector::sharedDirector()->getWinSize();
    // center
    auto center = size / 2; 

    int vd = Mod::get()->getSettingValue<int64_t>("vert-distance");
    auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

    float y = 0;
    while (center.height - y > vd) {
        y += vd;

        auto t = CCLayerColor::create(ccc4(255, 255, 255, visible * 255));
        t->setPosition(ccp(0, -y));
        t->setContentSize(ccp(size.width, 0.6));
        t->ignoreAnchorPointForPosition(false);
        t->setColor(color);
        t->setTag(514);
        m_grid->addChild(t);
        this->hori.push_back(t);

        auto b = CCLayerColor::create(ccc4(255, 255, 255, visible * 255));
        b->setPosition(ccp(0, +y));
        b->setContentSize(ccp(size.width, 0.6));
        b->ignoreAnchorPointForPosition(false);
        b->setColor(color);
        b->setTag(514);
        m_grid->addChild(b);
        this->hori.push_back(b);
    }    
}

void PreviewFrame::helpTransition(bool in) {
    for (auto b : CCArrayExt<CCLayerColor*>(m_border->getChildren()))
        b->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 255 * in)));

    if (Mod::get()->getSavedValue<bool>("snap"))
        this->setGridVisibility(in);
}

void PreviewFrame::setGridVisibility(bool visible) {
    for (auto v : vert)
        v->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, visible * (240 - 96 * bool(v->getPositionX())))));   
    for (auto h : hori)
        h->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, visible * (240 - 96 * bool(h->getPositionY())))));   
}

bool PracticePreviewFrame::init() {
    if (!PreviewFrame::init())
        return false;

    // target node
    // the practice button (sprite here)
    this->m_sprL = CCSprite::createWithSpriteFrameName("GJ_checkpointBtn_001.png");
    m_sprL->setPositionX(-28.25f);
    m_sprL->setOpacity(gm->m_practiceOpacity);

    this->m_sprR = CCSprite::createWithSpriteFrameName("GJ_removeCheckBtn_001.png");
    m_sprR->setPositionX(41.50f);
    m_sprR->setOpacity(gm->m_practiceOpacity);

    this->m_target = CCSprite::create();
    m_target->setPosition(gm->m_practicePos);
    m_target->setContentSize(ccp(0.f, 0.f));
    m_target->setID("target");
    m_target->addChild(this->m_sprL);
    m_target->addChild(this->m_sprR);
    this->addChild(m_target);

    return true;
}

bool PracticePreviewFrame::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    auto p = this->m_target->getPosition();
    CCRect rect = {p.x - 70.f, p.y - 28.f, 140.f, 56.f};
    bool ret = rect.containsPoint(this->convertTouchToNodeSpace(touch));
    Signal(-100, ret).post();
    return ret;
}

void PracticePreviewFrame::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    //auto delta = touch->m_point - touch->m_startPoint;
    auto delta = touch->getDelta();
    //log::debug("x = {}, y = {}", delta.x, delta.y);
    auto dest = this->m_target->getPosition() + delta / this->getScale();
    this->m_target->setPosition(dest);
    //this->m_target->setPosition(this->convertTouchToNodeSpace(touch));
    PosSignal(0, dest).post();
}

void PracticePreviewFrame::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    if (!Mod::get()->getSavedValue<bool>("snap"))
        return;
    auto t = this->convertTouchToNodeSpace(touch);
    auto c = this->getContentSize() / 2;

    int hd = Mod::get()->getSettingValue<int64_t>("hori-distance");
    int vd = Mod::get()->getSettingValue<int64_t>("vert-distance");

    auto dest = ccp(c.width + hd * round((t.x - c.width) / hd), c.height + vd * round((t.y - c.height) / vd));
    this->m_target->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, dest)));
    PosSignal(0, dest).post();
}

void PracticePreviewFrame::helpTransition(bool in) {
    this->alphaNode(in * 255 * gm->m_practiceOpacity, 0.3);
    PreviewFrame::helpTransition(in);
}

void PracticePreviewFrame::placeNode(const CCPoint& pos) {
    this->m_target->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, pos)));
}

void PracticePreviewFrame::alphaNode(GLubyte opacity, float d) {
    for (auto child : CCArrayExt<CCSprite*>(m_target->getChildren()))
        child->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
}

bool PlatformPreviewFrame::init(bool preview) {
    if (!PreviewFrame::init())
        return false;

    this->current = Mod::get()->getSavedValue<bool>("dual");
    this->preview= preview;
    // target nodes
    this->p1mNode = GJUINode::create(current ? gm->m_dpad2 : gm->m_dpad1);
    p1mNode->setTag(current);
    this->alphaNode(p1mNode, 0);
    this->addChild(p1mNode);

    this->p2mNode = GJUINode::create(gm->m_dpad3);
    p2mNode->setTag(2);
    this->alphaNode(p2mNode, 0);
    this->addChild(p2mNode);

    this->p1jNode = GJUINode::create(gm->m_dpad4);
    p1jNode->setTag(3);
    this->alphaNode(p1jNode, 0);
    this->addChild(p1jNode);

    this->p2jNode = GJUINode::create(gm->m_dpad5);
    p2jNode->setTag(4);
    this->alphaNode(p2jNode, 0);
    this->addChild(p2jNode);

    return true;
}

bool PlatformPreviewFrame::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!this->activate)
        return false;

    auto t = this->convertTouchToNodeSpace(touch);
    
    if (this->preview) {
        auto s = this->getContentSize() / 2;
        if (t.x < s.width - 110 || t.x > s.width + 110 || t.y > s.height - 232)
            Signal(-100, -514).post();
        return false;
    }

    // not preview
    // single mode
    if (!this->current) {
        auto r = p1mNode->m_rect;
        //log::debug("rect = {} {}", r.origin.x, r.origin.y);
        r.origin = p1mNode->getPosition() - r.size / 2;
        bool ret = r.containsPoint(t);
        
        Signal(-100, ret ? 0 : -114).post();
        return ret;
    }
    // dual mode
    auto r = this->getTargetNode(current)->m_rect;
    r.origin = this->getTargetNode(current)->getPosition() - r.size / 2;
    if (r.containsPoint(t)) {
        Signal(-100, current).post();
        return true;
    } else for (auto tag = 1; tag < 5; tag ++) {
        if (tag == this->current)
            continue;

        auto r = this->getTargetNode(tag)->m_rect;
        r.origin = this->getTargetNode(tag)->getPosition() - r.size / 2;
        if (r.containsPoint(t)) {
            this->current = tag;
            Signal(-100, tag).post();
            return true;
        }
    }
    if (this->boundingBox().containsPoint(t))
        Signal(-100, -114).post();
    return false;
}

void PlatformPreviewFrame::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    auto delta = touch->getDelta();
    auto dest = this->getChildByTag(current)->getPosition() + delta / this->getScale();
    this->getChildByTag(current)->setPosition(dest);
    PosSignal(false, dest).post();
}

void PlatformPreviewFrame::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    auto delta = touch->getDelta();
    auto dest = this->getChildByTag(current)->getPosition() + delta / this->getScale();
    if (!Mod::get()->getSavedValue<bool>("snap")) {
        this->getChildByTag(current)->setPosition(dest);
        PosSignal(true, dest).post();
        return;
    }
    auto c = this->getContentSize() / 2;

    int hd = Mod::get()->getSettingValue<int64_t>("hori-distance");
    int vd = Mod::get()->getSettingValue<int64_t>("vert-distance");
    
    dest = ccp(c.width + hd * round((dest.x - c.width) / hd), c.height + vd * round((dest.y - c.height) / vd));
    this->getChildByTag(current)->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, dest)));
    PosSignal(true, dest).post();
}

void PlatformPreviewFrame::registerDevTools() {
    devtools::registerNode<PlatformPreviewFrame>([](PlatformPreviewFrame* node) {
        devtools::label("PlatformPreviewFrame");
        devtools::property("index", node->current);        
        devtools::property("preview", node->preview);
        devtools::property("activate", node->activate);
    });
}

int PlatformPreviewFrame::getCurrent() {
    return this->current;
}

void PlatformPreviewFrame::switchDual() {
    this->current = !this->current;
    this->p1mNode->setTag(this->current);

    this->updateState(this->current, this->current ? gm->m_dpad2 : gm->m_dpad1, 0.2 * (!this->preview));
    if (this->current) {
        this->updateState(2, gm->m_dpad3, 0.2 * (!this->preview));
        this->updateState(3, gm->m_dpad4, 0.2 * (!this->preview));
        this->updateState(4, gm->m_dpad5, 0.2 * (!this->preview));
    }

    this->alphaNode(this->p2mNode, this->current * gm->m_dpad3.m_opacity, 0.2 * (!this->preview));
    this->alphaNode(this->p1jNode, this->current * gm->m_dpad4.m_opacity, 0.2 * (!this->preview));
    this->alphaNode(this->p2jNode, this->current * gm->m_dpad5.m_opacity, 0.2 * (!this->preview));

    this->p2mNode->m_drawLines = current;
    this->p1jNode->m_drawLines = current;
    this->p2jNode->m_drawLines = current;
}

GJUINode* PlatformPreviewFrame::getTargetNode(int tag) {
    return static_cast<GJUINode*>(this->getChildByTag(tag));
}

void PlatformPreviewFrame::helpTransition(bool in) {
    this->activate = in;
    if (this->current) {
        this->alphaNode(1, in * gm->m_dpad2.m_opacity, 0.3);
        this->alphaNode(2, in * gm->m_dpad3.m_opacity, 0.3);
        this->alphaNode(3, in * gm->m_dpad4.m_opacity, 0.3);
        this->alphaNode(4, in * gm->m_dpad5.m_opacity, 0.3);
    }
    else
        this->alphaNode(0, in * gm->m_dpad1.m_opacity, 0.3);

    this->p1mNode->m_drawLines = in;
    this->p2mNode->m_drawLines = in && this->current;
    this->p1jNode->m_drawLines = in && this->current;
    this->p2jNode->m_drawLines = in && this->current;

    PreviewFrame::helpTransition(in);
}

// update state
void PlatformPreviewFrame::updateState(int tag, const UIButtonConfig &config, float d) {
    //log::debug("upstate tag = {}", tag);
    this->updateState(static_cast<GJUINode*>(this->getChildByTag(tag)), config, d);
}

void PlatformPreviewFrame::updateState(GJUINode* node, const UIButtonConfig &config, float d) {

    node->m_rect.size = ccp(config.m_width, config.m_height);
    node->m_rect.origin = ccp(-config.m_width / 2.f, -config.m_height / 2.f);
    node->m_deadzone = config.m_deadzone;
    node->m_modeB = config.m_modeB;
    node->m_snap = config.m_snap;
    node->m_split = config.m_split;
    
    node->m_firstSprite->setVisible(!config.m_modeB);
    if (!node->m_oneButton)
        node->m_secondSprite->setVisible(!config.m_modeB);

    node->updateButtonFrames();

    this->placeNode(node, config.m_player2 ? ccp(this->getContentWidth() - config.m_position.x, config.m_position.y) : config.m_position, d);
    this->scaleNode(node, config.m_scale, d);
    this->alphaNode(node, config.m_opacity, d);
    this->radiusNode(node, config.m_radius, d);
}

// set position of target node
void PlatformPreviewFrame::placeNode(int tag, const CCPoint& pos, float d) {
    this->placeNode(static_cast<GJUINode*>(this->getChildByTag(tag)), pos, d);
}

void PlatformPreviewFrame::placeNode(GJUINode* node, const CCPoint& pos, float d) {
    node->runAction(CCEaseExponentialOut::create(CCMoveTo::create(d, pos)));
}


void PlatformPreviewFrame::alphaNode(int tag, GLubyte opacity, float d) {
    //log::debug("alpha node, tag = {}", tag);
    this->alphaNode(static_cast<GJUINode*>(this->getChildByTag(tag)), opacity, d);
}

void PlatformPreviewFrame::alphaNode(GJUINode* node, GLubyte opacity, float d) {
    node->m_firstSprite->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
    if (!node->m_oneButton)
        node->m_secondSprite->runAction(CCEaseExponentialOut::create(CCFadeTo::create(d, opacity)));
}

void PlatformPreviewFrame::scaleNode(int tag, float scale, float d) {
    this->scaleNode(static_cast<GJUINode*>(this->getChildByTag(tag)), scale, d);
}

void PlatformPreviewFrame::scaleNode(GJUINode* node, float scale, float d) {
    node->m_firstSprite->runAction(CCEaseExponentialOut::create(CCScaleTo::create(d, scale)));
    if (!node->m_oneButton)
        node->m_secondSprite->runAction(CCEaseExponentialOut::create(CCScaleTo::create(d, scale)));
}

void PlatformPreviewFrame::radiusNode(int tag, float r, float d) {
    this->radiusNode(static_cast<GJUINode*>(this->getChildByTag(tag)), r, d);
}

void PlatformPreviewFrame::radiusNode(GJUINode* node, float r, float d) {
    if (node->m_oneButton)
        return;
    node->m_radius = r;
    if (node->m_modeB) {
        node->updateButtonFrames();
        return;
    }
    float base = node->m_firstSprite->getContentWidth() / 2;
    node->m_firstSprite->runAction(CCEaseExponentialOut::create(CCMoveTo::create(d, ccp(-base - r, 0))));
    node->m_secondSprite->runAction(CCEaseExponentialOut::create(CCMoveTo::create(d, ccp(base + r, 0))));
}

bool SlotFrame::init(int nametag) {
    if (!CCMenu::init())
        return false;

    this->setContentSize(ccp(360.f, 70.f));
    this->setAnchorPoint(ccp(0.5, 0.5));
    this->ignoreAnchorPointForPosition(false);
    this->setScale(0);
    this->setTag(nametag);

    this->bg = CCLayerColor::create(Mod::get()->getSettingValue<ccColor4B>("slot-color"));
    //this->bg->setOpacity(0);
    this->bg->setContentSize(ccp(360.f, 70.f));
    this->bg->setAnchorPoint(ccp(0.5, 0.5));
    this->bg->setID("background");
    this->addChildAtPosition(this->bg, Anchor::BottomLeft);

    this->titleLabel = CCLabelBMFont::create("title", "bigFont.fnt");
    this->titleLabel->setPosition(ccp(10.f, 55.f));
    this->titleLabel->setAnchorPoint(ccp(0.f, 0.5));
    this->titleLabel->setScale(0.5);
    this->titleLabel->setID("title");
    this->addChild(this->titleLabel);

    auto color = Mod::get()->getSettingValue<ccColor3B>("ui-color");

    this->descLabel = CCLabelBMFont::create("desc", "chatFont.fnt", 1000.f);
    this->descLabel->setPosition(ccp(10.f, 40.f));
    this->descLabel->setAnchorPoint(ccp(0.f, 1));
    this->descLabel->setScale(0.55);
    this->descLabel->setColor(color);
    this->descLabel->setID("desc");
    this->addChild(this->descLabel);

    auto prevSpr = CCSprite::create("fullscreenBtn.png"_spr);
    prevSpr->setScale(0.4);
    this->prevBtn = CCMenuItemSpriteExtra::create(prevSpr, this, menu_selector(SlotFrame::onPreview));
    this->prevBtn->setPosition(ccp(290.f, 10.f));
    this->prevBtn->setColor(color);
    this->addChild(this->prevBtn);

    auto saveSpr = CCSprite::create("slotBtn.png"_spr);
    saveSpr->setScale(0.4);
    this->saveBtn = CCMenuItemSpriteExtra::create(saveSpr, this, menu_selector(SlotFrame::onSave));
    this->saveBtn->setPosition(ccp(315.f, 10.f));
    this->saveBtn->setColor(color);
    this->addChild(this->saveBtn);

    auto loadSpr = CCSprite::create("applyBtn.png"_spr);
    loadSpr->setScale(0.4);
    this->loadBtn = CCMenuItemSpriteExtra::create(loadSpr, this, menu_selector(SlotFrame::onApply));
    this->loadBtn->setPosition(ccp(340.f, 10.f));
    this->loadBtn->setColor(color);
    this->addChild(this->loadBtn);

    if (Mod::get()->getSettingValue<bool>("dont-crash"))
        return true;

	listenForSettingChangesV3("slot-color", [this] (ccColor4B val) {
        this->bg->setColor(to3B(val)); this->bg->setOpacity(val.a); });

    listenForSettingChangesV3("ui-color", [this] (ccColor3B color) {
        this->descLabel->setColor(color);
        this->prevBtn->setColor(color);
        this->saveBtn->setColor(color);
        this->loadBtn->setColor(color);
    });

    return true;
}

void SlotFrame::parseSingleBtn(UIButtonConfig &config, std::string raw) {
    if (raw == "") {
        this->real = false;
        return;
    }
    // regex
    std::regex pt(config.m_oneButton ? R"w(^(\d+),(\d+),(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,(-?\d+.?\d{0,2})\d*,(-?\d+.?\d{0,2})\d*$)w"
        : R"w(^(\d+),(\d+),(\d+.?\d{0,2})\d*,(\d+.?\d{0,2})\d*,(-?\d+.?\d{0,2})\d*,(-?\d+.?\d{0,2})\d*,([01]),(\d+.?\d{0,1})\d*,(\d+.?\d{0,1})\d*,([01]),([01])$)w");
    std::smatch match;

    this->real = std::regex_match(raw, match, pt);
    //log::debug("parse single raw str = {} ret = {}", raw, this->real);
    if (!this->real)
        return;
    // data fill to be fixed
    config.m_width = numFromString<int>(match.str(1)).unwrapOrDefault();
    config.m_height = numFromString<int>(match.str(2)).unwrapOrDefault();
    config.m_scale = numFromString<float>(match.str(3)).unwrapOrDefault();
    config.m_opacity = 255 * (numFromString<float>(match.str(4)).unwrapOrDefault() > 1 ? 1 : numFromString<float>(match.str(4)).unwrapOrDefault());
    config.m_position = ccp(numFromString<float>(match.str(5)).unwrapOrDefault(), numFromString<float>(match.str(6)).unwrapOrDefault());
    if (!config.m_oneButton) {
        config.m_modeB = numFromString<int>(match.str(7)).unwrapOrDefault();
        config.m_deadzone = numFromString<float>(match.str(8)).unwrapOrDefault();
        config.m_radius = numFromString<float>(match.str(9)).unwrapOrDefault();
        config.m_snap = numFromString<int>(match.str(10)).unwrapOrDefault();
        config.m_split = numFromString<int>(match.str(11)).unwrapOrDefault();
    }
}

std::string SlotFrame::dumpSingleBtn(UIButtonConfig const &config) {
    return fmt::format("{},{},{},{},{},{}",
        config.m_width, config.m_height, config.m_scale, config.m_opacity / 255, config.m_position.x, config.m_position.y)
        + (config.m_oneButton ? "" : fmt::format(",{},{},{},{},{}",
        int(config.m_modeB), config.m_deadzone, config.m_radius, int(config.m_snap), int(config.m_snap)));
}

void SlotFrame::parse() {
    if (*this->raw == "") {
        this->real = false;
        return;
    }
    std::regex pt(this->dual ? R"w(([-\d.,]+);([-\d.,]+);([-\d.,]+);([-\d.,]+))w" : R"w(([-\d.,]+);([01]))w");
    std::smatch match;
    // match
    std::string str = fmt::format("{}", *this->raw);
    this->real = std::regex_match(str, match, pt);
    //log::debug("parse full raw str = {} ret = {}", *raw, this->real);
    if (!this->real)
        return;
    // parse
    this->parseSingleBtn(this->p1m, match.str(1));
    if (this->dual) {
        this->parseSingleBtn(this->p1m, match.str(1));
        this->parseSingleBtn(this->p2m, match.str(2));
        this->parseSingleBtn(this->p1j, match.str(3));
    } else
        this->jumpL = numFromString<int>(match.str(2)).unwrapOrDefault();
}

void SlotFrame::refreshDescLabel() {
    auto pos = this->descLabel->getPosition();
    auto scale = this->descLabel->getScale();
    this->descLabel->removeFromParentAndCleanup(true);

    auto str = this->dual ? fmt::format(
        "P1M p={},{} w={} h={} s={} o={} m={} dz={} r={} sn={} sp={}\n"
        "P2M p={},{} w={} h={} s={} o={} m={} dz={} r={} sn={} sp={}\n"
        "P1J p={},{} w={} h={} s={} o={}\n"
        "P1J p={},{} w={} h={} s={} o={}",
        numToString<float>(p1m.m_position.x, 2), numToString<float>(p1m.m_position.y, 2), p1m.m_width, p1m.m_height, p1m.m_scale, p1m.m_opacity,
        p1m.m_modeB, p1m.m_deadzone, p1m.m_radius, p1m.m_snap, p1m.m_split,
        numToString<float>(p2m.m_position.x, 2), numToString<float>(p2m.m_position.y, 2), p2m.m_width, p2m.m_height, p2m.m_scale, p2m.m_opacity,
        p2m.m_modeB, p2m.m_deadzone, p2m.m_radius, p2m.m_snap, p2m.m_split,
        numToString<float>(p1j.m_position.x, 2), numToString<float>(p1j.m_position.y, 2), p1j.m_width, p1j.m_height, p1j.m_scale, p1j.m_opacity,
        numToString<float>(p2j.m_position.x, 2), numToString<float>(p2j.m_position.y, 2), p2j.m_width, p2j.m_height, p2j.m_scale, p2j.m_opacity
    ) : fmt::format("p={},{} w={} h={} s={} o={} m={} dz={} r={} sn={} sp={}\njL={}",
        numToString<float>(p1m.m_position.x, 2), numToString<float>(p1m.m_position.y, 2), p1m.m_width, p1m.m_height, p1m.m_scale, p1m.m_opacity,
        p1m.m_modeB, p1m.m_deadzone, p1m.m_radius, p1m.m_snap, p1m.m_split, this->jumpL
    );
    this->descLabel = CCLabelBMFont::create(str.c_str(), "chatFont.fnt");
    this->descLabel->setPosition(pos);
    this->descLabel->setAnchorPoint(ccp(0.f, 1));
    this->descLabel->setScale(scale);
    this->descLabel->setID("desc");
    this->addChild(this->descLabel);
}

void SlotFrame::registerDevTools() {
    devtools::registerNode<SlotFrame>([](SlotFrame* node) {
        devtools::label("SlotFrame");
        devtools::property("dual", node->dual);        
        devtools::property("real", node->real);
        devtools::property("showing", node->showing);
    });
}

void SlotFrame::setDualStatus(bool dual) {
    this->dual = dual;

    this->p1m = UIButtonConfig{
        .m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
        .m_snap = false, .m_position = ccp(95.f, 36.f), .m_oneButton = false, .m_player2 = false, .m_split = false
    };
    if (this->dual) {
        this->p2m = UIButtonConfig{
            .m_width = 280, .m_height = 120, .m_deadzone = 0.f, .m_scale = 1.f, .m_opacity = 255, .m_radius = 10.f, .m_modeB = false,
            .m_snap = false, .m_position = ccp(95.f, 36.f), .m_oneButton = false, .m_player2 = true, .m_split = false
        };
        this->p1j = UIButtonConfig{
            .m_width = 200, .m_height = 200, .m_scale = 1.f, .m_opacity = 255, .m_position = ccp(95.f, 196.f), .m_oneButton = true, .m_player2 = false
        };
        this->p2j = UIButtonConfig{
            .m_width = 200, .m_height = 200, .m_scale = 1.f, .m_opacity = 255, .m_position = ccp(95.f, 196.f), .m_oneButton = true, .m_player2 = true
        };
    } else
        this->jumpL = false;

    // refresh raw string pointer
    gd::string* s[3] = {
        this->dual ? &gm->m_dpadLayoutDual1 : &gm->m_dpadLayout1,
        this->dual ? &gm->m_dpadLayoutDual2 : &gm->m_dpadLayout2,
        this->dual ? &gm->m_dpadLayoutDual3 : &gm->m_dpadLayout3
    };
    this->raw = s[this->getTag() - 1];
    this->parse();

    this->titleLabel->setString(fmt::format("Dpad Layout {} #{}", this->dual ? "Dual" : "", this->getTag()).c_str());

    this->descLabel->setString("");
    if (!this->real) {
        this->descLabel->setString("<NA>");
        return;
    }
    this->refreshDescLabel();
}

void SlotFrame::onPreview(CCObject*) {
    if (this->showing) {
        Signal(1919, -1).post();
        return;
    }

    // post signal firstly
    Signal(1919, this->real * this->getTag()).post();
    if (!this->real)
        return;
    
    this->showing = true;    
    // background
    this->bg->stopAllActions();
    this->bg->runAction(CCEaseExponentialOut::create(CCMoveTo::create(
        0.2, ccp(0.f, -this->getPositionY()))));
    this->bg->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, size.width / 360.f, size.height / 70.f)));
    this->bg->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, 0)));
    
    // title label
    this->titleLabel->stopAllActions();
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(
        0.2, ccp(180.f - 0.3 * this->titleLabel->getContentWidth(), 170.f - this->getPositionY()))));
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));

    // description text
    this->descLabel->stopAllActions();
    this->descLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(
        0.2, ccp(180.f - 0.35 * this->descLabel->getContentWidth(), -20.f - this->getPositionY()))));
    this->descLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.7)));

    this->prevBtn->stopAllActions();
    this->prevBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(140.f, -100.f - this->getPositionY()))));
    this->prevBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));

    this->saveBtn->stopAllActions();
    this->saveBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(180.f, -100.f - this->getPositionY()))));
    this->saveBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));

    this->loadBtn->stopAllActions();
    this->loadBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(220.f, -100.f - this->getPositionY()))));
    this->loadBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.6)));
}

void SlotFrame::resume() {
    if (!this->showing)
        return;
    this->showing = false;

    Signal(-100, 0).post();
    // title label
    this->titleLabel->stopAllActions();
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(10.f, 55.f))));
    this->titleLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5)));

    this->bg->stopAllActions();
    this->bg->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(0.f, 0.f))));
    this->bg->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 1)));
    this->bg->runAction(CCEaseExponentialOut::create(CCFadeTo::create(0.4, Mod::get()->getSettingValue<ccColor4B>("slot-color").a)));

    // description text
    this->descLabel->stopAllActions();
    this->descLabel->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(10.f, 40.f))));
    this->descLabel->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.5)));

    this->prevBtn->stopAllActions();
    this->prevBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(290.f, 10.f))));
    this->prevBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.4)));

    this->saveBtn->stopAllActions();
    this->saveBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(315.f, 10.f))));
    this->saveBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.4)));

    this->loadBtn->stopAllActions();
    this->loadBtn->runAction(CCEaseExponentialOut::create(CCMoveTo::create(0.4, ccp(340.f, 10.f))));
    this->loadBtn->getChildByType<CCSprite>(0)->runAction(CCEaseExponentialOut::create(CCScaleTo::create(0.4, 0.4)));
}

void SlotFrame::onSave(CCObject*) {
    this->real = true;

    if (this->dual) {
        this->p1m = gm->m_dpad2;
        this->p2m = gm->m_dpad3;
        this->p1j = gm->m_dpad4;
        this->p2j = gm->m_dpad5;
        *this->raw = fmt::format("{};{};{};{}",
            dumpSingleBtn(this->p1m), dumpSingleBtn(this->p2m),
            dumpSingleBtn(this->p1j), dumpSingleBtn(this->p2j)
        );
    } else {
        this->p1m = gm->m_dpad1;
        this->jumpL = gm->getGameVariable("0113");
        *this->raw = fmt::format("{};{}", dumpSingleBtn(this->p1m), int(this->jumpL));    
    }

    this->refreshDescLabel();
    Signal(94, this->getTag() + this->showing * 3).post();
}

void SlotFrame::onApply(CCObject*) {
    if (this->dual) {
        gm->m_dpad2 = this->p1m;
        gm->m_dpad3 = this->p2m;
        gm->m_dpad4 = this->p1j;
        gm->m_dpad5 = this->p2j;        
    } else {
        gm->m_dpad1 = this->p1m;
        gm->setGameVariable("0113", this->jumpL);
    }
    // tell main page to refresh
    Signal(810, this->real * this->getTag()).post();
}

$execute {
    // not ported yet, and ever loaded
    if (!Mod::get()->setSavedValue("ported", true) && Mod::get()->getSavedSettingsData().contains("bgopacity"))
        Mod::get()->setSettingValue<int64_t>("bg-opacity", Mod::get()->getSavedSettingsData()["bgopacity"].asInt().unwrapOr(217) * 100 / 255);

    new EventListener(+[](CCPoint pos) {
        //this->m_fields->pcpposMenu->setValue(pos);
        PosSignal(1, pos).post();
        return ListenerResult::Stop;
    }, DispatchFilter<CCPoint>(Mod::get()->getID()));
}

$on_mod(Loaded) {
    // makes sure DevTools is loaded before registering
    devtools::waitForDevTools([] {
        InputSliderBundle::registerDevTools();
        PlatformPreviewFrame::registerDevTools();
        SlotFrame::registerDevTools();
    });    
}