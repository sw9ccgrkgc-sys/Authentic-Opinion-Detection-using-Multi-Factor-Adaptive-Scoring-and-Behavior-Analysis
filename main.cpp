#include <iostream>
#include <string>
#include <map>

using namespace std;

struct Feedback {
    int userId;
    int productId;
    string text;
    int score;
    string signal;   // signal strength
};

// basic text cleaning (string processing)
string cleanText(string s) {
    string res = "";
    for (int i = 0; i < s.length(); i++) {
        char ch = s[i];

        if (ch >= 'A' && ch <= 'Z')
            ch += 32;

        if ((ch >= 'a' && ch <= 'z') || ch == ' ')
            res += ch;
    }
    return res;
}

// Linear Search → duplicate detection
bool isDuplicate(Feedback arr[], int n, string txt) {
    for (int i = 0; i < n; i++) {
        if (arr[i].text == txt)
            return true;
    }
    return false;
}

// Hashing → word frequency
map<string,int> getFreq(string txt) {
    map<string,int> m;
    string w = "";

    for (int i = 0; i < txt.length(); i++) {
        if (txt[i] != ' ') {
            w += txt[i];
        } else {
            if (w != "") m[w]++;
            w = "";
        }
    }
    if (w != "") m[w]++;

    return m;
}

// similarity (set based comparison using hashing)
int similarityPercent(string a, string b) {
    map<string,int> m1 = getFreq(a);
    map<string,int> m2 = getFreq(b);

    int common = 0, total = 0;

    for (auto it : m1) {
        total++;
        if (m2[it.first] > 0)
            common++;
    }

    if (total == 0) return 0;

    return (common * 100) / total;
}

// repetition penalty (hashing)
int repetitionPenalty(string txt) {
    map<string,int> m = getFreq(txt);
    int pen = 0;

    for (auto it : m) {
        if (it.second > 2)
            pen += (it.second - 2) * 2;
    }

    return pen;
}

// Linear Search → user behaviour
int userPenalty(Feedback arr[], int n, int uid, int pid) {
    int sameUser = 0, sameProduct = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i].userId == uid)
            sameUser++;

        if (arr[i].userId == uid && arr[i].productId == pid)
            sameProduct++;
    }

    int pen = 0;
    if (sameUser > 3) pen += 10;
    if (sameProduct > 1) pen += 10;

    return pen;
}

// Greedy scoring + signal strength
int calculateScore(Feedback arr[], int n, Feedback &f) {
    int score = 100;
    int signals = 0;

    if (isDuplicate(arr, n, f.text)) {
        score -= 40;
        signals++;
    }

    for (int i = 0; i < n; i++) {
        int sim = similarityPercent(f.text, arr[i].text);

        if (sim > 50) {
            score -= 20;
            signals++;
            break;
        }
    }

    int rep = repetitionPenalty(f.text);
    if (rep > 0) {
        score -= rep;
        signals++;
    }

    int up = userPenalty(arr, n, f.userId, f.productId);
    if (up > 0) {
        score -= up;
        signals++;
    }

    if (score < 0) score = 0;

    // signal strength classification
    if (signals >= 3)
        f.signal = "High";
    else if (signals == 2)
        f.signal = "Medium";
    else if (signals == 1)
        f.signal = "Low";
    else
        f.signal = "None";

    return score;
}

// Bubble Sort → sorting
void sortData(Feedback arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].score < arr[j + 1].score) {
                Feedback temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// display
void show(Feedback arr[], int n) {
    for (int i = 0; i < n; i++) {
        cout << "\nUser: " << arr[i].userId;
        cout << "\nProduct: " << arr[i].productId;
        cout << "\nFeedback: " << arr[i].text;
        cout << "\nScore: " << arr[i].score;
        cout << "\nSignal Strength: " << arr[i].signal;

        if (arr[i].score >= 75)
            cout << " (Genuine)";
        else if (arr[i].score >= 40)
            cout << " (Suspicious)";
        else
            cout << " (Fake)";

        cout << "\n";
    }
}

// Aggregation → product level stats
void productStats(Feedback arr[], int n) {
    for (int i = 0; i < n; i++) {
        int pid = arr[i].productId;

        int total = 0, genuine = 0, fake = 0, sum = 0;

        for (int j = 0; j < n; j++) {
            if (arr[j].productId == pid) {
                total++;
                sum += arr[j].score;

                if (arr[j].score >= 75) genuine++;
                else if (arr[j].score < 40) fake++;
            }
        }

        if (total > 0) {
            cout << "\nProduct " << pid;
            cout << "\nTotal Feedback: " << total;
            cout << "\nGenuine: " << genuine;
            cout << "\nFake: " << fake;

            int percent = (genuine * 100) / total;
            int avg = sum / total;

            cout << "\nAuthentic %: " << percent;
            cout << "\nAverage Score: " << avg;
            cout << "\n";
        }
    }
}

int main() {
    Feedback arr[100];
    int n, count = 0;

    cout << "Enter number of feedbacks: ";
    cin >> n;
    cin.ignore();

    for (int i = 0; i < n; i++) {
        Feedback f;

        cout << "\nUser ID: ";
        cin >> f.userId;

        cout << "Product ID: ";
        cin >> f.productId;
        cin.ignore();

        cout << "Enter feedback: ";
        getline(cin, f.text);

        f.text = cleanText(f.text);

        f.score = calculateScore(arr, count, f);

        arr[count++] = f;
    }

    sortData(arr, count);

    cout << "\nResults after analysis:\n";
    show(arr, count);

    cout << "\nProduct summary:\n";
    productStats(arr, count);

    return 0;
}
