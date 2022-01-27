#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<filesystem>
#include<cassert>

using namespace std;
namespace fs = std::filesystem;

// ��������� �������
struct timer {
    int minute;
    int hour;
};

// ��������� ����
struct date {
    int day;
    int month;
    int year;
};

// �������� ��������� ��� ���
bool operator==(const date& frst, const date& scnd) {
    if (frst.day == scnd.day) {
        if (frst.month == scnd.month) {
            if (frst.year == scnd.year) {
                return true;
            }
        }
    }

    return false;
}

// ��������� ������
struct row {
    // ����� ������ � �����, � ���� ����
    int		pos;
    // ���� ������
    date	dateAdmission;
    // ����� ������
    timer	currTime;
    // ���� ��������
    date	bithDate;
    // ������������� �����
    int		numDoctor;
    // ��������� ������
    vector<string> strValues;
    // ����� �����, �� �������� �������������� ��������
    int		numFile;
};

// ��������� �������
struct table {
    // �������� �������
    vector<string>	columnName;
    // ������ ��������� ����� � �����
    vector<int>		columnNameIdx;
    // ��� �����, ������ ���� ��������� �������
    vector<string>	fileName;
    // ����� ��������
    vector<string>	nameDoctor;
    // ������ �������
    vector<row>		tableRows;
};

// ��������� ������, �� ���������
void split(const string& input, vector<string>& output, string delim=";") {
    size_t prev = 0;
    size_t next;
    size_t delta = delim.length();
    while ((next = input.find(delim, prev)) != string::npos) {
        output.push_back(input.substr(prev, next - prev));
        prev = next + delta;
    }	
    output.push_back(input.substr(prev));
}

// ��������� �������, �� ������ � ������� ��������
template <typename T>
int elementInArray(vector<T>& _array, const T& _elem) {
    const vector<T>::const_iterator _Iter = find(_array.begin(), _array.end(), _elem);
    if (_Iter == _array.end()) {
        _array.push_back(_elem);
        return _array.size()-1;
    }
    else {
        return distance(_array.cbegin(), _Iter);
    }
}

// ��������� ������ � ��� �������
void str2Timer(const string& input, timer& _time) {
    vector<string> strArray;
    split(input, strArray, ":");
    if (strArray.size() != 2) {
        //assert()
        cout << "Bad in str2Timer, input = " << input << endl;
    }
    else {
        _time.hour		=	atoi(strArray[0].c_str());
        _time.minute	=	atoi(strArray[1].c_str());
    }
}

// ��������� ������ � ��� ����
void str2Date(const string& input, date& _date) {
    vector<string> strArray;
    split(input, strArray, ".");
    if (strArray.size() != 3) {
        //assert()
        cout << "Bad in str2Date, input = " << input << endl;
    }
    else {
        _date.day	=	atoi(strArray[0].c_str());
        _date.month =	atoi(strArray[1].c_str());
        _date.year	=	atoi(strArray[2].c_str());
    }
}

