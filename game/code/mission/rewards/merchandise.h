#ifndef MERCHANDISE_H
#define MERCHANDISE_H

#include <mission\rewards\reward.h>
//derived from rewards
class Merchandise: public Reward
{
public:
    enum eSellerType
    {
        INVALID_SELLER_TYPE = -1,

        SELLER_INTERIOR,
        SELLER_SIMPSON,
        SELLER_GIL,

        NUM_SELLER_TYPES
    };
    
    Merchandise ();
    Merchandise (char* name, int level, Reward::eRewardType type, eSellerType sellerType );
    ~Merchandise();

    void SetCost(int cost);
    int GetCost() const;

    void SetSellerType( eSellerType type );
    eSellerType GetSellerType() const;
private:
    int mCost;
    eSellerType m_sellerType;
};

inline void
Merchandise::SetCost(int cost)
{
    mCost= cost;
}

inline int
Merchandise::GetCost() const
{
    return mCost;
}

inline void
Merchandise::SetSellerType( eSellerType type )
{
    m_sellerType = type;
}

inline Merchandise::eSellerType
Merchandise::GetSellerType() const
{
    return m_sellerType;
}

#endif // MERCHANDISE_H

