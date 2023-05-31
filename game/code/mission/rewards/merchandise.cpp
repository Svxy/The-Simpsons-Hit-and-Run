
#include <mission\rewards\merchandise.h>
#include<string.h>

Merchandise::Merchandise ( )
:   Reward(),
    m_sellerType( INVALID_SELLER_TYPE )
{
    mCost =0;
}

Merchandise::Merchandise(char* name, int level, Reward::eRewardType type, eSellerType sellerType )
:   Reward(name,level,type,eBlank),
    m_sellerType( sellerType )
{
    mCost=0;
}


Merchandise::~Merchandise ( )
{
}

