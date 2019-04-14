#include <algorithm>
#include <array>
#include <cinttypes>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <numeric>
#include <string>
#include <vector>

// ファイル一行の最大文字数
#define MAX_LENGTH (256u)
// 1チームの最大コスト
#define MAX_COST (100u)
// 野手データ最大人数
#define MAX_batter_NUM (100u)
// 投手データ最大人数
#define MAX_PITCHER_NUM (100u)
// 野手データファイル名
#define BATTER_FILE_NAME ("File1")
// 投手データファイル名
#define PITCHER_FILE_NAME ("File2")
// 一試合のイニング数
constexpr int INING_NUM =9;
// 一試合の最大イニング数
constexpr int MAX_INING_NUM = 12;
// 延長イニング数
constexpr int ENTYOU_NUM = MAX_INING_NUM - INING_NUM;
// アウト数
#define OUT_NUM (3)

// = BatterData

/*
 * 野手データ
 */
struct BatterData
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

// = PitherData

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

// = Team

class Team{
public :
	Team();
	virtual ~Team();

	bool readBatterData();
	bool readPitcherData();
	bool readPlayer(char* filename);
	void displayTeam();
	int Gettokuten(void)
	{
		return tokuten;
	}

	int Katen( int katen)
	{
		tokuten += katen;

		return tokuten;
	}
	
	BatterData Batter( void )
	{
		dajun = dajun % 9;
		return m_selectedBatterPlayers[dajun++];
	}
	
	PitcherData Pitcher( void )
	{
		return m_seletedPitcherPlayer;
	}
	
private:
	// ファイルから読み込んだ野手データ
	std::vector<BatterData> m_batterData;
	// ファイルから読み込んだ投手データ
	std::vector<PitcherData> m_pitcherData;
	// 選択した野手データ
	std::vector<BatterData> m_selectedBatterPlayers;
	// 選択した投手データ
	PitcherData m_seletedPitcherPlayer;
	int tokuten;
	int dajun;

};

Team::Team()
: m_batterData()
, m_pitcherData()
, m_selectedBatterPlayers()
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
            m_selectedBatterPlayers[i].id,
            m_selectedBatterPlayers[i].name.c_str());
    }
    printf("%d#%s\n",
        m_seletedPitcherPlayer.id,
        m_seletedPitcherPlayer.name.c_str() );
}

/*
 * 野手データ読み込み
 */
bool
Team::readBatterData()
{
    FILE* fp = fopen(BATTER_FILE_NAME, "r");
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", BATTER_FILE_NAME);
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];
    for( unsigned int i = 0u ; i < MAX_batter_NUM ; i++)
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

        BatterData data;
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\',%dが変換不可\n", __LINE__, e-tok+1, *e, *e);
    	    	continue;
    	    }

        }

        m_batterData.push_back(data);

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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\',%dが変換不可\n", __LINE__, e-tok+1, *e, *e);
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
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
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
		        m_selectedBatterPlayers.push_back(m_batterData[id]);
        	}
        }


    }

    fclose(fp);

    return true;
}

// = Base

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

// PlayData

class InningTeamPlayData
{
public:

	InningTeamPlayData() = default;
	InningTeamPlayData(const InningTeamPlayData&) = default;
	InningTeamPlayData(InningTeamPlayData&&) noexcept = default;
	InningTeamPlayData& operator=(const InningTeamPlayData&) = default;
	InningTeamPlayData& operator=(InningTeamPlayData&&) noexcept = default;
	~InningTeamPlayData() = default;

	void SetbatterResult(int result)
	{
		batter_result_.push_back(result);
	}

	size_t HitCount() const
	{
		return std::count_if(batter_result_.begin(), batter_result_.end(), [](int v) -> bool {
			return (0 != v);
		});
	}

	size_t batterCount() const
	{
		return batter_result_.size();
	}

	void SetRun(int run)
	{
		run_ = run;
	}

	int Run() const
	{
		return run_;
	}

private:

	std::vector< int > batter_result_;	// 打順ごとの結果
	int run_;	// 得点

};

class TeamPlayData
{
public:

	TeamPlayData() = default;
	TeamPlayData(const TeamPlayData&) = default;
	TeamPlayData(TeamPlayData&&) noexcept = default;
	TeamPlayData& operator=(const TeamPlayData&) = default;
	TeamPlayData& operator=(TeamPlayData&&) noexcept = default;
	~TeamPlayData() = default;

	// イニングの結果設定
	void SetInningResult(int inning, const InningTeamPlayData& data)
	{
		inning_data_[inning] = data;
	}
	void SetInningResult(int inning, InningTeamPlayData&& data)
	{
		inning_data_[inning] = std::move(data);
	}

