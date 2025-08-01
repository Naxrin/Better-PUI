#include <Geode/Geode.hpp>

using namespace geode::prelude;

const auto gm = GameManager::sharedState();
// event
// for positions we have unique index system
// for others robtop has this own
class Signal : public Event {
public:
    int tag;
    float value;
    Signal(int tag, float value) {
        this->tag = tag;
        this->value = value;
    }
};

// a new added bundle that allow to set position in two inputs
class PosInputBundle : public CCMenu, public TextInputDelegate {
protected:
    TextInput* m_inputX, * m_inputY;
    // init
    bool init() override;
    // text input feedback
    void textChanged(CCTextInputNode* input) override;
public:
    // set its value
    void setValue(const CCPoint &pos);
    // create it
    static PosInputBundle* create() {
        auto node = new PosInputBundle();
        if (node && node->init()) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};

// a bundle that contains a positive value
class InputSliderBundle : public CCMenu, public TextInputDelegate {
protected:
    float min, max;
    TextInput* m_input;
    Slider* m_slider;
    // init
    bool init(const char* title, int tag, float min, float max, int accu);
    // inputer
    void textChanged(CCTextInputNode* input) override;
    // slider
    void onSlider(CCObject* sender);
public:
    void setValue(float val);
    // create it
    // @param title text label
    // @param tag tag of the target value
    // @param min mininum value
    // @param max maxinum value
    // @param accu accuracy, zero for int
    static InputSliderBundle* create(const char* title, int tag, float min, float max, int accu) {
        auto node = new InputSliderBundle();
        if (node && node->init(title, tag, min, max, accu)) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};

class PreviewFrame : public CCNode {
protected:
    // target node;
    CCNode* m_target;
    CCSprite* m_sprL, * m_sprR;
    // init
    bool init();
    /*
    bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);
    void ccTouchEnded(CCTouch* touch, CCEvent* event);
    */
public:
    // change the density of grid
    void reGrid(int d);
    // set position of target node
    CCNode* getTargetNode() {
        return this->m_target;
    }
    void placeNode(const CCPoint& pos) {
        this->m_target->setPosition(pos);
    }
    void alphaNode(GLubyte opacity) {
        this->m_sprL->setOpacity(opacity);
        this->m_sprR->setOpacity(opacity);
    }

    static PreviewFrame* create() {
        auto node = new PreviewFrame();
        if (node && node->init()) {
            node->autorelease();
            return node;
        };
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};