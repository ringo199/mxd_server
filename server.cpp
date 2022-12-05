
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include <winsock2.h>
#include <windows.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 1024
#define READ_BUF_SIZE 1000000

DWORD WINAPI ThreadFun(LPVOID lpThreadParameter);

struct area
{
    int id;
    const char *name;

    area() {}
    area(int _id, const char *_name)
    {
        id = _id;
        name = _name;
    }
};

struct sarea
{
    int id;
    const char *name;
    int masterAreaId;

    sarea() {}
    sarea(int _id, const char *_name, int _master_area_id)
    {
        id = _id;
        name = _name;
        masterAreaId = _master_area_id;
    }
};

struct userInfo
{
    int userId;
    const char *username;
    const char *password;

    userInfo() {}
    userInfo(int _user_id, const char *_username, const char *_password)
    {
        userId = _user_id;
        username = _username;
        password = _password;
    }
};

struct character
{
    int id;
    int masterAreaId;
    int smallAreaId;
    int userId;
    const char *name;
    int level;
    int hp;
    int mp;
    int exp;
    int gameMapID;

    character() {}
    character(int _id, int _master_area_id, int _small_area_id, int _user_id, const char *_name,
              int _level, int _hp, int _mp, int _exp, int _game_map_id)
    {
        id = _id;
        masterAreaId = _master_area_id;
        smallAreaId = _small_area_id;
        userId = _user_id;
        name = _name;
        level = _level;
        hp = _hp;
        mp = _mp;
        exp = _exp;
        gameMapID = _game_map_id;
    }
};

struct lv_data_cfg
{
    int lv;
    int hp;
    int mp;
    int exp;
    int atk;
    int def;

    lv_data_cfg() {}
    lv_data_cfg(int _lv, int _hp, int _mp, int _exp, int _atk, int _def)
    {
        lv = _lv;
        hp = _hp;
        mp = _mp;
        exp = _exp;
        atk = _atk;
        def = _def;
    }
};

// ----gameMap

struct object_area
{
    int x, y, w, h;
    object_area() {}
    object_area(int _x, int _y, int _w, int _h)
    {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }
};

struct point
{
    int x, y;
    point() {}
    point(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
};

struct img_config
{
    const char *path;
    int num;

    img_config() {}
    img_config(const char *_path, int _num)
    {
        path = _path;
        num = _num;
    }
};

struct block
{
    object_area a;
    img_config img;

    block() {}
    block(object_area _a, img_config _img)
    {
        a = _a;
        img = _img;
    }
};

struct enemy
{
    int size;
    point *items;
    int enemyListSize;

    enemy() {}
    enemy(int _size, point *_items, int _enemyListSize)
    {
        size = _size;
        items = _items;
        enemyListSize = _enemyListSize;
    }
};

struct npc
{
    object_area a;
    const char *name;
    img_config img;
    int dialogID;

    npc() {}
    npc(object_area _a, const char *_name, img_config _img, int _dialogID)
    {
        a = _a;
        name = _name;
        img = _img;
        dialogID = _dialogID;
    }
};

struct transpoint
{
    int to;
    object_area a;
    img_config img;

    transpoint() {}
    transpoint(object_area _a, const char *_name, img_config _img, int _to)
    {
        a = _a;
        img = _img;
        to = _to;
    }
};

struct gameMap
{
    int id;
    object_area a;
    img_config background;
    block *blockList;
    enemy ene;
    npc *nnpc;
    point birthpoint;
    transpoint *transp;

    int blockListSize;
    int npcSize;
    int transpointSize;

    gameMap() {}
    gameMap(int _id, object_area _a, img_config _background,
            block *_blockList, enemy _ene, npc *_nnpc, point _birthpoint, transpoint *_transp,
            int _blockListSize, int _npcSize, int _transpointSize)
    {
        id = _id;
        a = _a;
        background = _background;
        blockList = _blockList;
        ene = _ene;
        nnpc = _nnpc;
        birthpoint = _birthpoint;
        transp = _transp;
        blockListSize = _blockListSize;
        npcSize = _npcSize;
        transpointSize = _transpointSize;
    }
};

struct dialog
{
    int id;
    const char *content;
    int next;

    dialog() {}
    dialog(int _id, const char *_content, int _next)
    {
        id = _id;
        content = _content;
        next = _next;
    }
};

template <class T>
class container
{
public:
    container()
    {
        this->unit_size = 10;
        this->cur_size = this->unit_size;
        this->size = 0;
        this->_t = new T[this->cur_size];
    }
    container(const char *_path)
    {
        this->unit_size = 10;
        this->cur_size = this->unit_size;
        this->size = 0;
        this->_t = new T[this->cur_size];
        this->path = _path;
    }
    ~container()
    {
        delete _t;
        _t = NULL;
    }

    T *getBegin()
    {
        return this->_t;
    }

    int getSize()
    {
        return this->size;
    }

    void push_back(T t)
    {
        this->adjust();
        this->_t[this->size] = t;
        ++this->size;
    }

    const char *path;

private:
    void adjust()
    {
        bool flag = false;
        while (this->size >= this->cur_size)
        {
            this->cur_size += this->unit_size;
            flag = true;
        }
        if (flag)
        {
            T *tmp = new T[this->cur_size];
            for (int i = 0; i < this->size; ++i)
            {
                tmp[i] = this->_t[i];
            }
            T *ttmp = _t;
            _t = tmp;
            delete ttmp;
            tmp = NULL;
            ttmp = NULL;
        }
    }