	// 得点取得
	int Runs() const
	{
		return std::accumulate(std::begin(inning_data_), std::end(inning_data_), 0, [](int val, const InningTeamPlayData& data) -> int {
			return val + data.Run();
		});
	}

private:

	std::array< InningTeamPlayData, MAX_INING_NUM > inning_data_;	// 各イニングごとの結果

};

class PlayData
{
public:

	PlayData() = default;
	PlayData(const PlayData&) = default;
	PlayData& operator=(const PlayData&) = default;
	~PlayData() = default;

	void SetInningResultBatFirstTeam(int inning, const InningTeamPlayData& data) { return bat_first_team_.SetInningResult(inning, data); };
	void SetInningResultBatFirstTeam(int inning, InningTeamPlayData&& data) { return bat_first_team_.SetInningResult(inning, std::move(data)); };
	void SetInningResultBatSecondTeam(int inning, const InningTeamPlayData& data) { return bat_second_team_.SetInningResult(inning, data); };
	void SetInningResultBatSecondTeam(int inning, InningTeamPlayData&& data) { return bat_second_team_.SetInningResult(inning, std::move(data)); };


private:

	TeamPlayData bat_first_team_;
	TeamPlayData bat_second_team_;

};

//

void PlayBall( Team &senkou, Team &koukou );

// コマンドオプションを扱いやすいようvectorに変更
std::vector< std::string > OptStore(int argc, char** argv)
{
	std::vector< std::string > result;
	for (int i = 0; i < argc; ++i)
	{
		result.push_back(argv[i]);
	}

	return std::move(result);
}

int main( int argc , char** argv )
{
	if (argc < 5)
	{
		fprintf(stderr, "usage %s 先攻チーム選手データ 後攻チーム選手データ 野手データ 投手データ\n", argv[0]);

		return -1;
	}

	// コマンドオプション
	std::vector< std::string > options(OptStore(argc, argv));

	Team senkou;	// 先攻チームデータ
	Team koukou;	// 後攻チームデータ

    //printf("#野手データ読み込み\n");
    if(!senkou.readBatterData())
    {
        return 1;
    }

    // printf("#投手データ読み込み\n");
    if(!senkou.readPitcherData())
    {
        return 1;
    }
	
    //printf("#野手データ読み込み\n");
    if(!senkou.readBatterData())
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

InningTeamPlayData Play( Team &seme, Team &mamori );

int Chkhit( const BatterData& batter , const PitcherData& pitcher );
bool IsHit( const BatterData& batter , const PitcherData& pitcher );

bool IsInningFinishBatFirst(int inning, int first_run, int second_run)
{
	if (inning < (INING_NUM - 1))
	{	// 8回までは無条件で終了しない
		return false;
	}

	// 9回以降は後攻側が負けていれば終了しない
	if (first_run >= second_run)
	{
		return false;
	}

	// 9以降かつ先攻側が勝っていれば試合終了
	return true;
}

bool IsInningFinishBatSecond(int inning, int first_run, int second_run)
{
	if (inning < (INING_NUM - 1))
	{	// 8回までは無条件で終了しない
		return false;
	}

	// 9回以降はどちらかのチームが勝っていれば終了する
	if (first_run != second_run)
	{
		return true;
	}

	if (inning >= (MAX_INING_NUM - 1))
	{	// 最大イニングまで行けば終了する
		return true;
	}

	// 9回以降かつ最大イニング未満で同一得点なら試合続行
	return false;
}

void PlayBall( Team &senkou, Team &koukou )
{
	PlayData play_data;

	for (int inning = 0; inning < MAX_INING_NUM; ++inning)
	{
		// 先攻の攻撃
		play_data.SetInningResultBatFirstTeam(inning, Play( senkou , koukou ));
		if (IsInningFinishBatFirst(inning, senkou.Gettokuten(), koukou.Gettokuten()))
		{
			break;
		}

		// 後攻の攻撃
		play_data.SetInningResultBatSecondTeam(inning, Play( koukou , senkou ));
		if (IsInningFinishBatSecond(inning, senkou.Gettokuten(), koukou.Gettokuten()))
		{
			break;
		}
	}
}

// 1イニング
InningTeamPlayData  Play( Team &seme, Team &mamori)
{
	InningTeamPlayData inning_team_result;

	int out = 0;		// アウト数
	int tokuten = 0;	// 得点

	Base base;	// 進塁情報
	
	// アウトが規定値に達すれば処理を終了
	while( OUT_NUM > out )
	{
		// ヒット結果を判定
		const int hit = Chkhit( seme.Batter(), mamori.Pitcher() );
		inning_team_result.SetbatterResult(hit);

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
	inning_team_result.SetRun(tokuten);

	return std::move(inning_team_result);
}

// ヒット確認
int Chkhit( const BatterData& batter , const PitcherData& pitcher )
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
bool IsHit( const BatterData& batter , const PitcherData& pitcher )
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

