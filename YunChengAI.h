//
//  Created by CronlyGames Inc. on 14-8-26.
//  Copyright (c) 2014年 CronlyGames. All rights reserved.
//


/*
1. 最基本的常量数值定义，包括基本的面值，和一些特殊的牌值，牌型等;
2. AINode,      每次出手的牌型结构
3. OneHand,     计算手牌权重时的辅助结构
4. LordCards,   每个人的手牌
5. YunChengGame, 游戏的结构定义
*/

#ifndef __YunChengAI_H__
#define __YunChengAI_H__

#include <vector>
#include <set>
#include <algorithm>
#include <functional>
#include <string>

#include <math.h>
#include <unordered_map>
#include <stdint.h>
#include <vector>
#include <string>


using namespace std;

enum {
    // 以下为牌的面值，从3开始
    kCard_ValueLeast        =   2,
    kCard_Value3            =   3,
    kCard_Value4            =   4,
    kCard_Value5            =   5,
    kCard_Value6            =   6,
    kCard_Value7            =   7,
    kCard_Value8            =   8,
    kCard_Value9            =   9,
    kCard_ValueT            =   10,     // Ten
    kCard_ValueJ            =   11,
    kCard_ValueQ            =   12,
    kCard_ValueK            =   13,
    kCard_ValueA            =   14,
    kCard_Value2            =   15,
    kCard_ValueJoker1       =   16,
    kCard_ValueJoker2       =   17,
    kCard_ValueMax          =   18,

    kCard_TableMax          =   20,
    kCard_KindMax           =   5,

    // 特殊牌值
    kCard_Joker1            =   53,
    kCard_Joker2            =   54,
    kCard_Flower            =   55,

    kCardMask_CardValue     =   0x00ff,     // 牌的面值
    kCardMask_AnyMatch      =   0x0100,     // 任意配


    kMaxCardNum             =   56,
    kMaxPlayers             =   3,

    // 牌型定义
    kCardType_Single        =   1,   // 单纯类型, seriaNum == 1
    kCardType_Serial        =   2,   // 单顺, 双顺, 三顺(飞机), 4顺
    kCardType_Rocket        =   3,   // 火箭(大小王)
};


// 权重计算
#define kOneHandPower       (-150)
#define kPowerUnit          (-100)

#define  kMinPowerValue  (-1000000000.0f)


template <typename Ele>
void mergeTwoVectors(std::vector<Ele> & dest, const std::vector<Ele> &src) {
    dest.insert(dest.end(), src.begin(), src.end());
}

/**
* cardType是牌型，只有三种，王炸，单纯，连续;
* value 是牌型的值，单纯类型为牌的面值，连续类型为起始牌的面值，相同牌型以此比较大小;
* mainNum是主牌张数，比如三带二和飞机里mainNum=3, 连对时, mainNum=2;
* seralNum是连续张数，seralNum=1是单纯牌型，顺子时seralNum>=5;
* subNum是副牌数目，三带一和四带二时subNum=1，三带二和四带两对时,subNum=2;
* cards是牌型里包括的牌的牌值，比如三带一时，可能就是[3, 16, 42, 4], 连对时，可能就是[3, 16, 4, 17, 5, 18, 6, 19]等等
* aggregate是权重，根据不同的情况求出权重，再按照权重排序所有牌型。可以是本牌型的权重，也可以是手牌里除了本牌型外剩下所有牌加在一起的权重。
 */
struct AINode {
    int32_t cardType : 4;
    int32_t mainNum  : 4;
    int32_t value    : 10;
    int32_t seralNum : 10;
    int32_t subNum   : 4;

    float aggregate;

    std::vector<int>  cards;


public:
    AINode();
    AINode(int type, int val, int mainN, int len, int sub);
    AINode(const AINode &other);
    AINode & operator = (const AINode &other);

    bool isValidNode() const;
    void resetNode();

    int getTopValue() const;
    int getMaxCapacity() const;

    void fillJokers() ;
    void merge(const AINode & other);

    bool isRocket() const;
    bool isBomb() const;
    bool isExactLessThan(const AINode & other) const;
    bool isStrictLessThan(const AINode &other) const;

    float getPower() const;
    bool operator < (const AINode & other) const;
    bool isEqualTo(const AINode & other) const;
    std::string description() const ;
};

bool AINode_Compare_Aggregate_Reversed(const AINode &a, const AINode &b);

struct OneHand {
    float   totalPower;
    int     handNum;
    AINode  bestNode;

public:
    OneHand():bestNode() {
        totalPower = kMinPowerValue;
        handNum = 0;
    }
};

class YunChengGame;
bool cardLessThan(const int a, const int b);
bool cardGreaterThan(const int a, const int b);

class LordCards
{
public:
    static int getMinSerialLength(int mainNum);
    static int getMaxSubNum(int mainNum);
    static int getDupSubNum(int mainNum);

    static int getCardSuit(int card);
    static int getCardValue(int v);

    static bool updateHandForNode(OneHand & best, OneHand &left, AINode & node, bool isTrim);

public:
    LordCards(class YunChengGame * game, const std::vector<int>&vec);
    LordCards(class YunChengGame * game, int cards[], int num);
    ~LordCards();

    LordCards & operator = (const LordCards & other);
    void assign(class YunChengGame * game, const std::vector<int>&vec);
    void assign(class YunChengGame * game, int cards[], int num);

public:
    float winRateIfLord();
    bool  bigEnough();

    std::vector<int> removeSubset(const std::vector<int> & subset);

    int scanToTable();

public:
    std::string getKey(bool checkFlower, int &leastValue, int &maxCount);

    bool containsFlower(int value, int num);
    bool collectNode(AINode & one, int value, int num);

    OneHand    calcPowerByRemoveNode(const AINode & node);

    void       checkRocket (const std::string & key, OneHand & hand);

    void       checkBomb4 (const std::string & key, OneHand & hand, int top);
    void       checkSerial (const std::string & key, OneHand & hand, int top, int mainNum, int len, int subNum);
    void       checkSub (const std::string & key, OneHand & hand, int mainNum, int subNum, int poss);

    OneHand    calcPowerValue_noFlower();
    OneHand    calcPowerValue_expandAny(int countAny, int cardIndex);
    OneHand    calcPowerValue(bool checkFlower);

    AINode     typeAndValueFind();

public:
    void collectAllNodes(std::set<AINode> &possNodes, AINode & node, int dup);
    void sortByFactorInNodes(std::vector<AINode> &allNodes, const AINode & other, bool isDirect);

    void                 getGreaterNodes_expandAny(int countAny, int cardIndex, std::set<AINode> &possNodes, const AINode &other);
    void                 getGreaterNodes_possNode(std::set<AINode> &possNodes, const AINode &other);
    std::vector<AINode>  getNodesGreaterThan(const AINode & node);

    void  getGreaterNodes_simple(std::set<AINode> &possNodes, const AINode &other);

public:
    class YunChengGame * theGame;
    std::vector<int> theCards;
    int num;

    std::vector<int> m_fillCards[kCard_TableMax];
    int cardsTable[kCard_KindMax][kCard_TableMax];     // 保存每牌面值的数目，比如A的牌有几张
};

// #pragma mark - YunChengGame
class YunChengGame {
public:
    int     masterSeatId;
    int     curSeatId;
    int     pair3BombLevel;

    LordCards  * seatHands[kMaxPlayers + 1];

    unordered_map<std::string, OneHand> * powerOfCards;

public:
    void    init(int pair3BombLvl);

    std::string debugSnakeInfo(std::vector<int>&cards);
};

#endif /* defined(__YunChengAI__YunChengAI__) */
