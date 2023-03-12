#include <iostream>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <regex>

using namespace std;

namespace {

using pair_64_32 = pair <int64_t, int32_t>;

const int SIZE = 7; // Maximum size of result.

int32_t current_max = 0;

// Songs banned from voting.
unordered_set <int32_t> out_of_top_list;

// Comparator for voting_result and summary_result.
// First element on the set is the one with most votes or 
// smallest number in case of equal votes.
bool cmp(const pair_64_32 &p1, const pair_64_32 &p2) {
    if (p1.first != p2.first)
        return p1.first > p2.first;
    return p1.second < p2.second;
}

// Current list for songs which take part in the ongoing voting or summary.
set <pair_64_32, decltype(cmp)*> voting_result(cmp), 
                                 summary_result(cmp);

// Previous list for songs which were present in the past voting.
vector <int32_t> previous_voting_list, previous_summary_list;

// Maps to count votes and points scored by songs.
unordered_map <int32_t, int64_t> votes_for_songs, points_for_songs;

// Maps to remember the position of songs in previous rankings.
unordered_map <int32_t, int> previous_voting_result, previous_summary_result;

// Regular expressions used for input validation.
regex regex_votes("^\\s*[1-9]{1}[0-9]{0,7}(\\s+[1-9]{1}[0-9]{0,7})*\\s*$");
regex regex_new("^\\s*NEW\\s+[1-9]{1}[0-9]{0,7}\\s*$");
regex regex_top("^\\s*TOP\\s*$");
regex regex_empty_line("^\\s*$");

void print_error(string & line_content, size_t line_number) {
    cerr << "Error in line " << line_number << ": " << line_content << "\n";
}

void reset_places_of_songs_not_in_top() {
    unordered_set <int32_t> current_summary_list;
    for (auto song : summary_result) {
        current_summary_list.insert(song.second);
    }
    for (auto song : previous_summary_list) {
        if (current_summary_list.find(song) == 
            current_summary_list.end()) {
            previous_summary_result.erase(song);
        }
    }
    previous_summary_list.clear();
    for (auto song : current_summary_list) {
        previous_summary_list.push_back(song);
    }
}

// Function for printing the results of voting and summary.
void print_result(set <pair_64_32, decltype(cmp)*> & results, 
                  unordered_map <int32_t, int> & previous_positions) {
    int position = 1;
    for (pair_64_32 song_result: results) {
        int32_t song = song_result.second;
        int previous_position = previous_positions[song];

        cout << song << " ";
        if (previous_position == 0)
            cout << "-\n";
        else
            cout << previous_position - position << "\n";
        
        previous_positions[song] = position;
        position++;
    }
}

void make_top() {
    reset_places_of_songs_not_in_top();
    print_result(summary_result, previous_summary_result);
}

// Function for adding votes and points to songs.
// We support adding a vote as adding one point to the 'votes_for_songs'.
void add_to_result(int32_t song, set <pair_64_32, decltype(cmp)*> & results, 
            unordered_map <int32_t, int64_t> & result_points, int64_t points) {
    result_points[song] += points;
    int64_t number_of_votes = result_points[song];

    pair_64_32 previous_song_result = {number_of_votes - points, song};
    pair_64_32 new_song_result = {number_of_votes, song};
    if (results.find(previous_song_result) != results.end()) {
        // This song is ranked so it won't drop out of it.
        results.erase(previous_song_result);
        results.insert(new_song_result);
    }
    else if (results.size() < (size_t)SIZE) {
        // There are less than SIZE elements on the list, we can always
        // add a new song to it.
        results.insert(new_song_result);
    }
    else {
        auto it = results.end();
        --it;
        if (cmp(new_song_result, *it)) {
            results.erase(it);
            results.insert(new_song_result);
        } 
    }
}

// Function for printing hit list and adding points to summary.
void make_hit_list() {
    print_result(voting_result, previous_voting_result);
    int position = 1;
    for (pair_64_32 song_result: voting_result) {
        int32_t song = song_result.second;
        int64_t points = (int64_t)(SIZE - position + 1);
        add_to_result(song, summary_result, points_for_songs, points);
        position++;
    }
}

// Function for removing songs from the voting.
void ban_songs_and_clear_votings() {
    for (size_t i = 0; i < previous_voting_list.size(); i++) {
        int32_t song = previous_voting_list[i];
        if (voting_result.find({votes_for_songs[song], song}) 
                == voting_result.end()) {
            out_of_top_list.insert(song);
            previous_voting_result.erase(song);
        }
    }

    previous_voting_list.clear();
    for (pair_64_32 song_result: voting_result) {
        previous_voting_list.push_back(song_result.second);
    }

    votes_for_songs.clear();
    voting_result.clear();
}

// A function to handle the 'NEW' event.
void make_new(string & input, size_t line_number) {    
    stringstream stream_input(input);
    string new_max_string;
    bool correct_data = true;

    // Read 'NEW' as a first word.
    stream_input >> new_max_string;

    if (stream_input >> new_max_string) {
        int32_t new_max = stoi(new_max_string);
        if (new_max < current_max)
            correct_data = false;
        else
            current_max = new_max;
    }
    else {
        correct_data = false;
    }

    if (!correct_data) {
        print_error (input, line_number);
    }
    else {
        make_hit_list();
        ban_songs_and_clear_votings();
    }
}

// A function to handle the vote cast event.
void make_vote (string & input, size_t line_number) {
    stringstream stream_input(input);
    string data;
    unordered_set <int32_t> votes;

    while (stream_input >> data) {
        int32_t vote = stoi(data);
        if (vote <= current_max
                && out_of_top_list.find(vote) == out_of_top_list.end()
                && votes.find(vote) == votes.end()) {
            votes.insert(vote);
        }   
        else {
            print_error(input, line_number);
            return;
        } 
    }
    
    // We add one vote to each song.
    for (int32_t song: votes)
        add_to_result(song, voting_result, votes_for_songs, 1);
}

}

int main() {
    string input;
    size_t line_number = 0;
    while(getline(cin, input)) {
        line_number++;

        if (regex_match(input, regex_empty_line)) {
            continue;
        }

        if (regex_match(input, regex_top)) {
            make_top();
        }
        else if (regex_match(input, regex_new)) {
            make_new(input, line_number);  
        }
        else if (regex_match(input, regex_votes)) {
            make_vote(input, line_number);
        }
        else {
            // Nothing matches, error in that line.
            print_error(input, line_number);
        }
    }

    return 0;
}
