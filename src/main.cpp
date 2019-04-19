#include <algorithm>
#include <array>
#include <cinttypes>
#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iterator>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

constexpr const char* DEFAULT_INPUT[] =
{
	"BatFirstTeam.dat",
	"BatSecondTeam.dat",
	"Batter.dat",
	"Pitcher.dat",
};

// ファイル一行の最大文字数
constexpr size_t MAX_LENGTH = 512u;
// 1チームの最大コスト
constexpr unsigned int  MAX_COST = 550u;
// 野手データ最大人数
constexpr unsigned int MAX_BATTER_NUM = 65535u;
// 投手データ最大人数
constexpr unsigned int MAX_PITCHER_NUM = 65535u;
// 一試合のイニング数
constexpr int INING_NUM =9;
// 一試合の最大イニング数
constexpr int MAX_INING_NUM = 12;
// 延長イニング数
constexpr int ENTYOU_NUM = MAX_INING_NUM - INING_NUM;
// アウト数
constexpr int OUT_NUM = 3;

// fopenオプション
constexpr const char* FO_W = "w";
//constexpr const char* FO_W = "w,ccs=UTF-8";
constexpr const char* FO_R = "r";
//constexpr const char* FO_R = "r,ccs=UTF-8";

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
		
		const double rnd = static_cast<double>(rand()) / RAND_MAX;
		
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

	bool Initialize(const std::string& team_data, const std::string& batter_data, const std::string& pitcher_data)
	{
		if (!readBatterData(batter_data))
		{
			return false;
		}

		if (!readPitcherData(pitcher_data))
		{
			return false;
		}

		if (!readPlayer(team_data))
		{
			return false;
		}

		return true;
	}


	void displayTeam();

	const std::string& Name() const
	{
		return m_teamName;
	}

	int Gettokuten(void)
	{
		return tokuten;
	}

	int Katen( int katen)
	{
		tokuten += katen;

		return tokuten;
	}
	
	const BatterData& Batter( void ) const
	{
		dajun = dajun % 9;
		return m_selectedBatterPlayers[dajun++];
	}
	
	const PitcherData& Pitcher( void ) const
	{
		return m_seletedPitcherPlayer;
	}

	int Cost() const
	{
		const unsigned int batter_cost = std::accumulate(
			m_selectedBatterPlayers.cbegin(), m_selectedBatterPlayers.cend(), 0u, [](unsigned int val, const BatterData& dat) -> unsigned int {
				return val += dat.cost;
			});

		return batter_cost + m_seletedPitcherPlayer.cost;
	}

private:

	bool readBatterData(const std::string& data_file);
	bool readPitcherData(const std::string& data_file);
	bool readPlayer(const std::string& player_data);

	// チーム名
	std::string m_teamName;
	// ファイルから読み込んだ野手データ
	std::map<unsigned int, BatterData> m_batterData;
	// ファイルから読み込んだ投手データ
	std::map<unsigned int, PitcherData> m_pitcherData;
	// 選択した野手データ
	std::vector<BatterData> m_selectedBatterPlayers;
	// 選択した投手データ
	PitcherData m_seletedPitcherPlayer;
	int tokuten;
	mutable int dajun;

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

std::vector< std::string > SplitComma(const std::string& str)
{
	std::vector< std::string > result;

	std::stringstream ss(str);
	std::string buffer;
	while (std::getline(ss, buffer, ','))
	{
		result.push_back(buffer);
	}

	return result;
}

/*
 * 処理結果出力
 */
void
Team::displayTeam()
{
    printf("<%s>\n", m_teamName.c_str());

	for (const auto& dat : m_selectedBatterPlayers)
	{
		printf("%d#%s, %u\n", dat.id, dat.name.c_str(), dat.cost);
	}

	printf("%d#%s, %u\n", m_seletedPitcherPlayer.id, m_seletedPitcherPlayer.name.c_str(), m_seletedPitcherPlayer.cost);

	printf("TotalCost:%u\n", Cost());
}

/*
 * 野手データ読み込み
 */
