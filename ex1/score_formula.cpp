#include <map>
#include <algorithm>

using namespace std;

extern "C" {

	int calc_score(const map<string, int>& score_params){
		int position_in_competition = 10;
		if (score_params.at("sum_dirt_in_house") == score_params.at("dirt_collected") && score_params.at("is_back_in_docking")){
			position_in_competition = min(4, score_params.at("actual_position_in_competition"));
		}

		return max(0,
			2000
			- (position_in_competition - 1) * 50
			+ (score_params.at("winner_num_steps") - score_params.at("this_num_steps")) * 10
			- (score_params.at("sum_dirt_in_house") - score_params.at("dirt_collected")) * 3
			+ (score_params.at("is_back_in_docking") ? 50 : -200));
	}
}

