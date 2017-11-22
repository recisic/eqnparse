#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>

using namespace std;

string parse(string input_string)
{
    typedef enum state {p, q,
        q_plus, q_minus, q_mul, q_div, q_lpar, q_rpar,
        q_a, q_b, q_c, q_d, q_x, q_y, q_z,
        q_2, q_3, q_4, q_5, q_6, q_7, q_sharp, r} state;
    const state initial_state = p;
    const state end_state = r;

    typedef char alphabet;
    typedef char stack_alphabet;
    const string terminals = "+-*/()abcdxyz234567";
    const string symbols = "abcdxyz234567";
    const char EPS = '\0';

    stack<stack_alphabet> pa_stack;
    pa_stack.push('#'); // start stack alphabet

    /* build transition function */
    typedef tuple<state, alphabet, stack_alphabet> dom;
    typedef pair<state, vector<stack_alphabet> > cod;
    map <dom, cod> transition;

    transition[make_tuple(p, EPS, '#')] = {q, {'E', '#'}};
    for (int i = 0; i < terminals.length(); i++)
    { // from q and to q
        state q_ = static_cast<state>(q_plus+i);
        transition[make_tuple(q, terminals[i], EPS)] = {q_, {}};
        transition[make_tuple(q_, EPS, terminals[i])] = {q, {}};
    }
    transition[make_tuple(q_plus, EPS, 'e')] = {q_plus, {'+', 'T', 'e'}};
    transition[make_tuple(q_plus, EPS, 't')] = {q_plus, {}};
    transition[make_tuple(q_minus, EPS, 'e')] = {q_minus, {'-', 'T', 'e'}};
    transition[make_tuple(q_minus, EPS, 't')] = {q_minus, {}};
    transition[make_tuple(q_mul, EPS, 't')] = {q_mul, {'*', 'F', 't'}};
    transition[make_tuple(q_div, EPS, 't')] = {q_div, {'/', 'F', 't'}};
    transition[make_tuple(q_lpar, EPS, 'E')] = {q_lpar, {'T', 'e'}};
    transition[make_tuple(q_lpar, EPS, 'T')] = {q_lpar, {'F', 't'}};
    transition[make_tuple(q_lpar, EPS, 'F')] = {q_lpar, {'(', 'E', ')'}};
    transition[make_tuple(q_rpar, EPS, 'e')] = {q_rpar, {}};
    transition[make_tuple(q_rpar, EPS, 't')] = {q_rpar, {}};
    for (int i = 0; i < symbols.length(); i++)
    { // for each symbols a, b, ..., 7
        state q_ = static_cast<state>(q_a+i);
        transition[make_tuple(q_, EPS, 'E')] = {q_, {'T', 'e'}};
        transition[make_tuple(q_, EPS, 'T')] = {q_, {'F', 't'}};
        transition[make_tuple(q_, EPS, 'F')] = {q_, {'A'}};
        transition[make_tuple(q_, EPS, 'A')] = {q_, {symbols[i]}};
    }
    transition[make_tuple(q, '#', EPS)] = {q_sharp, {}};
    transition[make_tuple(q_sharp, EPS, 'e')] = {q_sharp, {}};
    transition[make_tuple(q_sharp, EPS, 't')] = {q_sharp, {}};
    transition[make_tuple(q_sharp, EPS, '#')] = {r, {'#'}};

    /* run pushdown automata */
    input_string += '#';
    string::iterator it = input_string.begin();
    state curr_state = initial_state;
    state next_state;
    stack_alphabet curr_stack;
    vector<stack_alphabet> next_stack;
    cod next_conf;
    string sentential_form = "E";
    string result = "Yes\nE";

    while (curr_state != end_state)
    { // for each characters in input string
        curr_stack = pa_stack.top();
        try
        { // transition without reading input character
            next_conf = transition.at(make_tuple(curr_state, EPS, curr_stack));
            pa_stack.pop();
        }
        catch (out_of_range& e)
        {
            try
            { // transition reading input character, not reading stack
                next_conf = transition.at(make_tuple(curr_state, *it, EPS));
                it++;
            }
            catch (out_of_range& e)
            {
                try
                { // transition reading input character and stack
                    next_conf = transition.at(make_tuple(curr_state, *it, curr_stack));
                    pa_stack.pop();
                    it++;
                }
                catch (out_of_range& e)
                { // no transitions possible
                    return "No";
                }
            }
        }

        next_state = next_conf.first;
        next_stack = next_conf.second;

        if (curr_state == next_state)
        { // use production rule
            size_t f = sentential_form.find(curr_stack);
            string s = string(next_stack.data(), next_stack.size());
            sentential_form.replace(f, 1, s);
            result += "\n=> ";
            result += sentential_form;
        }

        curr_state = next_state;
        reverse(next_stack.begin(), next_stack.end());
        for (stack_alphabet &sa : next_stack)
        { // update stack
            pa_stack.push(sa);
        }
    }
    return result;
}


int main()
{
    ios::sync_with_stdio(false);

    string input_string;
    cin >> input_string;
    cout << parse(input_string);
    return 0;
}
