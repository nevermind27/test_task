#include <iostream>
#include <string>
#include <regex>
#include <algorithm>
#include <vector>
#include <fstream>

using namespace std;



struct Data {
    int dd, mm, yy;
    Data(string date_string) {
        dd = std::stoi(date_string.substr(0, 2));
        mm = std::stoi(date_string.substr(3, 2));
        yy = std::stoi(date_string.substr(6, 2));
    }

    bool operator > (const Data& d) const
    {
        if (yy != d.yy) { return yy > d.yy; }
        else {
            if (mm != d.mm) { return mm > d.mm; }
            else {
                if (dd != d.dd) { return dd > d.dd; }
            }
        }
        return false;
    }

    bool operator < (const Data& d) const
    {
        if (yy != d.yy) { return yy < d.yy; }
        else {
            if (mm != d.mm) { return mm < d.mm; }
            else {
                if (dd != d.dd) { return dd < d.dd; }
            }
        }
        return false;
    }

    bool operator == (const Data& d) const
    {
        if (dd == d.dd && mm == d.mm && yy == d.yy) {
            return true;
        }
        else { return false; }
    }
};

struct Time {
    int hh, mm, ss;
    string s;
    Time(string time_string) {
        s = time_string;
        hh = std::stoi((time_string.substr(0, 2)));
        mm = std::stoi((time_string.substr(3, 2)));
        ss = std::stoi((time_string.substr(6, 2)));
    }

    int convert_to_seconds() {
        return hh * 3600 + mm * 60 + ss;
    }
};

bool IsValidDate(const std::string& str) {
    std::regex date_regex("\\b(0[1-9]|[12][0-9]|3[01])\\.(0[1-9]|1[0-2])\\.[0-9]{2}\\b");
    return std::regex_match(str, date_regex);
}

int main()
{
    setlocale(LC_ALL, "Russian");
    
    std::string s ;
    
    while (IsValidDate(s) == false) {
        std::cout << "Введите дату начала анализируемого периода в формате dd.mm.yy \n";
        std::cin >> s ;
    }
    Data beginning(s);

    s = ' ';
    while (IsValidDate(s) == false) {
        std::cout << "\nВведите дату конца анализируемого периода в формате dd.mm.yy \n";
        std::cin >> s;
    }
    Data ending(s);

    if (bool c = ending < beginning) {
        std::cout << "Ошибка, конец периода должен быть позже начала\n";
        exit;
    }
    vector<string> operators;  //имена операторов 
    vector<int> count_turnings; //соответствующие им количества включений
    int no_user_turnings_counter = 0;
    vector<int> summary_time;
    unsigned int no_user_summary_rime = 0;
    int total_turnings = 0;
    ifstream file("person.log"); 
    string ss;
    std::regex rx{ R"((0[1-9]|[12][0-9]|3[01])[- /.](0[1-9]|1[012])[- /.](19|20)\d\d)" };
    std::regex rx2{ R"(([0-1][0-9]|2[0-3]):([0-5][0-9])(?::([0-5][0-9])))" };
    while (getline(file, s)) { 
        if (s.find("Начало работы :") != std::string::npos) {
            total_turnings++;
            if (std::regex_match(s.substr(15,10), rx)){s = s.erase(22, 2); }
            string date = s.substr(15, 8);
            if (std::regex_match(s.substr(24, 8), rx2) == false) { s = s.insert(26, "0"); }
            string time = s.substr(24, 8);
            Data start_working_date(date);
            Time start_working_time(time);
            if ((start_working_date > beginning || start_working_date == beginning) && (start_working_date < ending || start_working_date == ending)) {
                getline(file, ss);
                bool is_user = 0;
                bool is_new_user = 0;
                int iter = 0;
                if (ss.find("Оператор") != std::string::npos) {
                    is_user = 1;
                    string op = ss.substr(12, 50);
                    if (iter = std::find(operators.begin(), operators.end(), op) != operators.end()) {
                        count_turnings.at(0) += 1;
                        is_new_user = 0;
                    }
                    else {
                        operators.push_back(op);
                        count_turnings.push_back(1);
                        is_new_user = 1;
                    }
                }
                else {
                    no_user_turnings_counter++;
                }
                while (s.find("Работа завершена") == std::string::npos) {
                    getline(file, s);
                }
                if (std::regex_match(s.substr(27, 8), rx2)==false) { s = s.insert(27, "0"); }
                time = s.substr(27, 8);
                Time end_working_time(time);
                int working_time = end_working_time.convert_to_seconds() - start_working_time.convert_to_seconds();
                if (is_user && is_new_user) { summary_time.push_back(working_time); }
                if (is_user && is_new_user == 0) { summary_time[iter-1] += working_time; }
                if (is_user == 0) { no_user_summary_rime += working_time;}

            }
        }
    }

    file.close();
    for (int i = 0; i < operators.size(); i++) {
        std::cout << operators[i] << " включил аппаратуру " << count_turnings[i] << " раз(а), ";
        std::cout<< "его суммарное время  " << summary_time[i] / 3600 << " часов " << (summary_time[i] % 3600) / 60 << " минут(ы) "<< (summary_time[i] % 3600) % 60+" секунд(ы)\n ";
    }
    std::cout << " Без пользователя аппаратура включалась " << no_user_turnings_counter <<" раз(а) ";
}