bool
Team::readBatterData(const std::string& data_file)
{
    FILE* fp = fopen(data_file.c_str(), FO_R);
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", data_file.c_str());
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];
    for( unsigned int i = 0u ; i < MAX_BATTER_NUM ; i++)
    {
        // 一行読み込み
        memset(buffer, 0, sizeof(buffer));
        if (NULL == fgets(buffer, sizeof(buffer)-1, fp)) {
            break;
        }

		auto fields = SplitComma(buffer);

		constexpr size_t INDEX_ID = 0;
		constexpr size_t INDEX_NAME = 1;
		constexpr size_t INDEX_AVERAGE_HIT = 2;
		constexpr size_t INDEX_AVERAGE_FIRST_BASE = 3;
		constexpr size_t INDEX_AVERAGE_SECOND_BASE = 4;
		constexpr size_t INDEX_AVERAGE_THIRD_BASE = 5;
		constexpr size_t INDEX_COST = 9;
		if (fields.size() < INDEX_COST)
		{
			fprintf(stderr, "%s:%d missing fields %s\n", data_file.c_str(), i + 1, buffer);
			continue;
		}

        BatterData data;
        // ID
		try
        {
			data.id = std::stoul(fields[INDEX_ID]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d ID error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_ID].c_str(), e.what());

			continue;
		}

        // 名前
		try
        {
            data.name = fields[INDEX_NAME];
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d name error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_NAME].c_str(), e.what());

			continue;
		}

        // 打率
		try
        {
			data.daritsu = std::stod(fields[INDEX_AVERAGE_HIT]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d daritsu error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_AVERAGE_HIT].c_str(), e.what());

			continue;
		}

        // 一塁打率
		try
        {
			data.anda = std::stod(fields[INDEX_AVERAGE_FIRST_BASE]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d first base error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_AVERAGE_FIRST_BASE].c_str(), e.what());

			continue;
		}

    	// 二塁打率
		try
        {
			data.niruida = data.anda + std::stod(fields[INDEX_AVERAGE_SECOND_BASE]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d second base error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_AVERAGE_SECOND_BASE].c_str(), e.what());

			continue;
		}

		// 三塁打率
		try
        {
			data.sanruida = data.niruida + std::stod(fields[INDEX_AVERAGE_THIRD_BASE]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d 3rd base error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_AVERAGE_THIRD_BASE].c_str(), e.what());

			continue;
		}

        // コスト
		try
        {
			data.cost = std::stoul(fields[INDEX_COST]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d cost error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_COST].c_str(), e.what());

			continue;
		}

        m_batterData.insert(std::make_pair(data.id, data));

    }

    fclose(fp);

    return true;
}

/*
 * 投手データ読み込み
 */
bool
Team::readPitcherData(const std::string& data_file)
{
    FILE* fp = fopen(data_file.c_str(), FO_R);
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", data_file.c_str());
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

		auto fields = SplitComma(buffer);

		constexpr size_t INDEX_ID = 0;
		constexpr size_t INDEX_NAME = 1;
		constexpr size_t INDEX_AVERAGE_BOUGYO = 2;
		constexpr size_t INDEX_AVERAGE_SIKYU = 3;
		constexpr size_t INDEX_COST = 7;
		if (fields.size() < INDEX_COST)
		{
			fprintf(stderr, "%s:%d missing fields %s\n", data_file.c_str(), i + 1, buffer);
			continue;
		}

        PitcherData data;
        // ID
        try
		{
			data.id = std::stoul(fields[INDEX_ID]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d ID error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_ID].c_str(), e.what());

			continue;
		}

        // 名前
        data.name = fields[INDEX_NAME];

        // 防御率
		try
        {
			const double bougyo = std::stod(fields[INDEX_AVERAGE_BOUGYO]);
        	data.bougyoritsu = (0.0099 * bougyo * bougyo) - (0.039 * bougyo) + 0.2604;
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d BOUGYO error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_AVERAGE_BOUGYO].c_str(), e.what());

			continue;
		}

    	// 四球率
		try
        {
			data.sikyuuritu = std::stod(fields[INDEX_AVERAGE_SIKYU]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d SIKJYUU error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_AVERAGE_SIKYU].c_str(), e.what());

			continue;
		}

        // コスト
		try
        {
			data.cost = std::stoul(fields[INDEX_COST]);
        }
		catch (std::exception& e)
		{
			fprintf(stderr, "%s:%d COST error->%s, %s\n", data_file.c_str(), i + 1, fields[INDEX_COST].c_str(), e.what());

			continue;
		}

        m_pitcherData.insert(std::make_pair(data.id, data));

    }

    fclose(fp);

    return true;
}

