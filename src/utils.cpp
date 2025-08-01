#include "head.hpp"

bool PosInputBundle::init() {
    if (!CCMenu::init())
        return false;

    this->setContentSize(ccp(400.f, 20.f));

    auto labelX = CCLabelBMFont::create("X = ", "chatFont.fnt");
    labelX->setPosition(ccp(-150.f, 10.f));
    this->addChild(labelX);

    this->m_inputX = TextInput::create(60, "X Pos");
    m_inputX->setFilter("-0123456789");
    m_inputX->setPosition(ccp(-90.f, 10.f));
    m_inputX->setTag(114);
    this->addChild(m_inputX);

    auto labelY = CCLabelBMFont::create("Y = ", "chatFont.fnt");
    labelY->setPosition(ccp(60.f, 10.f));
    this->addChild(labelY);

    this->m_inputY = TextInput::create(60, "Y Pos");
    m_inputY->setFilter("-0123456789");
    m_inputY->setPosition(ccp(120.f, 10.f));
    m_inputY->setTag(514);
    this->addChild(m_inputY);

    return true;
}

void PosInputBundle::textChanged(CCTextInputNode* input) {
    Signal(input->getTag(), stof(input->getString())).post();
}

void PosInputBundle::setValue(const CCPoint &pos) {
    this->m_inputX->setString(std::to_string(pos.x));
    this->m_inputY->setString(std::to_string(pos.y));
}

bool InputSliderBundle::init(const char* title, int tag, float min, float max, int accu) {
    if (!CCMenu::init())
        return false;

    this->setContentSize(ccp(200.f, 20.f));
    this->setID("map");

    auto label = CCLabelBMFont::create("Opacity = ", "chatFont.fnt");
    label->setPosition(ccp(-100.f, 10.f));
    this->addChild(label);

    this->m_input = TextInput::create(60, "0 ~ 1");
    m_input->setFilter("0123456789.");
    m_input->setPosition(ccp(-30.f, 10.f));
    m_input->setTag(-29);
    this->addChild(m_input);

    this->m_slider = Slider::create(this, menu_selector(InputSliderBundle::onSlider));
    m_slider->setPosition(ccp(40.f, 10.f));
    m_slider->setScale(0.6f);
    this->addChild(m_slider);

    return true;
}

void InputSliderBundle::textChanged(CCTextInputNode* input) {
    float val = stof(input->getString());
    this->m_slider->setValue((val - this->min) / (this->max - this->min));
    Signal(this->getTag(), val).post();
}

void InputSliderBundle::onSlider(CCObject* sender) {
    float val = this->min + (this->max - this->min) * this->m_slider->getValue();
    this->m_input->setString(std::to_string(val));
    Signal(this->getTag(), val).post();
}

void InputSliderBundle::setValue(float val) {
    this->m_input->setString(std::to_string(val));
    this->m_slider->setValue((val - this->min) / (this->max - this->min));
}

bool PreviewFrame::init() {
    if (!CCNode::init())
        return false;

    auto size = CCDirector::sharedDirector()->getWinSize();
    this->setContentSize(size);
    // center
    auto center = size / 2;

    // grid
    auto batch = CCNode::create();
    batch->setPosition(center);
    batch->setID("grid");
    this->addChild(batch);

    auto v = CCLayerColor::create(ccc4(255, 255, 255, 240));
    v->setPosition(ccp(0, 0));
    v->setContentSize(ccp(1.2, size.height));
    v->ignoreAnchorPointForPosition(false);
    batch->addChild(v);

    auto h = CCLayerColor::create(ccc4(255, 255, 255, 240));
    h->setPosition(ccp(0, 0));
    h->setContentSize(ccp(size.width, 1.2));
    h->ignoreAnchorPointForPosition(false);
    batch->addChild(h);

    float x;
    while (center.width - x > 0) {
        x += 30.f;
        auto l = CCLayerColor::create(ccc4(255, 255, 255, 144));
        l->setPosition(ccp(-x, 0));
        l->setContentSize(ccp(0.4, size.height));
        l->ignoreAnchorPointForPosition(false);
        batch->addChild(l);

        auto r = CCLayerColor::create(ccc4(255, 255, 255, 144));
        r->setPosition(ccp(+x, 0));
        r->setContentSize(ccp(0.4, size.height));
        r->ignoreAnchorPointForPosition(false);
        batch->addChild(r);
    }

    float y;
    while (center.height - y > 0) {
        y += 30.f;

        auto t = CCLayerColor::create(ccc4(255, 255, 255, 144));
        t->setPosition(ccp(0, -y));
        t->setContentSize(ccp(size.width, 0.4));
        t->ignoreAnchorPointForPosition(false);
        batch->addChild(t);

        auto b = CCLayerColor::create(ccc4(255, 255, 255, 144));
        b->setPosition(ccp(0, +y));
        b->setContentSize(ccp(size.width, 0.4));
        b->ignoreAnchorPointForPosition(false);
        batch->addChild(b);
    }

    // target node
    // the practice button (sprite here)
    this->m_sprL = CCSprite::createWithSpriteFrameName("GJ_checkpointBtn_001.png");
    m_sprL->setPositionX(-28.25f);
    m_sprL->setOpacity(gm->m_practiceOpacity);

    this->m_sprR = CCSprite::createWithSpriteFrameName("GJ_removeCheckBtn_001.png");
    m_sprR->setPositionX(41.50f);
    m_sprR->setOpacity(gm->m_practiceOpacity);

    this->m_target = CCNode::create();
    m_target->setPosition(gm->m_practicePos);
    m_target->setContentSize(ccp(0.f, 0.f));
    m_target->setID("target");
    m_target->addChild(this->m_sprL);
    m_target->addChild(this->m_sprR);
    this->addChild(m_target);

    return true;
}

