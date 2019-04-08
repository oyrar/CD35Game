#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <vector>
#include <algorithm>
#include <string>

// ファイル一行の最大文字数
#define MAX_LENGTH (256u)
// 1チームの最大コスト
#define MAX_COST (100u)
// 野手データ最大人数
#define MAX_BUTTER_NUM (100u)
// 投手データ最大人数
#define MAX_PITCHER_NUM (100u)
// 野手データファイル名
#define BUTTER_FILE_NAME ("File1")
// 投手データファイル名
#define PITCHER_FILE_NAME ("File2")
// 一試合のイニング数
#define INING_NUM (9)
// 一試合の最大イニング数
#define MAX_INING_NUM (12)
// 延長イニング数
#define ENTYOU_NUM ( MAX_INING_NUM - INING_NUM )
// アウト数
#define OUT_NUM (3)

/*
 * 野手データ
 */
struct ButterData
{
    unsigned int	id;			// ID
    std::string		name;		// 氏名
    double			daritsu;	// 打率
	double			anda;		// 安打
	double			niruida;	// 二塁打(安打率＋二塁打率)
	double			sanruida;	// 三塁打(二塁打率＋三塁打率)
    unsigned int cost;			// コスト

	// ヒット結果判定
	int ResultHit(void) const
	{
		int hit;
		
		double rnd = rand();
		
		if( rnd < anda )
		{
			hit = 1;
		}
		else if( rnd < niruida )
		{
			hit = 2;
		}
		else if( rnd < sanruida )
		{
			hit = 3;
		}
		else 
		{
			hit = 4;
		}
		
		return hit;
	}
};

/*
 * 投手データ
 */
struct PitcherData
{
    unsigned int	id;				// ID
    std::string		name;			// 氏名
    double			bougyoritsu;	// 防御率
    double			sikyuuritu;		// 与死球

    unsigned int cost;			// コスト
};

class Team{
public :
	Team();
	virtual ~Team();

	bool readButterData();
	bool readPitcherData();
	bool readPlayer(char* filename);
	bool selectPlayers();
	void displayTeam();
	void debugPrint();
	int Gettokuten(void)
	{
		return tokuten;
	}

	int Katen( int katen)
	{
		tokuten += katen;

		return tokuten;
	}
	
	ButterData Butter( void )
	{
		dajun = dajun % 9;
		return m_selectedButterPlayers[dajun++];
	}
	
	PitcherData Pitcher( void )
	{
		return m_seletedPitcherPlayer;
	}
	
private:
	// ファイルから読み込んだ野手データ
	std::vector<ButterData> m_butterData;
	// ファイルから読み込んだ投手データ
	std::vector<PitcherData> m_pitcherData;
	// 選択した野手データ
	std::vector<ButterData> m_selectedButterPlayers;
	// 選択した投手データ
	PitcherData m_seletedPitcherPlayer;
	int tokuten;
	int dajun;

};

Team::Team()
: m_butterData()
, m_pitcherData()
, m_selectedButterPlayers()
, m_seletedPitcherPlayer()
{
	tokuten = 0;
	dajun = 0;
}

Team::~Team(){}



/*
 * 処理結果出力
 */
void
Team::displayTeam()
{
    printf("Example Team\n");
    unsigned int i;
    for( i = 0u ; i < 9 ; i++ )
    {
        printf("%d#%s\n",
            m_selectedButterPlayers[i].id,
            m_selectedButterPlayers[i].name.c_str());
    }
    printf("%d#%s\n",
        m_seletedPitcherPlayer.id,
        m_seletedPitcherPlayer.name.c_str() );
}

/*
 * 野手データ読み込み
 */