/*
 * 選択選手データ読み込み
 */
bool
Team::readPlayer(const std::string& player_data)
{
    FILE* fp = fopen(player_data.c_str(), FO_R);
    if( NULL == fp )
    {
        // 読み込み失敗
        fprintf(stderr, "fopen err %s\n", player_data.c_str());
        return false;
    }

    // 読み込みバッファ
    char buffer[MAX_LENGTH];

	// 一行目はチーム名
	{
		if (NULL == fgets(buffer, sizeof(buffer) - 1, fp))
		{
			fprintf(stderr, "Team name gets err %s\n", player_data.c_str());

			return false;
		}

		// 改行を除いて取得する
		m_teamName = buffer;
		m_teamName = std::string(
			std::cbegin(m_teamName), std::find_if(
				std::crbegin(m_teamName), std::crend(m_teamName), [](char c) -> bool {
					return (c != '\n') && (c != '\r');
				}).base());
	}

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
        	const unsigned int id = (unsigned int)strtoul(tok, &e, 10);

        	
    	    if (*e != '\0') {
        		fprintf(stderr, "%d:変換不可能部分＝%s\n", __LINE__, e);
    	    	fprintf(stderr, "%d:%" PRIuPTR "文字目の\'%c\'が変換不可\n", __LINE__, e-tok+1, *e);
    	    	continue;
    	    }

        	if( 9 == i )
        	{
        		// 投手はIDから2000引く
		        m_seletedPitcherPlayer = m_pitcherData[id];
        	}
        	else
        	{
        		// 野手はIDから2000引く
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

	void SetBatterResult(const BatterData& batter, int result, int tokuten)
	{
		batter_result_.push_back(std::make_tuple(batter, result, tokuten));
	}

	size_t HitCount() const
	{
		return std::count_if(batter_result_.cbegin(), batter_result_.cend(), [](const std::tuple<BatterData, int, int>& v) -> bool {
			return (0 != ButterHitResult(v));
		});
	}

	size_t batterCount() const
	{
		return batter_result_.size();
	}

	// ヒット情報詳細
	void PrintHitVerbose(FILE* fp, int inning) const
	{
		for (const auto& i : batter_result_)
		{
			if (ButterHitResult(i) == 0)
			{
				continue;
			}

			fprintf(stdout, "%02d回 %s %d塁打", inning, std::get<0>(i).name.c_str(), ButterHitResult(i));
			fprintf(fp, "%02d回 %s %d塁打", inning, std::get<0>(i).name.c_str(), ButterHitResult(i));

			if (0 != ButterRunResult(i))
			{
				fprintf(stdout, "%4d 点\n", ButterRunResult(i));
				fprintf(fp, "%d 点\n", ButterRunResult(i));
			}
			else
			{
				fprintf(stdout, "\n");
				fprintf(fp, "\n");
			}
		}
	}

	void SetRun(int run)
	{
		run_ = run;
	}

	int Run() const
	{
		return run_;
	}

	void Validate()
	{
		valid_ = true;
	}

	bool IsValid() const
	{
		return valid_;
	}

private:

	inline static int ButterHitResult(const std::tuple< BatterData, int, int >& data)
	{
		return std::get<1>(data);
	}

	inline static int ButterRunResult(const std::tuple< BatterData, int, int >& data)
	{
		return std::get<2>(data);
	}

	std::vector< std::tuple< BatterData, int, int > > batter_result_{};	// 打順ごとの結果(バッター情報, 塁打, 得点
	int run_{ 0 };	// 得点
	bool valid_{ false };

};

class TeamPlayData
{
public:

	TeamPlayData(const std::string& name) : team_name_(name){}
	TeamPlayData(const TeamPlayData&) = default;
	TeamPlayData(TeamPlayData&&) noexcept = default;
	TeamPlayData& operator=(const TeamPlayData&) = default;
	TeamPlayData& operator=(TeamPlayData&&) noexcept = default;
	~TeamPlayData() = default;

	const std::string& TeamName() const
	{
		return team_name_;
	}

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
		return std::accumulate(std::cbegin(inning_data_), std::cend(inning_data_), 0, [](int val, const InningTeamPlayData& data) -> int {
			return val + data.Run();
		});
	}

	// スコアボードプリント
	void PrintScoreBoard(FILE* fp) const
	{
		std::for_each(std::cbegin(inning_data_), std::cend(inning_data_), [fp](const InningTeamPlayData& data) -> void {
			if (data.IsValid())
			{
				fprintf(stdout, "%4d", data.Run());
				fprintf(fp, "%4d", data.Run());
			}
			else
			{
				fprintf(stdout, "%4s", "x");
				fprintf(fp, "%4s", "x");
			}
		});
	}

	// ヒット情報プリント
	void PrintHitVerbose(FILE* fp) const
	{
		for (int i = 0; i < MAX_INING_NUM; ++i)
		{
			const InningTeamPlayData& data = inning_data_[i];
			if (!data.IsValid())
			{
				break;
			}

			data.PrintHitVerbose(fp, i + 1);
		}
	}

private:

	std::string team_name_;

	std::array< InningTeamPlayData, MAX_INING_NUM > inning_data_{};	// 各イニングごとの結果

	TeamPlayData() = delete;

};

