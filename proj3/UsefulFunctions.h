#ifndef USEFULFUNCTIONS_H
#define USEFULFUNCTIONS_H

#include <sstream>
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class UsefulFunctions {
public:
    // проверка правильности даты
    bool isCorrectDateFormat(string str) {
        // если у нас равное количество символов по формату: dd.MM.YYYY » на месте числа стоит только цифра
        if (str.size() == 10 && (isdigit(str[0]) && isdigit(str[1]) && isdigit(str[3]) && isdigit(str[4]) && isdigit(str[6]) && isdigit(str[7]) && isdigit(str[8]) && isdigit(str[9]))) {
            return true;
        }
        else return false;
    }

    // возврат копии строки, переведЄнный в низкий регистр
    string toLowerCase(string str) {
        for (auto i=0; i<str.size(); i++) {
            str[i] = tolower(str[i]);
        }
        return str;
    }

    // передаваемую строку переводим в низкий регистр
    void toLowerCaseSource(string& str) {
        for (auto i=0; i<str.size(); i++) {
            str[i] = tolower(str[i]);
        }
    }

    // возврат очищенной строки от пробелов в начале и конце
    string trim(string str) {
        size_t x = 0;
        // ищем вход¤щий первый символ, который не ¤вл¤етс¤ пробелом или \t
        for (auto i=0; str[i] != '\0'; i++) {
            if (!isblank(str[i])) {
                x += i;
                break;
            }
        }
        // ищем с конца по начало первый символ, не ¤вл¤ющийс¤ пробелом
        for (auto i=str.size()-1; i >= 0; i--) {
            if (!isblank(str[i])) {
                return str.substr(x, (i-x)+1);
            }
        }
        return "";
    }

    // очищаем пердаваемую строку от пробелов в начале и конце
    void trimSource(string& str) {
        size_t x = 0;
        // ищем вход¤щий первый символ, который не ¤вл¤етс¤ пробелом или \t
        for (auto i=0; str[i] != '\0'; i++) {
            if (!isblank(str[i])) {
                x += i;
                break;
            }
        }
        // ищем с конца по начало первый символ, не ¤вл¤ющийс¤ пробелом
        for (auto i=str.size()-1; i >= 0; i--) {
            if (!isblank(str[i])) {
                str = str.substr(x, (i-x)+1);
                return;
            }
        }
        str = "";
    }

    // возврат числа int из строки
    int toInteger(string str) {
        int value = 0;
        bool oneMinusHave = false;
        stringstream ss;
        // проверка на посторонние знаки
        for (auto i=0; i<str.size(); i++) {
            if ((!isdigit(str[i]) && str[i] != '-') || ((str[i] == '-') && oneMinusHave)) return value;
            if (str[i] == '-') oneMinusHave = true;
        }
        // перевод из строки в число
        ss << str;
        ss >> value;
        return value;
    }

    // возврат числа long из строки
    long toLong(string str) {
        long value = 0;
        bool oneMinusHave = false;
        stringstream ss;
        // проверка на посторонние знаки
        for (auto i=0; i<str.size(); i++) {
            if ((!isdigit(str[i]) && str[i] != '-') || ((str[i] == '-') && oneMinusHave)) return value;
            if (str[i] == '-') oneMinusHave = true;
        }
        // перевод из строки в число
        ss << str;
        ss >> value;
        return value;
    }

    // экстракт числа из строки, содержащей любые символы
    long extractLong(string str) {
        string line = "";
        long value = 0;
        bool oneMinusHave = false;
        stringstream ss;
        // сборка числа
        for (auto i=0; i<str.size(); i++) {
            if ((isdigit(str[i]) && str[i] == '-')  && !oneMinusHave || (isdigit(str[i]))) line += str[i];
            if (str[i] == '-') oneMinusHave = true;
        }
        // перевод из строки в число
        ss << line;
        ss >> value;
        return value;
    }

    // возврат строки
    template <typename T>
    string toString(T obj) {
        string str;
        stringstream ss;
        ss << obj;
        ss >> str;
        ss.clear();
        ss.str("");
        return str;
    }

    // псевдо-очистка консоли (пускай незнающие люди думают что здесь что-то умное)
    void clearConsole(){
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" << endl;
    }

    double toDouble(string str) {
        double value = -0.00001015;
        bool oneMinusHave = false;
        bool onePointHave = false;
        stringstream ss;
        // проверка на посторонние знаки
        for (auto i=0; i<str.size(); i++) {
            if ((!isdigit(str[i]) && str[i] != '-' && str[i] != '.') || ((str[i] == '-') && oneMinusHave) || ((str[i] == '.') && onePointHave)) return value;
            if (str[i] == '-') oneMinusHave = true;
            if (str[i] == '.') onePointHave = true;
        }
        // перевод из строки в число
        ss << str;
        ss >> value;
        return value;
    }

    // возврат нового вектора: сборка перечисленных значений (например: "утка, псина, резина")
    vector<string> collectValues(string str, string iterator, bool trimmed, bool lowercased) {
        vector<string> values;
        int i = 0;
        if (iterator.size() > 1) {
            int x = 0;
            for (auto a=0; str[a] != '\0'; a++) {
                if (str[a] == iterator[x]) x++;                 // повышаем счетчик Х, если символы в строке начинают совпадать с символами итератора
                else if (str[a] != iterator[x] && x > 0) x = 0; // если далее обнаруживается несоответствие, счетчик Х - сбрасываем

                // записываем значение в вектор
                if (iterator.size() == x) {
                    if (a-1 >= 0) {
                        if (trimmed && !lowercased) values.push_back(trim(str.substr(i, a-i)));
                        else if (lowercased && !trimmed) values.push_back(toLowerCase(str.substr(i, a-i)));
                        else if (trimmed && lowercased) values.push_back(trim(toLowerCase(str.substr(i, a-i))));
                        else values.push_back(str.substr(i, a-i));
                    }
                    i = a+1;
                    x = 0;
                }
            }
        }
        else if (iterator.size() == 1) {
            char iter = iterator[0];
            for (auto a=0; str[a] != '\0'; a++) {
                // увеличиваем индекс a, пока не наткнемся на итератор или конец строки - далее идет запись значения
                if (str[a] == iter) {
                    if (a-1 >= 0) values.push_back(str.substr(i, a-i));
                    i = a+1;
                }
            }
        }
        return values;
    }

    // перевод строчных "true" и "false" в булевое значение
    bool stringToBool(string str) {
        if (trim(toLowerCase(str)).find("true") != string::npos) return true;
        else return false;
    }

    // продвинутый getline, который не обижается после использования cin и игнорирует \n
    template<typename T>
    void getinput(T& obj){
        getline(cin, obj);
        //cin.ignore(numeric_limits<streamsize>::max(), '\n');
        //cin.clear();
    }
};
#endif // USEFULFUNCTIONS_H
