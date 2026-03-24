#include <iostream>
#include <string>
#include <map>
#include <cmath>

using namespace std;

// stores each feedback
struct Feedback {
    int userId;
    int productId;
    string text;
    int score;
    string signal;
};

// stores trust score of users (memory)
map<int,int> userTrust;

// convert text to lowercase and remove unwanted characters
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

// checks if feedback is already present
bool isDuplicate(Feedback arr[], int n, string txt) {
    for (int i = 0; i < n; i++) {
        if (arr[i].text == txt)
            return true;
    }
    return false;
}

// creates frequency map of words
map<string,int> getFreq(string txt) {
    map<string,int> m;
    string word = "";

    for (int i = 0; i < txt.length(); i++) {
        if (txt[i] != ' ') {
            word += txt[i];
        } else {
            if (word != "") m[word]++;
            word = "";
        }
    }

    if (word != "") m[word]++;
    return m;
}

// calculates similarity percentage between two feedbacks
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

// checks repeated words
int repetitionPenalty(string txt) {
    map<string,int> m = getFreq(txt);
    int penalty = 0;

    for (auto it : m) {
        if (it.second > 2)
            penalty += (it.second - 2) * 2;
    }

    return penalty;
}

// checks how user behaves
int userPenalty(Feedback arr[], int n, int uid, int pid) {
    int sameUser = 0, sameProduct = 0;

    for (int i = 0; i < n; i++) {
        if (arr[i].userId == uid)
            sameUser++;

        if (arr[i].userId == uid && arr[i].productId == pid)
            sameProduct++;
    }

    int penalty = 0;

    if (sameUser > 3) penalty += 10;
    if (sameProduct > 1) penalty += 10;

    return penalty;
}

// main scoring logic
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

    // user trust memory
    if (userTrust[f.userId] < 30) {
        score -= 10;
        signals++;
    }

    if (score < 0) score = 0;

    // signal strength
    if (signals >= 3) f.signal = "High";
    else if (signals == 2) f.signal = "Medium";
    else if (signals == 1) f.signal = "Low";
    else f.signal = "None";

    return score;
}

// sorts feedback by score
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

// display feedback results
void show(Feedback arr[], int n) {

    for (int i = 0; i < n; i++) {

        cout << "\nUser: " << arr[i].userId;
        cout << "\nProduct: " << arr[i].productId;
        cout << "\nFeedback: " << arr[i].text;
        cout << "\nScore: " << arr[i].score;
        cout << "\nSignal: " << arr[i].signal;

        if (arr[i].score >= 75)
            cout << " (Genuine)";
        else if (arr[i].score >= 40)
            cout << " (Suspicious)";
        else
            cout << " (Fake)";

        cout << "\n";
    }
}

// shows product level stats
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

// finds unusual feedback (pattern detection)
void detectOutliers(Feedback arr[], int n) {

    for (int i = 0; i < n; i++) {

        int pid = arr[i].productId;
        int sum = 0, count = 0;

        for (int j = 0; j < n; j++) {
            if (arr[j].productId == pid) {
                sum += arr[j].score;
                count++;
            }
        }

        if (count > 0) {
            int avg = sum / count;

            if (abs(arr[i].score - avg) > 30) {
                cout << "\nOutlier detected -> User "
                     << arr[i].userId << " on Product "
                     << pid;
            }
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

        // update trust
        if (f.score >= 75)
            userTrust[f.userId] += 2;
        else if (f.score < 40)
            userTrust[f.userId] -= 5;

        if (userTrust[f.userId] > 100) userTrust[f.userId] = 100;
        if (userTrust[f.userId] < 0) userTrust[f.userId] = 0;

        arr[count++] = f;
    }

    sortData(arr, count);

    cout << "\nResults:\n";
    show(arr, count);

    cout << "\nProduct Summary:\n";
    productStats(arr, count);

    cout << "\nPattern Detection:\n";
    detectOutliers(arr, count);

    return 0;
}