// ������ ��������� ����
void readFile(const string& fileName, table& bigTable) {
    ifstream		inputFile(fileName);

    const int		numFile = (int)bigTable.fileName.size();
    bigTable.fileName.push_back(fileName);

    char			_buffer[1000];
    row				currRow;
    currRow.numFile                 =   numFile;

    vector<string>&	columnName		=	bigTable.columnName;
    vector<int>&	columnNameIdx	=	bigTable.columnNameIdx;
    vector<string>& nameDoctor		=	bigTable.nameDoctor;
    int				numDoctor		=	-1;
    date			dateAdmission;
    int				idxBDay			=	-1, idxTime	=	-1, idxName	=	-1;
    int				idxColumnTime	=	-1;
    int				idxColumnBDay   =   -1;

    while (!inputFile.eof()) {
        inputFile.getline(_buffer, 1000);

        string		currString(_buffer);
        vector<string> outputStrings;
        split(currString, outputStrings);

        if (!outputStrings.empty()) {
            const string& currStr = outputStrings[0];
            int digit = atoi(currStr.c_str());
            if (digit >= 1) {
                currRow.pos = digit;

                str2Timer(outputStrings[idxColumnTime], currRow.currTime);
                str2Date(outputStrings[idxColumnBDay], currRow.bithDate);

                for (int idx = 0; idx < (int)columnNameIdx.size(); idx++) {
                    const int       idxStr  =   columnNameIdx[idx];
                    const string&   currStr =   outputStrings[idxStr];
                    currRow.strValues[idx]  =   currStr;
                }

                bigTable.tableRows.push_back(currRow);
            }
            else if (currStr == "����:") {
                for (int idxStr = 1; idxStr < (int)outputStrings.size(); idxStr++) {
                    const string& currStr = outputStrings[idxStr];
                    if (!currStr.empty()) {
                        numDoctor = elementInArray(nameDoctor, currStr);
                        currRow.numDoctor = numDoctor;
                        break;
                    }
                }
            }
            else if (currStr == "� �/�") {
                bool frstInsert = (columnName.size() == 0);
                for (int idxStr = 0; idxStr < (int)outputStrings.size(); idxStr++) {
                    const string& currStr = outputStrings[idxStr];
                    if (currStr.empty()) continue;

                    if (frstInsert == true) {
                        if (currStr == "� �/�") {
                        }
                        else if (currStr == "�����") {
                            idxColumnTime = idxStr;
                        }
                        else if (currStr == "���� ��������") {
                            idxColumnBDay = idxStr;
                        }
                        else {
                            columnName.push_back(currStr);
                            columnNameIdx.push_back(idxStr);
                        }
                    }
                    else{
                        if (currStr == "� �/�") {
                            if (idxStr == 0)    continue;
                        }
                        else if (currStr == "�����") {
                            if (idxColumnTime == idxStr) continue;
                        }
                        else if (currStr == "���� ��������") {
                            if (idxColumnBDay == idxStr) continue;
                        }

                        vector<int>::iterator _Iter = find(columnNameIdx.begin(), columnNameIdx.end(), idxStr);
                        if (_Iter == columnNameIdx.end()) {
                            assert("��� ������� � ������� ������� �� ������������� ��� �������� �������.");
                        }

                        const int idxNew = distance(columnNameIdx.begin(), _Iter);
                        if (columnName[idxNew] != currStr) {
                            assert("Column name in previus table not equal current table");
                        }
                    }
                }
                if (frstInsert == true) {
                    currRow.strValues.resize(columnName.size());
                }
                cout << "Column name - insert!" << endl;
            }
            else if (currStr == "����:") {
                for (int idxStr = 0; idxStr < (int)outputStrings.size(); idxStr++) {
                    const string& currStr = outputStrings[idxStr];
                    if (!currStr.empty()) {
                        str2Date(currStr, dateAdmission);
                    }
                }
            }
        }
    }

    inputFile.close();
}

int idxPatient = -1;
int idxSortStr = -1;

// ������ ���������, ���� ��� ��������, ���� �������� � ������� - ���������
bool operator==(const row& frst, const row& scnd) {
    if (frst.strValues[idxPatient] == scnd.strValues[idxPatient]) {
        if (frst.bithDate == scnd.bithDate) {
            if (frst.strValues[idxSortStr] == scnd.strValues[idxSortStr]) {
                return true;
            }
        }
    }

    return false;
}

// ���������� �� ����� ��������
bool operator<(const row& frst, const row& scnd) {
    return frst.strValues[idxPatient] < scnd.strValues[idxPatient];
}

// ��������� � ������ �������
void uniqueRows(vector<row>& table) {
    sort(table.begin(), table.end());
    table.resize(unique(table.begin(), table.end()) - table.begin());
}

// ���������� ������ ���������� ���������
void uniqueColumn(const vector<row>& tables, const vector<string>& nameColumn, vector<pair<string, vector<string>> >& uniqRows) {
    uniqRows.clear();
    uniqRows.resize(nameColumn.size());
    for (int numColumn = 0; numColumn < (int)nameColumn.size(); numColumn++) {
        uniqRows[numColumn].first = nameColumn[numColumn];
        vector<string>& currColumn = uniqRows[numColumn].second;

        //const row& currRow = tables[numRow];
        //elementInArray(diagnozes, currRow.diagnoz);
    }
}

// �������� ������� ���������, �� ������� �������
int getByColumnName(const vector<row>& tables, const string& nameColumn, const string& valueColumn, vector<int>& numsRows) {

    //for(int idx)

    //for (int numRow = 0; numRow < (int)tables.size(); numRow++) {
    //	const row& currRow = tables[numRow];
    //	if (currRow.diagnoz == diagnoz) {
    //		numsRows.push_back(numRow);
    //	}
    //}
    return numsRows.size();
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        cout << "Error input!" << endl;
        return 0;
    }

    setlocale(LC_ALL, "Russian");

    string path(argv[1]);

    table bigTable;
    for (auto& name : fs::directory_iterator(path)) {
        string currPath = name.path().string();
        readFile(currPath, bigTable);
    }

    cout << "All file is read." << endl;

    //uniqueRows(tables);

    //cout << "amount row = " << tables.size() << endl;

    //vector<string> diagnozes;
    ////uniqueColumn(tables, diagnozes);

    //vector<int> amounts(diagnozes.size(), 0);

    //for (int idxDiagnoz = 0; idxDiagnoz < (int)diagnozes.size(); idxDiagnoz++) {
    //	//cout << "������� = " << diagnozes[idxDiagnoz] << ", ���-�� ����������� �������: " << amountByDiagnoz(tables, diagnozes[idxDiagnoz]) << endl;
    //}
}