    int size;
    int cur_size;
    int unit_size;
    T *_t;
};

struct Node
{
    SOCKET value;
    Node *next;
    Node() : value(INT_MIN), next(NULL) {}
    Node(SOCKET v) : value(v), next(NULL) {}
};
Node *head;
Node *tail;

void addSOCKET(SOCKET soc)
{
    tail->next = new Node(soc);
    tail = tail->next;
}
void removeSOCKET(SOCKET soc)
{
    Node *node = head;
    Node *lastnode = head;
    while (node)
    {
        if (node->value == soc)
        {
            lastnode->next = node->next;
            if (node->next == NULL)
            {
                tail = lastnode;
            }
            delete node;
            break;
        }
        lastnode = node;
        node = node->next;
    }
}
const char *UTF8ToGB(const char *str)
{
    const char *result;
    WCHAR *strSrc;
    LPSTR szRes;

    //获得临时变量的大小
    int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    strSrc = new WCHAR[i + 1];
    MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

    //获得临时变量的大小
    i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
    szRes = new CHAR[i + 1];
    WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

    result = szRes;
    delete[] strSrc;
    delete[] szRes;

    return result;
}

void readJsonFile(const char *path, char *&res)
{
    FILE *fpRead = fopen(path, "rb");

    char buf[1000];
    memset(res, 0, READ_BUF_SIZE);

    int rc;
    while ((rc = fread(buf, sizeof(char), 5, fpRead)) != 0)
    {
        //　１.　一次读写一个字节，也可以一次读写n个字节，但rc总小于等于n，表示实际读取的字节数
        // ２.　fread 函数遇到'\0'后会停止读入，返回rc，并buf余下的位置会补0，因此不用担心会报错的情况
        // ３.　在写入的时候也要给出写入多少个（rc）,每一个数据有多少位（sizeof(typ)
        // for (int i = 0; i < rc; i++)
        // {
        //     printf("%x, ", buf[i]);
        // }
        strcat(res, buf);
    }

    fclose(fpRead);
}

cJSON *parseJson(const char *path)
{
    char *res = (char *)malloc(READ_BUF_SIZE);
    readJsonFile(path, res);
    cJSON *data = cJSON_Parse(res);
    free(res);
    return data;
}

void writeJsonFile(const char *path, char *res)
{
    FILE *fpWrite = fopen(path, "wb");
    fprintf(fpWrite, "%s", res);
    fclose(fpWrite);
}

container<area> bigAreaList("data/bigArea.json");
container<sarea> smallAreaList("data/smallArea.json");
container<character> characterList("data/character.json");
container<userInfo> userInfoList("data/userInfo.json");
container<lv_data_cfg> lvDataCfgList("data/config/lvDataCfg.json");
container<gameMap> gameMapList("data/gameMap.json");
container<dialog> dialogList("data/dialog.json");

void loadBigArea()
{
    cJSON *data = parseJson(bigAreaList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row, *id, *name;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);
        id = cJSON_GetObjectItem(row, "id");
        name = cJSON_GetObjectItem(row, "name");
        bigAreaList.push_back({id->valueint, name->valuestring});
    }
}

void saveBigArea()
{
    cJSON *data = cJSON_CreateArray();
    for (int i = 0; i < bigAreaList.getSize(); ++i)
    {
        cJSON *dataItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(dataItem, "id", bigAreaList.getBegin()[i].id);
        cJSON_AddStringToObject(dataItem, "name", bigAreaList.getBegin()[i].name);
        cJSON_AddItemToArray(data, dataItem);
    }
    writeJsonFile(bigAreaList.path, cJSON_PrintUnformatted(data));
}

void loadSmallArea()
{
    cJSON *data = parseJson(smallAreaList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row, *id, *name, *masterAreaId;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);
        id = cJSON_GetObjectItem(row, "id");
        name = cJSON_GetObjectItem(row, "name");
        masterAreaId = cJSON_GetObjectItem(row, "masterAreaId");
        smallAreaList.push_back({id->valueint, name->valuestring, masterAreaId->valueint});
    }
}

void saveSmallArea()
{
    cJSON *data = cJSON_CreateArray();
    for (int i = 0; i < smallAreaList.getSize(); ++i)
    {
        cJSON *dataItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(dataItem, "id", smallAreaList.getBegin()[i].id);
        cJSON_AddStringToObject(dataItem, "name", smallAreaList.getBegin()[i].name);
        cJSON_AddNumberToObject(dataItem, "masterAreaId", smallAreaList.getBegin()[i].masterAreaId);
        cJSON_AddItemToArray(data, dataItem);
    }
    writeJsonFile(smallAreaList.path, cJSON_PrintUnformatted(data));
}

void loadCharacterList()
{
    cJSON *data = parseJson(characterList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row, *id, *masterAreaId, *smallAreaId, *userId, *name, *level, *hp, *mp, *exp, *gameMapID;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);
        id = cJSON_GetObjectItem(row, "id");
        masterAreaId = cJSON_GetObjectItem(row, "masterAreaId");
        smallAreaId = cJSON_GetObjectItem(row, "smallAreaId");
        userId = cJSON_GetObjectItem(row, "userId");
        name = cJSON_GetObjectItem(row, "name");
        level = cJSON_GetObjectItem(row, "level");
        hp = cJSON_GetObjectItem(row, "hp");
        mp = cJSON_GetObjectItem(row, "mp");
        exp = cJSON_GetObjectItem(row, "exp");
        gameMapID = cJSON_GetObjectItem(row, "gameMapID");
        characterList.push_back({id->valueint,
                                 masterAreaId->valueint,
                                 smallAreaId->valueint,
                                 userId->valueint,
                                 name->valuestring,
                                 level->valueint,
                                 hp->valueint,
                                 mp->valueint,
                                 exp->valueint,
                                 gameMapID->valueint});
    }
}