bool
Team::readButterData()
{
    FILE* fp = fopen(BUTTER_FILE_NAME, "r");
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", BUTTER_FILE_NAME);
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];
    for( unsigned int i = 0u ; i < MAX_BUTTER_NUM ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

        if( '\r' == buffer[strlen(buffer) - 2]  )
        {
            buffer[strlen(buffer) - 2] = '\0';
        }
        else if('\n' == buffer[strlen(buffer) - 1] )
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        ButterData data;
        // ID
        {
            char* tok = strtok(buffer, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.id = (unsigned int)strtoul(tok, &e, 10);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

        // 名前
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }
            data.name = tok;
        }

        // 打率
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.daritsu = (double)strtof(tok, &e);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

        // 一塁打率
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.anda = (double)strtof(tok, &e);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

    	// 二塁打率
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.niruida = data.anda + (double)strtof(tok, &e);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

		// 三塁打率
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.sanruida = data.niruida + (double)strtof(tok, &e);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

        // コスト
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                fprintf(stderr, "%d行目", i+1 );
                continue;
            }

            char *e;
            data.cost = (unsigned int)strtoul(tok, &e, 10);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:%d行目 変換不可能部分＝%s\n", __LINE__, i, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\',%dが変換不可\n", __LINE__, e-tok+1, *e, *e);
    	    	continue;
    	    }

        }

        m_butterData.push_back(data);

    }

    fclose(fp);

    return true;
}

/*
 * 投手データ読み込み
 */
bool
Team::readPitcherData()
{
    FILE* fp = fopen(PITCHER_FILE_NAME, "r");
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", PITCHER_FILE_NAME);
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];

    unsigned int i;
    for( i = 0u ; i < MAX_PITCHER_NUM ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

        if( '\r' == buffer[strlen(buffer) - 2]  )
        {
            buffer[strlen(buffer) - 2] = '\0';
        }
        else if('\n' == buffer[strlen(buffer) - 1] )
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        PitcherData data;
        // ID
        {
            char* tok = strtok(buffer, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.id = (unsigned int)strtoul(tok, &e, 10);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

        // 名前
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }
            data.name = tok;
        }

        // 防御率
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
        	double bougyo = (double)strtof(tok, &e);
        	data.bougyoritsu = (0.0099 * bougyo * bougyo) - (0.039 * bougyo) + 0.2604;

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

    	// 四球率
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
            data.sikyuuritu = (double)strtof(tok, &e);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }
        }

        // コスト
        {
            char* tok = strtok( NULL, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                fprintf(stderr, "%d行目", i+1 );
                continue;
            }

            char *e;
            data.cost = (unsigned int)strtoul(tok, &e, 10);

    	    if (*e != '\0') {
        		fprintf(stderr, "%d:%d行目 変換不可能部分＝%s\n", __LINE__, i, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\',%dが変換不可\n", __LINE__, e-tok+1, *e, *e);
    	    	continue;
    	    }

        }

        m_pitcherData.push_back(data);

    }

    fclose(fp);

    return true;
}

/*
 * 選択選手データ読み込み
 */
bool
Team::readPlayer(char* filename)
{
    FILE* fp = fopen(filename, "r");
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", PITCHER_FILE_NAME);
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];

    unsigned int i;
    for( i = 0u ; i < 10 ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

        if( '\r' == buffer[strlen(buffer) - 2]  )
        {
            buffer[strlen(buffer) - 2] = '\0';
        }
        else if('\n' == buffer[strlen(buffer) - 1] )
        {
            buffer[strlen(buffer) - 1] = '\0';
        }

        PitcherData data;
        // ID
        {
            char* tok = strtok(buffer, ",");
            if( NULL == tok )
            {
                fprintf(stderr, "analysis error %d\n", __LINE__);
                continue;
            }

            char *e;
        	unsigned int id = (unsigned int)strtoul(tok, &e, 10);

        	
    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%ld文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }

        	if( 9 == i )
        	{
        		// 投手はIDから2000引く
        		id = id - 2000;
		        m_seletedPitcherPlayer = m_pitcherData[id];
        	}
        	else
        	{
        		// 野手はIDから2000引く
        		id = id - 1000;
		        m_selectedButterPlayers.push_back(m_butterData[id]);
        	}
        }


    }

    fclose(fp);

    return true;
}

class Base
{
public :
	Base()
		: m_Base()
	{
	}
	virtual ~Base()
	{
	}

	int Hit( int hit );
	void Clear( void );

private:
	bool m_Base[4];		// 出塁者有無
};

// ヒット処理
int Base::Hit( int hit )
{
	int tokuten = 0;
	
	// 出塁済みの選手をヒット結果数分進塁させる
	for( int i = 4 - 1 ; i >= 0 ; i-- )
	{
		// 出塁者あり？
		if( m_Base[i] )
		{
			// 出塁者をなしにする。
			m_Base[i] = false;
			
			int j = i + hit;

			// ホームベースに帰れば加点する。
			if( j > 4 - 1 )
			{
				tokuten++;
			}
			else
			{
				m_Base[j] = true; // 出塁した場所をture
			}
		}
	}
	
	m_Base[0] = true; // 打者BOXは常にtrue
	
	return tokuten;
}

