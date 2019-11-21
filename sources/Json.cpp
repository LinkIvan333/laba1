//Copyright 2019 <LinkIvan333>
#include <Json.hpp>
Json::Json(const string& s) {
    unsigned int i = 0;
    i = skip_spaces(i, s);
    if (s[i] == '{') {
        is_json_object = true;
        create_map(s);
    }else if (s[i] == '[') {
        is_json_array = true;
        create_vector(s);
    } else {
        throw std::logic_error("invalid data type");
    }
}

bool Json::is_array() const{
    return is_json_array;
}

bool Json::is_object() const{
    return is_json_object;
}

std::any& Json::operator[](const string& key) {
    if (Json::is_object()) {
        return this->json_map[key];
    } else {
        throw std::logic_error("It is not an object");
    }
}

std::any& Json::operator[](int index) {
    if (is_array()) {
        return this->json_arr[index];
    } else {
        throw std::logic_error("It is not an array");
    }
}
Json Json::parse(const std::string& s) {
    Json obj(s);
    return obj;
}

Json Json::parseFile(const std::string& path_to_file){
    std::ifstream json;
    json.open(path_to_file);
    string s = "";
    string line;
    while (!json.eof()) {
        std::getline(json, line);
        s += line;
    }
    Json obj(s);
    return obj;
}

unsigned int Json::skip_spaces(unsigned int i, const string& s) {
    while (s[i] == ' ' || s[i] == ',') {
        i++;
    }
    return i;
}

unsigned int Json::find_end(unsigned int i, const string& s) {
    unsigned int cnt_open = 1 , cnt_close = 0;
    char key1, key2;
    key2 = '}';
    key1 = '{';
    if (s[i] == '[') {
        key1 = '[';
        key2 = ']';
    }
    while (cnt_close != cnt_open && i < s.length()) {
        i++;
        if (s[i] == key1) cnt_open++;
        else if (s[i] == key2) cnt_close++;
    }
    if (cnt_close != cnt_open) return s.length();
    return i;
}

string Json::cut_num(unsigned int i, const string& s) {
    unsigned int st = i;
    string num;
    while (isdigit(static_cast<unsigned char>(s[i]))) i++;
    num = s.substr(st, i - st);
    return num;
}

string Json::read_word(unsigned int &i, const string& s) {
    unsigned int st;
    string key;
    i = skip_spaces(i, s);
    if (s[i] == '\"') {
        i++;
        st = i;
    } else {
        throw std::logic_error("string isn't valid!");
    }
    while (s[i] != '"') i++;
    key = s.substr(st, i - st);
    i++;
    return key;
}

void Json::create_vector(const string& s) {
    unsigned int i = 1;
    while (i < s.find_last_of(']')) {
        skip_spaces(i, s);
        if (s[i] == '{') {
            string s1;
            unsigned int n = find_end(i, s);
            if (n == s.length())
                throw std::logic_error("string is not valid");
            s1 = s.substr(i, n - i + 1);
            Json obj(s1);
            this->json_arr.emplace_back(obj.json_map);
            i += s1.length();
        } else if (s[i] == '\"') {
            string word;
            word = read_word(i, s);
            i = skip_spaces(i, s);
            this->json_arr.emplace_back(word);
        } else if ((s[i] == 't' && s[i + 1] == 'r' && s[i + 2] == 'u'
        && s[i + 3] == 'e') || (s[i] == 'f' && s[i + 1] == 'a'
        && s[i + 2] == 'l' && s[i + 3] == 's' && s[i + 4] == 'e')) {
            bool x;
            if (s[i] == 't') {
                i += 4;
                x = true;
            } else {
                i += 5;
                x = false;
            }
            this->json_arr.emplace_back(x);
        } else if (s[i] == '[') {
            string s1;
            unsigned int n = find_end(i, s);
            if (n == s.length())
                throw std::logic_error("string is not valid");
            s1 = s.substr(i, n - i + 1);
            Json obj(s1);
            this->json_arr.emplace_back(obj.json_arr);
            i += s1.length();
        } else if (std::isdigit(static_cast<int>(s[i]))||
        (s[i] == '-'  && std::isdigit(static_cast<int>(s[i + 1])))) {
            string num = cut_num(i, s);
            i += num.length();
            double d = stod(num);
            if (d - (int) d == 0) {
                int n = (int)d;
                this->json_arr.emplace_back(n);
            } else {
                this->json_arr.emplace_back(d);
            }
        }
        i = skip_spaces(i, s);
    }
}

void Json::create_map(const string& s) {
    unsigned int i = 1;
    while (i < s.find_last_of('}')) {
        string key;
        skip_spaces(i, s);
        key = read_word(i, s);
        i = skip_spaces(i, s);
        if (s[i] != ':')
            throw std::logic_error("String is not valid");
        i++;
        i = skip_spaces(i, s);
        if (s[i] == '{') {
            string s1;
            unsigned int n = find_end(i, s);
            if ( n == s.length())
                throw std::logic_error("string is not valid");
            s1 = s.substr(i, n - i + 1);
            Json obj(s1);
            this->json_map[key] = obj.json_map;
            i += s1.length();
        } else if (s[i] == '\"') {
            string word;
            word = read_word(i, s);
            i = skip_spaces(i, s);
            this->json_map[key] = word;
        } else if ((s[i] == 't' && s[i + 1] == 'r' && s[i + 2] == 'u'
        && s[i + 3] == 'e') || (s[i] == 'f' && s[i + 1] == 'a'
        && s[i + 2] == 'l' && s[i + 3] == 's' && s[i + 4] == 'e')) {
            bool x;
            if (s[i] == 't') {
                i += 4;
                x = true;
            } else {
                i += 5;
                x = false;
            }
            this->json_map[key] = x;
        } else if (s[i] == '[') {
            string s1;
            unsigned int  n = find_end(i, s);
            if ( n == s.length())
                throw std::logic_error("string is not valid");

            s1 = s.substr(i, n - i + 1);
            Json obj(s1);
            this->json_map[key] = obj.json_arr;
            i += s1.length();
        } else if (std::isdigit(static_cast<unsigned char>(s[i])) ||
        (s[i] == '-'  && std::isdigit(static_cast<unsigned char>(s[i + 1])))) {
            string num = cut_num(i, s);
            i += num.length();
            double d = stod(num);
            if (d - (int) d == 0) {
                int n = (int)d;
                this->json_map[key] = n;
            } else {
                this->json_map[key] = d;
            }
        }
        i = skip_spaces(i, s);
    }
}