void saveCharacterList()
{
    cJSON *data = cJSON_CreateArray();
    for (int i = 0; i < characterList.getSize(); ++i)
    {
        cJSON *dataItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(dataItem, "id", characterList.getBegin()[i].id);
        cJSON_AddNumberToObject(dataItem, "masterAreaId", characterList.getBegin()[i].masterAreaId);
        cJSON_AddNumberToObject(dataItem, "smallAreaId", characterList.getBegin()[i].smallAreaId);
        cJSON_AddNumberToObject(dataItem, "userId", characterList.getBegin()[i].userId);
        cJSON_AddStringToObject(dataItem, "name", characterList.getBegin()[i].name);
        cJSON_AddNumberToObject(dataItem, "level", characterList.getBegin()[i].level);
        cJSON_AddNumberToObject(dataItem, "hp", characterList.getBegin()[i].hp);
        cJSON_AddNumberToObject(dataItem, "mp", characterList.getBegin()[i].mp);
        cJSON_AddNumberToObject(dataItem, "exp", characterList.getBegin()[i].exp);
        cJSON_AddNumberToObject(dataItem, "gameMapID", characterList.getBegin()[i].gameMapID);
        cJSON_AddItemToArray(data, dataItem);
    }
    writeJsonFile(characterList.path, cJSON_PrintUnformatted(data));
}

void loadUserInfoList()
{
    cJSON *data = parseJson(userInfoList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row, *userId, *username, *password;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);
        userId = cJSON_GetObjectItem(row, "userId");
        username = cJSON_GetObjectItem(row, "username");
        password = cJSON_GetObjectItem(row, "password");
        userInfoList.push_back({userId->valueint,
                                username->valuestring,
                                password->valuestring});
    }
}

void saveUserInfoList()
{
    cJSON *data = cJSON_CreateArray();
    for (int i = 0; i < userInfoList.getSize(); ++i)
    {
        cJSON *dataItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(dataItem, "userId", userInfoList.getBegin()[i].userId);
        cJSON_AddStringToObject(dataItem, "username", userInfoList.getBegin()[i].username);
        cJSON_AddStringToObject(dataItem, "password", userInfoList.getBegin()[i].password);
        cJSON_AddItemToArray(data, dataItem);
    }
    writeJsonFile(userInfoList.path, cJSON_PrintUnformatted(data));
}

void loadLvDataCfgList()
{
    cJSON *data = parseJson(lvDataCfgList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row, *lv, *hp, *mp, *exp, *atk, *def;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);
        lv = cJSON_GetObjectItem(row, "lv");
        hp = cJSON_GetObjectItem(row, "hp");
        mp = cJSON_GetObjectItem(row, "mp");
        exp = cJSON_GetObjectItem(row, "exp");
        atk = cJSON_GetObjectItem(row, "atk");
        def = cJSON_GetObjectItem(row, "def");

        lvDataCfgList.push_back({lv->valueint,
                                 hp->valueint,
                                 mp->valueint,
                                 exp->valueint,
                                 atk->valueint,
                                 def->valueint});
    }
}