// クリア処理(出塁者を解除)
void Base::Clear( void )
{
	for( int i = 3 ; i > 0 ; i-- )
	{
		// 出塁者をなしにする。
		m_Base[i] = false;
	}
	
	m_Base[0] = true; // 打者BOXは常にtrue
}

void PlayBall( Team &senkou, Team &koukou );
void Play( Team &seme, Team &mamori );
int Chkhit( const ButterData& batter , const PitcherData& pitcher );
bool IsHit( const ButterData& batter , const PitcherData& pitcher );

int main( int argc , char** argv )
{
	Team senkou;	// 先攻チームデータ
	Team koukou;	// 後攻チームデータ

    //printf("#野手データ読み込み\n");
    if(!senkou.readButterData())
    {
        return 1;
    }

    // printf("#投手データ読み込み\n");
    if(!senkou.readPitcherData())
    {
        return 1;
    }
	
    //printf("#野手データ読み込み\n");
    if(!senkou.readButterData())
    {
        return 1;
    }

    // printf("#投手データ読み込み\n");
    if(!koukou.readPitcherData())
    {
        return 1;
    }

    //printf("#先行攻チームのデータ読み込み\n");
    if(!koukou.readPlayer(argv[0]))
    {
        return 1;
    }

    //printf("#後攻チームのデータ読み込み\n");
    if(!koukou.readPlayer(argv[1]))
    {
        return 1;
    }

	// 試合
	PlayBall(senkou,koukou);
	
	return 0;
}

void PlayBall( Team &senkou, Team &koukou )
{
	// 1回から9回まで実行
	for( int i = 0; i < INING_NUM; i++ )
	{
		// 先攻の攻撃
		Play( senkou , koukou );

		// 9回かつ後攻が勝っている場合はそのまま試合終了
		if( ( INING_NUM - 1 ) != i ||  senkou.Gettokuten() >= koukou.Gettokuten() )
		{
			// 後攻の攻撃
			Play( koukou , senkou );
		}
	}
	
	int entyou = 0;
	
	// 同点の場合は、延長戦を行う。
	while( senkou.Gettokuten() == koukou.Gettokuten() )
	{
		// 先攻の攻撃
		Play( senkou , koukou );
		// 後攻の攻撃
		Play( koukou , senkou );
		
		++entyou;

		// 延長は3回(全12回)まで
		if( ENTYOU_NUM <= entyou )
		{
			break;
		}
	}
}

// 1イニング
void Play( Team &seme, Team &mamori )
{
	int out = 0;		// アウト数
	int hit = 0;		// 出塁数
	int tokuten = 0;	// 得点

	Base base;	// 進塁情報
	
	// アウトが規定値に達すれば処理を終了
	while( OUT_NUM > out )
	{
		// ヒット結果を判定
		hit = Chkhit( seme.Butter(), mamori.Pitcher() );
		
		// 出塁数が0であればアウトとする
		if( 0 == hit )
		{
			out++;
		}
		else
		{
			// ヒットであれば出塁処理と得点の加算を行う。
			tokuten = tokuten + base.Hit(hit);
		}
	}

	// 得点を攻撃チームに加える。
	seme.Katen(tokuten);
}

// ヒット確認
int Chkhit( const ButterData& batter , const PitcherData& pitcher )
{
	int hit = 0;
	
	// ヒットしたか判定
	if( IsHit( batter, pitcher ) )
	{
		// ヒットであれば出塁数を算出する。
		hit = batter.ResultHit();
	}
	
	return hit;
}

// ヒット判定
bool IsHit( const ButterData& batter , const PitcherData& pitcher )
{
	const double bougyoritu = pitcher.bougyoritsu;
	const double daritu = batter.daritsu;
	
	bool ret = false;
	
	// ヒット率を取得
	const double hitritu = ( bougyoritu + daritu ) / 2.0;
	
	// 四球率を取得
	const double sikyuuritu = pitcher.sikyuuritu;
	
	const double rnd1 = rand();
	
	// 四球でもヒット扱いにする。
	if( rnd1 < ( hitritu + sikyuuritu) )
	{
		ret = true;
	}
	else
	{
		ret = false;
	}
	
	return ret;
}