class PlayData
{
public:

	PlayData(const std::string& bat_first_team, const std::string& bat_second_team)
		: bat_first_team_(bat_first_team)
		, bat_second_team_(bat_second_team)
	{
	}
	PlayData(const PlayData&) = default;
	PlayData& operator=(const PlayData&) = default;
	~PlayData() = default;

	const std::string& Name() const
	{
		return name_;
	}

	void SetInningResultBatFirstTeam(int inning, const InningTeamPlayData& data) { return bat_first_team_.SetInningResult(inning, data); };
	void SetInningResultBatFirstTeam(int inning, InningTeamPlayData&& data) { return bat_first_team_.SetInningResult(inning, std::move(data)); };
	void SetInningResultBatSecondTeam(int inning, const InningTeamPlayData& data) { return bat_second_team_.SetInningResult(inning, data); };
	void SetInningResultBatSecondTeam(int inning, InningTeamPlayData&& data) { return bat_second_team_.SetInningResult(inning, std::move(data)); };

	// 試合結果出力 UTF-8の文字幅判定が面倒なのでチーム名は改行して出力
	void PrintResult(FILE* fp) const
	{
		char buf[MAX_LENGTH] = {};
		bool bat_first_team_win = (bat_first_team_.Runs() > bat_second_team_.Runs());
		sprintf(buf, "<%s>\n%s\n", bat_first_team_.TeamName().c_str(), ResultString(bat_first_team_.Runs(), bat_second_team_.Runs()).c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		sprintf(buf, "<%s>\n%s\n", bat_second_team_.TeamName().c_str(), ResultString(bat_second_team_.Runs(), bat_first_team_.Runs()).c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		sprintf(buf, "<%s> %d - %d <%s>\n", bat_first_team_.TeamName().c_str(), bat_first_team_.Runs(), bat_second_team_.Runs(), bat_second_team_.TeamName().c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
	}

	// 試合経過出力 UTF-8の文字幅判定が面倒なのでチーム名は改行して出力
	void PrintGameProgress(FILE* fp, bool verbose) const
	{
		char buf[MAX_LENGTH] = {};
		sprintf(buf, "<%s>\n", bat_first_team_.TeamName().c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		bat_first_team_.PrintScoreBoard(fp);
		fprintf(stdout, "\n"); fprintf(fp, "\n");

		sprintf(buf, "<%s>\n", bat_second_team_.TeamName().c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		bat_second_team_.PrintScoreBoard(fp);
		fprintf(stdout, "\n"); fprintf(fp, "\n");

		if (!verbose)
		{
			return;
		}

		fprintf(stdout, "\n詳細\n"); fprintf(fp, "%s", buf);

		sprintf(buf, "<%s>\n", bat_first_team_.TeamName().c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		bat_first_team_.PrintHitVerbose(fp);
		sprintf(buf, "<%s>\n", bat_second_team_.TeamName().c_str());
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		bat_second_team_.PrintHitVerbose(fp);
	}

private:

	static std::string ResultString(int mine_runs, int opposition_runs)
	{
		if (mine_runs == opposition_runs)
		{
			return "even";
		}
		return (mine_runs > opposition_runs) ? ("win") : ("lose");
	}

private:

	std::string name_;

	TeamPlayData bat_first_team_;
	TeamPlayData bat_second_team_;

	PlayData() = delete;

};

//

namespace Util
{

template< class C >
constexpr auto Size(const C& v) -> decltype(v.size()) { return v.size(); }

	template< typename T, size_t SZ >
constexpr size_t Size(const T (&)[SZ]) noexcept { return SZ; }

}

// コマンドオプションを扱いやすいようvectorに変更
std::vector< std::string > OptStore(int argc, char** argv)
{
	std::vector< std::string > result;

	if (argc == 5)
	{
		result.insert(result.end(), argv, argv + argc);

		return result;
	}

	if (argc == 1)
	{
		result.push_back(argv[0]);
		result.insert(result.end(), DEFAULT_INPUT, DEFAULT_INPUT + Util::Size(DEFAULT_INPUT));

		return  result;
	}

	fprintf(stderr, "usage %s [先攻チーム選手データ 後攻チーム選手データ 野手データ 投手データ]\n", argv[0]);
	fprintf(stderr, "省略時 先攻チーム選手データ:%s\n", DEFAULT_INPUT[0]);
	fprintf(stderr, "       後攻チーム選手データ:%s\n", DEFAULT_INPUT[1]);
	fprintf(stderr, "       野手データ:%s\n", DEFAULT_INPUT[2]);
	fprintf(stderr, "       投手データ:%s\n", DEFAULT_INPUT[3]);

	return result;
}

void PlayBall( Team &senkou, Team &koukou );

int main( int argc , char** argv )
{

	// コマンドオプション
	const std::vector< std::string > options(OptStore(argc, argv));
	if (options.empty())
	{
		return -1;
	}

	const std::string& bat_first_team = options[1];
	const std::string& bat_second_team = options[2];
	const std::string& batter_data = options[3];
	const std::string& pitcher_data = options[4];

	Team senkou;	// 先攻チームデータ
	if (!senkou.Initialize(bat_first_team, batter_data, pitcher_data))
	{
		return 1;
	}

	Team koukou;	// 後攻チームデータ
	if (!koukou.Initialize(bat_second_team, batter_data, pitcher_data))
	{
		return 1;
	}

	senkou.displayTeam();
	koukou.displayTeam();

	const bool senkou_cost_over = (senkou.Cost() > MAX_COST);
	const bool koukou_cost_over = (koukou.Cost() > MAX_COST);
	if (senkou_cost_over || koukou_cost_over)
	{
		FILE* fp = fopen("result.txt", FO_W);

		char buf[MAX_LENGTH] = {};
		sprintf(buf, "<%s>\n%s\n", senkou.Name().c_str(), (senkou_cost_over) ? ("cost over") : ("win"));
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);
		sprintf(buf, "<%s>\n%s\n", koukou.Name().c_str(), (koukou_cost_over) ? ("cost over") : ("win"));
		fprintf(stdout, "%s", buf); fprintf(fp, "%s", buf);

		fclose(fp);

		return 0;
	}

	srand(static_cast<unsigned int>(time(NULL)));

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
	PlayData play_data(senkou.Name(), koukou.Name());

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

	{
		FILE* fp = fopen("result.txt", FO_W);
		play_data.PrintResult(fp);
		play_data.PrintGameProgress(fp, true);
		fclose(fp);
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
		const BatterData batter = seme.Batter();
		const int hit = Chkhit( batter, mamori.Pitcher() );

		// 出塁数が0であればアウトとする
		const int before_tokuten = tokuten;
		if( 0 == hit )
		{
			out++;
		}
		else
		{
			// ヒットであれば出塁処理と得点の加算を行う。
			tokuten = tokuten + base.Hit(hit);
		}

		inning_team_result.SetBatterResult(batter, hit, tokuten - before_tokuten);
	}

	// 得点を攻撃チームに加える。
	seme.Katen(tokuten);
	inning_team_result.SetRun(tokuten);

	inning_team_result.Validate();
	return inning_team_result;
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
	
	const double rnd1 = static_cast<double>(rand()) / RAND_MAX;
	
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