void loadGameMapList()
{
    cJSON *data = parseJson(gameMapList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row,
        *id,

        *a,
        *a_x,
        *a_y,
        *a_w,
        *a_h,

        *background,
        *background_path,
        *background_num,

        *blockRows,
        *blockRow,
        *block_a,
        *block_a_x,
        *block_a_y,
        *block_a_w,
        *block_a_h,
        *block_img,
        *block_img_path,
        *block_img_num,

        *enemyObj,
        *enemy_size,
        *enemy_items,
        *enemy_item,
        *enemy_item_x,
        *enemy_item_y,

        *npc_rows,
        *npc_row,
        *npc_area,
        *npc_area_x,
        *npc_area_y,
        *npc_area_w,
        *npc_area_h,
        *npc_name,
        *npc_img,
        *npc_img_path,
        *npc_img_num,
        *npc_dialog,

        *birthpoint_obj,
        *birthpoint_x,
        *birthpoint_y,

        *transpoint_rows,
        *transpoint_row,
        *transpoint_to,
        *transpoint_area,
        *transpoint_area_x,
        *transpoint_area_y,
        *transpoint_area_w,
        *transpoint_area_h,
        *transpoint_img,
        *transpoint_img_path,
        *transpoint_img_num;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);

        id = cJSON_GetObjectItem(row, "id");
        a = cJSON_GetObjectItem(row, "area");
        a_x = cJSON_GetObjectItem(a, "x");
        a_y = cJSON_GetObjectItem(a, "y");
        a_w = cJSON_GetObjectItem(a, "width");
        a_h = cJSON_GetObjectItem(a, "height");

        object_area oa = {a_x->valueint,
                          a_y->valueint,
                          a_w->valueint,
                          a_h->valueint};

        background = cJSON_GetObjectItem(row, "background");
        background_path = cJSON_GetObjectItem(background, "path");
        background_num = cJSON_GetObjectItem(background, "num");

        img_config bic = {background_path->valuestring,
                          background_num->valueint};
        blockRows = cJSON_GetObjectItem(row, "block");

        block *blockList = nullptr;
        int block_rows_size = cJSON_GetArraySize(blockRows);

        if (block_rows_size > 0)
        {
            blockList = new block[block_rows_size];

            for (int bri = 0; bri < block_rows_size; ++bri)
            {
                blockRow = cJSON_GetArrayItem(blockRows, bri);
                block_a = cJSON_GetObjectItem(blockRow, "area");
                block_a_x = cJSON_GetObjectItem(block_a, "x");
                block_a_y = cJSON_GetObjectItem(block_a, "y");
                block_a_w = cJSON_GetObjectItem(block_a, "width");
                block_a_h = cJSON_GetObjectItem(block_a, "height");

                block_img = cJSON_GetObjectItem(blockRow, "img");
                block_img_path = cJSON_GetObjectItem(block_img, "path");
                block_img_num = cJSON_GetObjectItem(block_img, "num");

                blockList[bri].a = {block_a_x->valueint,
                                    block_a_y->valueint,
                                    block_a_w->valueint,
                                    block_a_h->valueint};
                blockList[bri].img = {block_img_path->valuestring,
                                      block_img_num->valueint};
            }
        }

        enemyObj = cJSON_GetObjectItem(row, "enemy");
        enemy_size = cJSON_GetObjectItem(enemyObj, "size");
        enemy_items = cJSON_GetObjectItem(enemyObj, "items");

        int enemy_items_size = cJSON_GetArraySize(enemy_items);
        enemy ene = {enemy_size->valueint, nullptr, enemy_items_size};

        if (enemy_items_size > 0)
        {
            ene.items = new point[enemy_items_size];

            for (int eii = 0; eii < enemy_items_size; ++eii)
            {
                enemy_item = cJSON_GetArrayItem(enemy_items, eii);
                enemy_item_x = cJSON_GetObjectItem(enemy_item, "x");
                enemy_item_y = cJSON_GetObjectItem(enemy_item, "y");

                ene.items[eii] = {enemy_item_x->valueint,
                                  enemy_item_y->valueint};
            }
        }

        npc_rows = cJSON_GetObjectItem(row, "npc");
        npc *npcList = nullptr;
        int npc_rows_size = cJSON_GetArraySize(npc_rows);

        if (npc_rows_size > 0)
        {
            npcList = new npc[npc_rows_size];

            for (int ni = 0; ni < npc_rows_size; ++ni)
            {
                npc_row = cJSON_GetArrayItem(npc_rows, ni);
                npc_area = cJSON_GetObjectItem(npc_row, "area");
                npc_area_x = cJSON_GetObjectItem(npc_area, "x");
                npc_area_y = cJSON_GetObjectItem(npc_area, "y");
                npc_area_w = cJSON_GetObjectItem(npc_area, "width");
                npc_area_h = cJSON_GetObjectItem(npc_area, "height");

                npc_name = cJSON_GetObjectItem(npc_row, "name");

                npc_img = cJSON_GetObjectItem(npc_row, "img");
                npc_img_path = cJSON_GetObjectItem(npc_img, "path");
                npc_img_num = cJSON_GetObjectItem(npc_img, "num");

                npc_dialog = cJSON_GetObjectItem(npc_row, "dialog");

                npcList[ni].a = {npc_area_x->valueint,
                                 npc_area_y->valueint,
                                 npc_area_w->valueint,
                                 npc_area_h->valueint};
                npcList[ni].name = npc_name->valuestring;
                npcList[ni].img = {npc_img_path->valuestring,
                                   npc_img_num->valueint};
                npcList[ni].dialogID = npc_dialog->valueint;
            }
        }

        birthpoint_obj = cJSON_GetObjectItem(row, "birthpoint");
        birthpoint_x = cJSON_GetObjectItem(birthpoint_obj, "x");
        birthpoint_y = cJSON_GetObjectItem(birthpoint_obj, "y");
        point birthp = {birthpoint_x->valueint, birthpoint_y->valueint};

        transpoint_rows = cJSON_GetObjectItem(row, "transpoint");
        transpoint *transpointList = nullptr;
        int transpoint_rows_size = cJSON_GetArraySize(transpoint_rows);

        if (transpoint_rows_size > 0)
        {
            transpointList = new transpoint[transpoint_rows_size];

            for (int tpi = 0; tpi < transpoint_rows_size; ++tpi)
            {
                transpoint_row = cJSON_GetArrayItem(transpoint_rows, tpi);
                transpoint_to = cJSON_GetObjectItem(transpoint_row, "to");
                transpoint_area = cJSON_GetObjectItem(transpoint_row, "area");
                transpoint_area_x = cJSON_GetObjectItem(transpoint_area, "x");
                transpoint_area_y = cJSON_GetObjectItem(transpoint_area, "y");
                transpoint_area_w = cJSON_GetObjectItem(transpoint_area, "width");
                transpoint_area_h = cJSON_GetObjectItem(transpoint_area, "height");

                transpoint_img = cJSON_GetObjectItem(transpoint_row, "img");
                transpoint_img_path = cJSON_GetObjectItem(transpoint_img, "path");
                transpoint_img_num = cJSON_GetObjectItem(transpoint_img, "num");

                transpointList[tpi].a = {transpoint_area_x->valueint,
                                         transpoint_area_y->valueint,
                                         transpoint_area_w->valueint,
                                         transpoint_area_h->valueint};
                transpointList[tpi].img = {transpoint_img_path->valuestring,
                                           transpoint_img_num->valueint};
                transpointList[tpi].to = transpoint_to->valueint;
            }
        }

        gameMapList.push_back({id->valueint,
                               oa,
                               bic,
                               blockList,
                               ene,
                               npcList,
                               birthp,
                               transpointList,
                               block_rows_size,
                               npc_rows_size,
                               transpoint_rows_size});
    }
}

