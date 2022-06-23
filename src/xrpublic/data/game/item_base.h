#pragma once 
#include "dtype.h" 
#include "data/game_data.h" 
 
// tolua_begin
struct item_base : public game_data 
{
    std::string comment; //ע��
    double price; //�۸�
    float point; //����
    long long num; //����
    int sid[3]; //����ID
    std::string sname[3]; //����
    std::string scomment[3]; //ע��
    double sprice[3]; //�۸�
    float spoint[3]; //����
    long long snum[3]; //����
// tolua_end

    item_base( )
    {
        price = 0.0;
        point = .0f;
        num = 0;
        memset(sid, 0, sizeof(sid[3]));
        memset(sprice, 0, sizeof(sprice[3]));
        memset(spoint, 0, sizeof(spoint[3]));
        memset(snum, 0, sizeof(snum[3]));
    }
    virtual bool read(int32_t index) override;
    virtual bool read_end(int32_t index) override;

// tolua_begin
};
// tolua_end