void loadDialogList()
{
    cJSON *data = parseJson(dialogList.path);

    int rows_size = cJSON_GetArraySize(data);
    cJSON *row, *id, *content, *next;

    for (int i = 0; i < rows_size; ++i)
    {
        row = cJSON_GetArrayItem(data, i);
        id = cJSON_GetObjectItem(row, "id");
        content = cJSON_GetObjectItem(row, "content");
        next = cJSON_GetObjectItem(row, "next");
        dialogList.push_back({id->valueint, content->valuestring, next->valueint});
    }
}

void saveDialogList()
{
    cJSON *data = cJSON_CreateArray();
    for (int i = 0; i < dialogList.getSize(); ++i)
    {
        cJSON *dataItem = cJSON_CreateObject();
        cJSON_AddNumberToObject(dataItem, "id", dialogList.getBegin()[i].id);
        cJSON_AddStringToObject(dataItem, "content", dialogList.getBegin()[i].content);
        cJSON_AddNumberToObject(dataItem, "next", dialogList.getBegin()[i].next);
        cJSON_AddItemToArray(data, dataItem);
    }
    writeJsonFile(dialogList.path, cJSON_PrintUnformatted(data));
}

void loadAllCfg()
{
    printf("正在加载数据...\n");
    loadBigArea();
    loadSmallArea();
    loadCharacterList();
    loadUserInfoList();
    // loadLvDataCfgList();
    loadGameMapList();
    loadDialogList();
    printf("所有数据已加载完毕\n");
}

void saveAllCfg()
{
    saveBigArea();
    saveSmallArea();
    saveCharacterList();
    saveUserInfoList();
    // saveDialogList();
    printf("所有数据已保存完毕\n");
}

int startup(int _port, const char *_ip)
{
    WSADATA ws;
    WSAStartup(MAKEWORD(2, 2), &ws);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(_port);
    local.sin_addr.s_addr = inet_addr(_ip);
    int len = sizeof(local);

    if (bind(sock, (struct sockaddr *)&local, len) < 0)
    {
        perror("bind");
        exit(2);
    }

    if (listen(sock, 5) < 0) //允许连接的最大数量为5
    {
        perror("listen");
        exit(3);
    }

    return sock;
}

int listen_sock;

void beforeDestory(void)
{
    saveAllCfg();
    closesocket(listen_sock);
    WSACleanup();
}

int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        printf("Usage:%s [local_ip] [local_port]\n", argv[0]);
        return 1;
    }
    loadAllCfg();

    listen_sock = startup(atoi(argv[2]), argv[1]); //初始化

    head = new Node;
    tail = head;

    atexit(beforeDestory);
    printf("初始化完成，开始准备链接客户端\n");
    while (1)
    {
        //用来接收客户端的socket地址结构体
        struct sockaddr_in remote;
        int len = sizeof(struct sockaddr_in);

        SOCKET con = accept(listen_sock, (struct sockaddr *)&remote, &len);
        if (con != INVALID_SOCKET)
        {
            //创建线程 并且传入与client通讯的套接字
            printf("get a client, ip:%s, port:%d\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port));
            HANDLE hThread = CreateThread(NULL, 0, ThreadFun, (LPVOID)con, 0, NULL);
            CloseHandle(hThread); //关闭对线程的引用
        }
    }
    return 0;
}

//线程通讯部分
DWORD WINAPI ThreadFun(LPVOID lpThreadParameter)
{
    //与客户端通讯 先发送再接收数据
    SOCKET sock = (SOCKET)lpThreadParameter;
    std::cout << "成功和" << sock << "建立连接！" << std::endl;
    addSOCKET(sock);

    char bufSend[BUF_SIZE];
    char bufRecv[BUF_SIZE];
    for (;;)
    {
        int _s = recv(sock, bufRecv, BUF_SIZE, 0);
        if (_s > 0)
        {
            bufRecv[_s] = 0;
            printf("client:%s\n", bufRecv);

            cJSON *sendData = cJSON_CreateObject();
            cJSON *sendDataData = cJSON_CreateObject();
            cJSON *recvData = cJSON_Parse(bufRecv);
            cJSON *type = cJSON_GetObjectItem(recvData, "type");
            cJSON *data = cJSON_GetObjectItem(recvData, "data");

            printf("client_type:%d\n", type->valueint);
            /**
             * 1. 获取大区
             * 2. 获取小区
             * 3. 登录并返回帐号人物信息
             * 4. 加入游戏
             * 5. 同步人物状态
             * 6. 获取地图信息
             * 7. 获取对话信息
            */
            if (type->valueint == 1)
            {
                cJSON *sendDataDataRows = cJSON_CreateArray();
                for (int i = 0; i < bigAreaList.getSize(); ++i)
                {
                    cJSON *sendDataDataRowsItem = cJSON_CreateObject();
                    cJSON_AddNumberToObject(sendDataDataRowsItem, "id", bigAreaList.getBegin()[i].id);
                    cJSON_AddStringToObject(sendDataDataRowsItem, "name", bigAreaList.getBegin()[i].name);
                    cJSON_AddItemToArray(sendDataDataRows, sendDataDataRowsItem);
                }
                cJSON_AddNumberToObject(sendData, "code", 0);
                cJSON_AddItemToObject(sendDataData, "rows", sendDataDataRows);
                cJSON_AddItemToObject(sendData, "data", sendDataData);
            }
            else if (type->valueint == 2)
            {
                cJSON *sendDataDataRows = cJSON_CreateArray();
                cJSON *id = cJSON_GetObjectItem(data, "id");
                for (int i = 0; i < smallAreaList.getSize(); ++i)
                {
                    if (smallAreaList.getBegin()[i].masterAreaId == id->valueint)
                    {
                        cJSON *sendDataDataRowsItem = cJSON_CreateObject();
                        cJSON_AddNumberToObject(sendDataDataRowsItem, "id", smallAreaList.getBegin()[i].id);
                        cJSON_AddStringToObject(sendDataDataRowsItem, "name", smallAreaList.getBegin()[i].name);
                        cJSON_AddItemToArray(sendDataDataRows, sendDataDataRowsItem);
                    }
                }
                cJSON_AddNumberToObject(sendData, "code", 0);
                cJSON_AddItemToObject(sendDataData, "rows", sendDataDataRows);
                cJSON_AddItemToObject(sendData, "data", sendDataData);
            }
            else if (type->valueint == 3)
            {
                cJSON *id = cJSON_GetObjectItem(data, "id");
                cJSON *username = cJSON_GetObjectItem(data, "username");
                cJSON *password = cJSON_GetObjectItem(data, "password");
                userInfo u;
                bool flag = false;
                for (int i = 0; i < userInfoList.getSize(); ++i)
                {
                    if (strcmp(userInfoList.getBegin()[i].username, username->valuestring) == 0)
                    {
                        u = userInfoList.getBegin()[i];
                        flag = true;
                        break;
                    }
                }
                if (!flag)
                {
                    cJSON_AddNumberToObject(sendData, "code", -1);
                    cJSON_AddItemToObject(sendData, "data", sendDataData);
                    cJSON_AddStringToObject(sendData, "message", "userinfo not found");
                }
                else
                {
                    if (strcmp(u.password, password->valuestring) != 0)
                    {
                        cJSON_AddNumberToObject(sendData, "code", -2);
                        cJSON_AddItemToObject(sendData, "data", sendDataData);
                        cJSON_AddStringToObject(sendData, "message", "password valid");
                    }
                    else
                    {
                        cJSON *sendDataDataRows = cJSON_CreateArray();
                        for (int i = 0; i < characterList.getSize(); ++i)
                        {
                            if (characterList.getBegin()[i].masterAreaId == id->valueint && u.userId == characterList.getBegin()[i].userId)
                            {
                                cJSON *sendDataDataRowsItem = cJSON_CreateObject();
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "smallAreaId", characterList.getBegin()[i].smallAreaId);
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "id", characterList.getBegin()[i].id);
                                cJSON_AddStringToObject(sendDataDataRowsItem, "name", characterList.getBegin()[i].name);
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "level", characterList.getBegin()[i].level);
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "hp", characterList.getBegin()[i].hp);
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "mp", characterList.getBegin()[i].mp);
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "exp", characterList.getBegin()[i].exp);
                                cJSON_AddNumberToObject(sendDataDataRowsItem, "gameMapID", characterList.getBegin()[i].gameMapID);
                                cJSON_AddItemToArray(sendDataDataRows, sendDataDataRowsItem);
                            }
                        }
                        cJSON_AddNumberToObject(sendData, "code", 0);
                        cJSON_AddItemToObject(sendDataData, "rows", sendDataDataRows);
                        cJSON_AddItemToObject(sendData, "data", sendDataData);
                    }
                }
            }
            else if (type->valueint == 4)
            {
                cJSON *id = cJSON_GetObjectItem(data, "id");
                char content[64];
                for (int i = 0; i < characterList.getSize(); ++i)
                {
                    if (characterList.getBegin()[i].id == id->valueint)
                    {
                        sprintf_s(content, "%s joined", characterList.getBegin()[i].name);
                        break;
                    }
                }

                type->valueint = 255;

                cJSON_AddNumberToObject(sendData, "code", 0);
                cJSON_AddNumberToObject(sendDataData, "id", userInfoList.getBegin()[0].userId);
                cJSON_AddStringToObject(sendDataData, "name", userInfoList.getBegin()[0].username);
                cJSON_AddStringToObject(sendDataData, "content", content);
                cJSON_AddItemToObject(sendData, "data", sendDataData);
            }
            else if (type->valueint == 5)
            {
                cJSON *id = cJSON_GetObjectItem(data, "id");
                cJSON *hp = cJSON_GetObjectItem(data, "hp");
                cJSON *mp = cJSON_GetObjectItem(data, "mp");
                cJSON *exp = cJSON_GetObjectItem(data, "exp");

                character *c;
                for (int i = 0; i < characterList.getSize(); ++i)
                {
                    if (characterList.getBegin()[i].id == id->valueint)
                    {
                        c = &characterList.getBegin()[i];
                        break;
                    }
                }
                int max_hp = 100 + 20 * c->level;
                int max_mp = 95 + 5 * c->level;
                int max_exp = 50 * c->level;
                int atk = 15 * c->level;
                int def = 12 * c->level;

                // 以下为计算方式
                if (hp->valueint < 0)
                {
                    c->hp += hp->valueint;
                    if (c->hp < 0)
                    {
                        c->hp = max_hp;
                        c->mp = max_mp;
                        c->exp -= 20;
                        if (c->exp < 0)
                        {
                            c->exp = 0;
                        }
                    }
                }
                else if (hp->valueint > 0)
                {
                    c->hp += hp->valueint;
                    c->hp = c->hp > max_hp ? max_hp : c->hp;
                }

                if (mp->valueint < 0)
                {
                    c->mp += mp->valueint;
                    c->mp = c->mp < 0 ? 0 : c->mp;
                }
                else if (mp->valueint > 0)
                {
                    c->mp += mp->valueint;
                    c->mp = c->mp > max_mp ? max_mp : c->mp;
                }

                if (exp->valueint > 0)
                {
                    c->exp += exp->valueint;
                    while (c->exp >= max_exp)
                    {
                        ++c->level;
                        c->exp = c->exp - max_exp;

                        max_hp = 100 + 20 * c->level;
                        max_mp = 95 + 5 * c->level;
                        max_exp = 50 * c->level;
                        atk = 15 * c->level;
                        def = 12 * c->level;
                        c->hp = max_hp;
                        c->mp = max_mp;
                    }
                }
                // ------------

                cJSON_AddNumberToObject(sendData, "code", 0);
                cJSON_AddNumberToObject(sendDataData, "level", c->level);
                cJSON_AddNumberToObject(sendDataData, "hp", c->hp);
                cJSON_AddNumberToObject(sendDataData, "mp", c->mp);
                cJSON_AddNumberToObject(sendDataData, "exp", c->exp);

                cJSON_AddNumberToObject(sendDataData, "max_hp", max_hp);
                cJSON_AddNumberToObject(sendDataData, "max_mp", max_mp);
                cJSON_AddNumberToObject(sendDataData, "max_exp", max_exp);
                cJSON_AddNumberToObject(sendDataData, "atk", atk);
                cJSON_AddNumberToObject(sendDataData, "def", def);

                cJSON_AddItemToObject(sendData, "data", sendDataData);
            }
            else if (type->valueint == 6)
            {
                cJSON *characterID = cJSON_GetObjectItem(data, "characterID");
                cJSON *gameMapID = cJSON_GetObjectItem(data, "gameMapID");
                cJSON *from = cJSON_GetObjectItem(data, "from");

                gameMap *cur_game_map;
                for (int i = 0; i < gameMapList.getSize(); ++i)
                {
                    if (gameMapList.getBegin()[i].id == gameMapID->valueint)
                    {
                        cur_game_map = &gameMapList.getBegin()[i];
                        break;
                    }
                }

                cJSON_AddNumberToObject(sendData, "code", 0);

                cJSON_AddNumberToObject(sendDataData, "gameMapID", cur_game_map->id);
                cJSON *are = cJSON_CreateObject();
                cJSON_AddNumberToObject(are, "x", cur_game_map->a.x);
                cJSON_AddNumberToObject(are, "y", cur_game_map->a.y);
                cJSON_AddNumberToObject(are, "w", cur_game_map->a.w);
                cJSON_AddNumberToObject(are, "h", cur_game_map->a.h);
                cJSON_AddItemToObject(sendDataData, "area", are);
                cJSON *bg = cJSON_CreateObject();
                cJSON_AddStringToObject(bg, "path", cur_game_map->background.path);
                cJSON_AddNumberToObject(bg, "num", cur_game_map->background.num);
                cJSON_AddItemToObject(sendDataData, "background", bg);
                cJSON *blocks = cJSON_CreateArray();
                for (int i = 0; i < cur_game_map->blockListSize; ++i)
                {
                    cJSON *block_row = cJSON_CreateObject();
                    cJSON *block_area = cJSON_CreateObject();
                    cJSON *block_img = cJSON_CreateObject();
                    cJSON_AddNumberToObject(block_area, "x", cur_game_map->blockList[i].a.x);
                    cJSON_AddNumberToObject(block_area, "y", cur_game_map->blockList[i].a.y);
                    cJSON_AddNumberToObject(block_area, "w", cur_game_map->blockList[i].a.w);
                    cJSON_AddNumberToObject(block_area, "h", cur_game_map->blockList[i].a.h);
                    cJSON_AddStringToObject(block_img, "path", cur_game_map->blockList[i].img.path);
                    cJSON_AddNumberToObject(block_img, "num", cur_game_map->blockList[i].img.num);
                    cJSON_AddItemToObject(block_row, "area", block_area);
                    cJSON_AddItemToObject(block_row, "img", block_img);
                    cJSON_AddItemToArray(blocks, block_row);
                }
                cJSON_AddItemToObject(sendDataData, "block", blocks);

                cJSON *npcs = cJSON_CreateArray();
                for (int i = 0; i < cur_game_map->npcSize; ++i)
                {
                    cJSON *npc_row = cJSON_CreateObject();
                    cJSON *npc_area = cJSON_CreateObject();
                    cJSON *npc_img = cJSON_CreateObject();
                    cJSON_AddNumberToObject(npc_area, "x", cur_game_map->nnpc[i].a.x);
                    cJSON_AddNumberToObject(npc_area, "y", cur_game_map->nnpc[i].a.y);
                    cJSON_AddNumberToObject(npc_area, "w", cur_game_map->nnpc[i].a.w);
                    cJSON_AddNumberToObject(npc_area, "h", cur_game_map->nnpc[i].a.h);
                    cJSON_AddStringToObject(npc_img, "path", cur_game_map->nnpc[i].img.path);
                    cJSON_AddNumberToObject(npc_img, "num", cur_game_map->nnpc[i].img.num);
                    cJSON_AddNumberToObject(npc_row, "dialogID", cur_game_map->nnpc[i].dialogID);
                    cJSON_AddStringToObject(npc_row, "name", cur_game_map->nnpc[i].name);
                    cJSON_AddItemToObject(npc_row, "area", npc_area);
                    cJSON_AddItemToObject(npc_row, "img", npc_img);
                    cJSON_AddItemToArray(npcs, npc_row);
                }

                cJSON_AddItemToObject(sendDataData, "npc", npcs);

                cJSON *transpoint_json = cJSON_CreateArray();
                for (int i = 0; i < cur_game_map->transpointSize; ++i)
                {
                    cJSON *transpoint_row = cJSON_CreateObject();
                    cJSON *transpoint_area = cJSON_CreateObject();
                    cJSON *transpoint_img = cJSON_CreateObject();
                    cJSON_AddNumberToObject(transpoint_area, "x", cur_game_map->transp[i].a.x);
                    cJSON_AddNumberToObject(transpoint_area, "y", cur_game_map->transp[i].a.y);
                    cJSON_AddNumberToObject(transpoint_area, "w", cur_game_map->transp[i].a.w);
                    cJSON_AddNumberToObject(transpoint_area, "h", cur_game_map->transp[i].a.h);
                    cJSON_AddStringToObject(transpoint_img, "path", cur_game_map->transp[i].img.path);
                    cJSON_AddNumberToObject(transpoint_img, "num", cur_game_map->transp[i].img.num);
                    cJSON_AddNumberToObject(transpoint_row, "to", cur_game_map->transp[i].to);
                    cJSON_AddItemToObject(transpoint_row, "area", transpoint_area);
                    cJSON_AddItemToObject(transpoint_row, "img", transpoint_img);
                    cJSON_AddItemToArray(transpoint_json, transpoint_row);
                }

                cJSON_AddItemToObject(sendDataData, "transpoint", transpoint_json);

                cJSON *poi = cJSON_CreateObject();
                if (from == nullptr)
                {
                    cJSON_AddNumberToObject(poi, "x", cur_game_map->birthpoint.x);
                    cJSON_AddNumberToObject(poi, "y", cur_game_map->birthpoint.y);
                }
                else
                {
                    bool flag = false;
                    for (int i = 0; i < cur_game_map->transpointSize; ++i)
                    {
                        if (cur_game_map->transp[i].to == from->valueint)
                        {
                            cJSON_AddNumberToObject(poi, "x", cur_game_map->transp[i].a.x);
                            cJSON_AddNumberToObject(poi, "y", cur_game_map->transp[i].a.y);
                            flag = true;
                            break;
                        }
                    }
                    if (!flag)
                    {
                        cJSON_AddNumberToObject(poi, "x", cur_game_map->birthpoint.x);
                        cJSON_AddNumberToObject(poi, "y", cur_game_map->birthpoint.y);
                    }
                }
                cJSON_AddItemToObject(sendDataData, "point", poi);

                cJSON_AddItemToObject(sendData, "data", sendDataData);

                for (int i = 0; i < characterList.getSize(); ++i)
                {
                    if (characterList.getBegin()[i].id == characterID->valueint)
                    {
                        characterList.getBegin()[i].gameMapID = gameMapID->valueint;
                        break;
                    }
                }
            }
            else if (type->valueint == 7)
            {
                cJSON *id = cJSON_GetObjectItem(data, "id");
                dialog *d = nullptr;

                for (int i = 0; i < dialogList.getSize(); ++i)
                {
                    if (dialogList.getBegin()[i].id == id->valueint)
                    {
                        d = &dialogList.getBegin()[i];
                        break;
                    }
                }
                if (d == nullptr)
                {
                    cJSON_AddNumberToObject(sendData, "code", -255);
                    cJSON_AddStringToObject(sendData, "message", "找不到消息");
                }
                else
                {
                    cJSON_AddNumberToObject(sendData, "code", 0);
                    cJSON_AddNumberToObject(sendDataData, "id", d->id);
                    cJSON_AddStringToObject(sendDataData, "content", d->content);
                    cJSON_AddNumberToObject(sendDataData, "next", d->next);
                    cJSON_AddItemToObject(sendData, "data", sendDataData);
                }
            }
            else if (type->valueint == 255)
            {
                cJSON *id = cJSON_GetObjectItem(data, "id");
                cJSON *content = cJSON_GetObjectItem(data, "content");
                char name[64];
                for (int i = 0; i < characterList.getSize(); ++i)
                {
                    if (characterList.getBegin()[i].id == id->valueint)
                    {
                        sprintf_s(name, "%s", characterList.getBegin()[i].name);
                        break;
                    }
                }
                cJSON_AddNumberToObject(sendData, "code", 0);
                cJSON_AddNumberToObject(sendDataData, "id", id->valueint);
                cJSON_AddStringToObject(sendDataData, "name", name);
                cJSON_AddStringToObject(sendDataData, "content", content->valuestring);
                cJSON_AddItemToObject(sendData, "data", sendDataData);
            }
            cJSON_AddNumberToObject(sendData, "type", type->valueint);

            sprintf(bufSend, "%s", cJSON_PrintUnformatted(sendData));
            if (type->valueint == 255)
            {
                Node *node = head;
                node = node->next;
                while (node)
                {
                    send(node->value, bufSend, BUF_SIZE, 0);
                    node = node->next;
                }
            }
            else
            {
                send(sock, bufSend, BUF_SIZE, 0);
            }
        }
        else
        {
            printf("client is quit!\n");
            removeSOCKET(sock);
            break;
        }
    }
    return 0;
}